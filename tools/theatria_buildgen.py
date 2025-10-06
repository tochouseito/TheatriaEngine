#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
theatria_buildgen.py
  - Parse a Visual Studio solution (.sln) and/or explicit .vcxproj/.csproj files
  - Generate a unified YAML (theatria.build.yml) for TheatriaEngine's multi-generator pipeline
  - Generate a .editorconfig file with requested charset rules

Usage examples:
  python theatria_buildgen.py --sln TheatriaEngine.sln -o theatria.build.yml --editorconfig .editorconfig
  python theatria_buildgen.py projects\ChoEngine.vcxproj projects\Editor.csproj --workspace TheatriaEngine

Notes:
  - Requires: PyYAML  (pip install pyyaml)
  - Startup project is not stored in .sln; pass --startup if you want to set it in YAML.
"""

import re, sys, argparse
from pathlib import Path
import xml.etree.ElementTree as ET

try:
    import yaml
except ImportError:
    yaml = None

SLN_GUID_SOLUTION_FOLDER = "{66A26720-8FB5-11D2-AA7E-00C04F688DDE}"

# ------------------------- Utilities -------------------------

def split_list(s, sep=';'):
    if not s: return []
    return [x.strip() for x in s.replace('\\n', ';').split(sep) if x.strip()]

def msbuild_expand_vars(s: str):
    if not s: return s
    s = s.replace('$(Configuration)', '${config}')
    s = s.replace('$(Platform)', '${platform}')
    s = s.replace('$(ProjectName)', '${target}')
    s = s.replace('$(TargetName)', '${target}')
    s = s.replace('$(TargetDir)', '${target_dir}')
    s = s.replace('$(IntDir)', '${obj_dir}')
    s = s.replace('$(OutDir)', '${target_dir}')
    return s

# ------------------------- .sln parsing -------------------------

def parse_sln(sln_path: Path):
    """
    Parse .sln to collect project paths and (optionally) solution folders.
    Return: (project_paths:list[Path], folders: dict[folder_name] = [project_names])
    """
    txt = sln_path.read_text(encoding='utf-8', errors='ignore')
    proj_re = re.compile(r'^Project\("\{[^}]+\}"\)\s*=\s*"([^"]+)",\s*"([^"]+)",\s*"\{([^}]+)\}"', re.MULTILINE)
    type_guid_re = re.compile(r'^Project\("\{([^}]+)\}"\)\s*=\s*"([^"]+)",\s*"([^"]+)",\s*"\{([^}]+)\}"', re.MULTILINE)

    # First pass: collect all project entries and type GUIDs
    projects = []  # list of dict: {name, relpath, guid, type_guid}
    for m in type_guid_re.finditer(txt):
        type_guid, name, relpath, guid = m.groups()
        projects.append({'type_guid': '{'+type_guid+'}', 'name': name, 'relpath': relpath.replace('\\','/'), 'guid': '{'+guid+'}'})
    # Map guid -> name, path, type
    by_guid = {p['guid']: p for p in projects}

    # Solution folders
    folder_guids = {p['guid']: p for p in projects if p['type_guid'].upper() == SLN_GUID_SOLUTION_FOLDER.upper()}

    # NestedProjects section provides parent-child mapping
    nested = {}
    for sec in re.finditer(r'GlobalSection\(NestedProjects\)\s*=\s*preSolution(.*?)EndGlobalSection', txt, re.DOTALL):
        body = sec.group(1)
        for line in body.splitlines():
            m = re.search(r'\{([0-9A-Fa-f\-]+)\}\s*=\s*\{([0-9A-Fa-f\-]+)\}', line)
            if m:
                child = '{'+m.group(1)+'}'
                parent = '{'+m.group(2)+'}'
                nested[child] = parent

    # Build folder -> [project names] mapping
    folders = {}
    for guid, parent in nested.items():
        if guid in folder_guids:
            # A folder nested under another folder (we don't need hierarchy beyond name mapping here)
            continue
        # child is a project, parent must be a folder
        if parent in folder_guids and guid in by_guid:
            folder_name = folder_guids[parent]['name']
            proj_name = by_guid[guid]['name']
            folders.setdefault(folder_name, []).append(proj_name)

    # Collect project file paths (exclude solution folders)
    proj_paths = []
    for p in projects:
        if p['type_guid'].upper() == SLN_GUID_SOLUTION_FOLDER.upper():
            continue
        rel = p['relpath']
        proj_paths.append((p['name'], sln_path.parent.joinpath(rel).resolve()))

    return proj_paths, folders

# ------------------------- .vcxproj parsing -------------------------

def parse_vcxproj(path: Path):
    ns = {'msb': 'http://schemas.microsoft.com/developer/msbuild/2003'}
    tree = ET.parse(path)
    root = tree.getroot()
    name = root.findtext('msb:PropertyGroup/msb:ProjectName', default=path.stem, namespaces=ns) or path.stem

    sources = set()
    headers = set()
    for it in root.findall('msb:ItemGroup/msb:ClCompile', ns):
        inc = it.attrib.get('Include')
        if inc: sources.add(str(Path(path.parent, inc)).replace('\\','/'))
    for it in root.findall('msb:ItemGroup/msb:ClInclude', ns):
        inc = it.attrib.get('Include')
        if inc: headers.add(str(Path(path.parent, inc)).replace('\\','/'))

    include_dirs = set()
    disable_warnings = set()
    warning_level = None
    multiproc = None
    runtime = None
    pch_header = None
    pch_source = None
    extra_compile = set()
    extra_link = set()
    outdir = None
    intdir = None

    # pch create source
    for it in root.findall('msb:ItemGroup/msb:ClCompile', ns):
        pre = it.find('msb:PrecompiledHeader', ns)
        if pre is not None and (pre.text or '').lower() == 'create':
            pch_source = str(Path(path.parent, it.attrib.get('Include',''))).replace('\\','/')

    for idg in root.findall('msb:ItemDefinitionGroup', ns):
        cl = idg.find('msb:ClCompile', ns)
        if cl is not None:
            incs = cl.findtext('msb:AdditionalIncludeDirectories', default='', namespaces=ns)
            include_dirs.update([x for x in split_list(incs) if x and x != '%(AdditionalIncludeDirectories)'])
            wl = cl.findtext('msb:WarningLevel', default='', namespaces=ns)
            if wl:
                mapping = {'Level0':0,'Level1':1,'Level2':2,'Level3':3,'Level4':4,'EnableAllWarnings':'all'}
                warning_level = mapping.get(wl, warning_level)
            mp = cl.findtext('msb:MultiProcessorCompilation', default='', namespaces=ns)
            if mp: multiproc = (mp.lower() == 'true')
            rl = cl.findtext('msb:RuntimeLibrary', default='', namespaces=ns)
            if rl:
                mapping = {'MultiThreaded':'MT','MultiThreadedDebug':'MTd','MultiThreadedDLL':'MD','MultiThreadedDebugDLL':'MDd'}
                runtime = mapping.get(rl, runtime)
            pch = cl.findtext('msb:PrecompiledHeader', default='', namespaces=ns)
            if pch and pch.lower() != 'notusing':
                ph = cl.findtext('msb:PrecompiledHeaderFile', default='', namespaces=ns)
                if ph: pch_header = str(Path(path.parent, ph)).replace('\\','/')
            ao = cl.findtext('msb:AdditionalOptions', default='', namespaces=ns)
            if ao:
                for tok in ao.split():
                    if tok not in ('%(AdditionalOptions)',):
                        extra_compile.add(tok)
            dw = cl.findtext('msb:DisableSpecificWarnings', default='', namespaces=ns)
            if dw:
                for w in split_list(dw):
                    if w: disable_warnings.add(w)

        lk = idg.find('msb:Link', ns)
        if lk is not None:
            ao = lk.findtext('msb:AdditionalOptions', default='', namespaces=ns)
            if ao:
                for tok in ao.split():
                    if tok not in ('%(AdditionalOptions)',):
                        extra_link.add(tok)

    for pg in root.findall('msb:PropertyGroup', ns):
        od = pg.findtext('msb:OutDir', default=None, namespaces=ns)
        if od and not outdir:
            outdir = msbuild_expand_vars(str(Path(path.parent, od)).replace('\\','/'))
        idr = pg.findtext('msb:IntDir', default=None, namespaces=ns)
        if idr and not intdir:
            intdir = msbuild_expand_vars(str(Path(path.parent, idr)).replace('\\','/'))

    include_dirs = [msbuild_expand_vars(str(Path(path.parent, x)).replace('\\','/')) for x in sorted(include_dirs) if x and x != '.']

    target = {
        'name': name,
        'type': 'StaticLib',
        'sources': sorted(list(sources | headers)),
        'include_dirs': include_dirs,
    }
    # ConfigurationType
    conf_type = None
    for pg in root.findall('msb:PropertyGroup', ns):
        ct = pg.findtext('msb:ConfigurationType', default=None, namespaces=ns)
        if ct:
            conf_type = ct
            break
    if conf_type:
        mapping = {'Application':'ConsoleApp', 'DynamicLibrary':'SharedLib', 'StaticLibrary':'StaticLib'}
        target['type'] = mapping.get(conf_type, target['type'])

    msvc = {}
    if warning_level is not None: msvc['warning_level'] = warning_level
    if multiproc is not None: msvc['multiprocessor'] = multiproc
    if runtime is not None: msvc['runtime'] = runtime
    if disable_warnings: msvc['disable_warnings'] = sorted(list(disable_warnings))
    if extra_compile: msvc['extra_compile_options'] = sorted(list(extra_compile))
    if extra_link: msvc['extra_link_options'] = sorted(list(extra_link))
    if msvc: target['msvc'] = msvc

    if pch_header:
        target['pch'] = {'enable': True, 'header': pch_header}
        if pch_source: target['pch']['source'] = pch_source

    build = {}
    if outdir: build['target_dir'] = outdir
    if intdir: build['obj_dir'] = intdir
    if build: target['build'] = build

    return target

# ------------------------- .csproj parsing -------------------------

def parse_csproj(path: Path):
    tree = ET.parse(path)
    root = tree.getroot()
    name = path.stem

    sources = []
    for it in root.findall('.//ItemGroup/Compile'):
        inc = it.attrib.get('Include')
        if inc:
            sources.append(str(Path(path.parent, inc)).replace('\\','/'))
    # If none, glob default
    if not sources:
        sources = [str(Path(path.parent, '**/*.cs')).replace('\\','/')]

    tfm = None
    outdir = None
    intdir = None
    langver = None
    nullable = None
    allow_unsafe = None
    output_type = None

    for pg in root.findall('PropertyGroup'):
        tfm = tfm or pg.findtext('TargetFramework')
        outdir = outdir or pg.findtext('OutputPath')
        intdir = intdir or pg.findtext('IntermediateOutputPath')
        langver = langver or pg.findtext('LangVersion')
        nullable = nullable or pg.findtext('Nullable')
        aus = pg.findtext('AllowUnsafeBlocks')
        if aus is not None and allow_unsafe is None:
            allow_unsafe = (aus.lower() == 'true')
        output_type = output_type or pg.findtext('OutputType')

    proj_refs = []
    for pr in root.findall('.//ItemGroup/ProjectReference'):
        inc = pr.attrib.get('Include')
        if inc:
            proj_refs.append(Path(inc).stem)

    packages = []
    for pk in root.findall('.//ItemGroup/PackageReference'):
        pid = pk.attrib.get('Include') or pk.findtext('Include')
        ver = pk.attrib.get('Version') or pk.findtext('Version')
        if pid:
            packages.append({'id': pid, 'version': ver or ''})

    target = {
        'name': name,
        'type': 'CsWindowedApp',
        'sources': sources,
        'cs': {}
    }
    if output_type:
        mapping = {'Exe':'CsConsoleApp', 'WinExe':'CsWindowedApp', 'Library':'CsClassLib'}
        target['type'] = mapping.get(output_type, target['type'])

    cs = target['cs']
    if tfm: cs['target_framework'] = tfm
    if langver: cs['lang_version'] = langver
    if nullable: cs['nullable'] = nullable
    if allow_unsafe is not None: cs['allow_unsafe'] = allow_unsafe
    if proj_refs: target['links'] = proj_refs
    if packages: cs['packages'] = packages

    build = {}
    if outdir: build['target_dir'] = msbuild_expand_vars(str(Path(path.parent, outdir)).replace('\\','/'))
    if intdir: build['obj_dir'] = msbuild_expand_vars(str(Path(path.parent, intdir)).replace('\\','/'))
    if build: target['build'] = build

    return target

# ------------------------- EditorConfig generation -------------------------

def emit_editorconfig(minimal=False, bom=False):
    # Always include user's requested section
    charset = 'utf-8-bom' if bom else 'utf-8'
    core = [
        "root = true",
        "",
        "[*.{cpp,h,hlsl,hlsli}]",
        f"charset = {charset}",
    ]
    if minimal:
        return "\n".join(core) + "\n"
    # Enriched template
    extra = [
        "",
        "[*]",
        "end_of_line = crlf",
        "insert_final_newline = true",
        "trim_trailing_whitespace = true",
        "indent_style = space",
        "indent_size = 4",
        "charset = utf-8",
        "",
        "[*.{hpp,hxx,ixx,inl}]",
        f"charset = {charset}",
        "indent_size = 4",
        "tab_width = 4",
        "",
        "[*.{json,yml,yaml}]",
        "indent_size = 2",
        "",
        "[*.md]",
        "trim_trailing_whitespace = false",
    ]
    return "\n".join(core + extra) + "\n"

# ------------------------- Main -------------------------

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--sln', help='.sln file to parse (optional)')
    ap.add_argument('projects', nargs='*', help='.vcxproj/.csproj files (optional if --sln provided)')
    ap.add_argument('-o', '--output', default='theatria.build.yml', help='YAML output path')
    ap.add_argument('--workspace', default='Theatria', help='workspace/solution name in YAML')
    ap.add_argument('--startup', default=None, help='startup project name for YAML solution section')
    ap.add_argument('--editorconfig', default=None, help='path to write .editorconfig (optional)')
    ap.add_argument('--editorconfig-minimal', action='store_true', help='write only the requested charset section')
    ap.add_argument('--editorconfig-bom', action='store_true', help='use utf-8-bom for C++/HLSL section')
    args = ap.parse_args()

    # Collect project paths
    proj_paths = []   # list[Path]
    folders = {}      # solution folders: name -> [project names]

    if args.sln:
        sln_path = Path(args.sln).resolve()
        if not sln_path.exists():
            print(f'[error] .sln not found: {sln_path}', file=sys.stderr)
            sys.exit(2)
        discovered, folders = parse_sln(sln_path)
        proj_paths.extend([p for (_, p) in discovered])

    for p in args.projects:
        proj_paths.append(Path(p).resolve())

    if not proj_paths:
        print('[error] no projects specified. Pass --sln and/or project paths.', file=sys.stderr)
        sys.exit(2)

    # Parse each project
    targets = []
    for p in proj_paths:
        if not p.exists():
            print(f'[warn] project not found: {p}', file=sys.stderr)
            continue
        ext = p.suffix.lower()
        try:
            if ext == '.vcxproj':
                t = parse_vcxproj(p)
            elif ext == '.csproj':
                t = parse_csproj(p)
            else:
                print(f'[warn] unsupported project type: {p}', file=sys.stderr)
                continue
            targets.append(t)
        except Exception as e:
            print(f'[error] failed to parse {p}: {e}', file=sys.stderr)

    # Top-level YAML
    data = {
        'workspace': args.workspace,
        'solution': {
            'name': args.workspace,
        },
        'build': {
            'target_dir': 'build/bin/${config}/${target}',
            'obj_dir':    'build/obj/${config}/${target}',
        },
        'msvc': {
            'multiprocessor': True,
            'common_defines': ['UNICODE','_UNICODE','NOMINMAX','_CRT_SECURE_NO_WARNINGS'],
            'compile_options': ['/permissive-','/Zc:preprocessor','/EHsc'],
        },
        'configs': [
            {'name':'Debug','defines':['DEBUG'],'symbols':True,'optimize':'None','msvc':{'runtime':'MDd','warning_level':4}},
            {'name':'Release','defines':['NDEBUG'],'symbols':False,'optimize':'Speed','msvc':{'runtime':'MD','warning_level':4}},
        ],
        'targets': targets
    }
    if args.startup:
        data['solution']['startup_project'] = args.startup
    if folders:
        # keep only folders that actually have known targets
        tnames = set(t['name'] for t in targets)
        flist = []
        for fname, plist in folders.items():
            keep = [p for p in plist if p in tnames]
            if keep:
                flist.append({'name': fname, 'targets': keep})
        if flist:
            data['solution']['folders'] = flist

    # YAML dump
    if yaml is None:
        print('[error] PyYAML is not installed. pip install pyyaml', file=sys.stderr)
        sys.exit(3)
    out_path = Path(args.output)
    out_path.write_text(
        yaml.safe_dump(data, sort_keys=False, allow_unicode=True),
        encoding='utf-8'
    )
    print(f'[ok] YAML written: {out_path} (targets: {len(targets)})')

    # .editorconfig
    if args.editorconfig:
        ec = emit_editorconfig(minimal=args.editorconfig_minimal, bom=args.editorconfig_bom)
        Path(args.editorconfig).write_text(ec, encoding='utf-8')
        print(f'[ok] .editorconfig written: {args.editorconfig}')

if __name__ == '__main__':
    main()
