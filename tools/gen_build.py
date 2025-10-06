#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
gen_build.py (safe-lua)
  YAML -> CMakeLists.txt & premake5.lua generator with multiline Lua tables (less quoting issues)

Usage:
  python gen_build.py theatria.build.yml
"""

import sys, os
from pathlib import Path
import yaml
import textwrap

def esc(s: str) -> str:
    return s.replace('"','\\"')

def cmake_globs(globs):
    lines = ["set(srcs)"]
    for pat in globs:
        lines.append(f'file(GLOB_RECURSE tmp "{esc(pat)}")')
        lines.append("list(APPEND srcs ${tmp})")
    return "\n".join(lines)

def map_c_target_type(t):
    if t == "StaticLib": return ("add_library", "STATIC", False)
    if t == "SharedLib": return ("add_library", "SHARED", False)
    if t == "ConsoleApp": return ("add_executable", "", False)
    if t == "WindowedApp": return ("add_executable", "", True)
    if t == "CsConsoleApp": return ("add_executable", "", False)
    if t == "CsWindowedApp": return ("add_executable", "", True)
    if t == "CsClassLib": return ("add_library", "MODULE", False)
    raise ValueError(f"unknown type: {t}")

def cmake_warning_level(w):
    if w == "all": return "/Wall"
    try:
        w = int(w)
        return f"/W{w}" if 0 <= w <= 4 else None
    except:
        return None

def runtime_to_cmake(rt):
    m = {"MD":"MultiThreadedDLL","MDd":"MultiThreadedDebugDLL","MT":"MultiThreaded","MTd":"MultiThreadedDebug"}
    return m.get(rt, None)

def premake_kind(t):
    return {
        "StaticLib":"StaticLib","SharedLib":"SharedLib","ConsoleApp":"ConsoleApp","WindowedApp":"WindowedApp",
        "CsConsoleApp":"ConsoleApp","CsWindowedApp":"WindowedApp","CsClassLib":"SharedLib"
    }[t]

def is_csharp(t): return t.startswith("Cs")

def emit_cmake(cfg: dict) -> str:
    ws = cfg.get("workspace","TheatriaEngine")
    std = cfg.get("cpp_standard", 20)
    cstd = cfg.get("c_standard", None)
    msvc = cfg.get("msvc", {})
    build = cfg.get("build", {})
    solution = cfg.get("solution", {})

    out = []
    out.append("cmake_minimum_required(VERSION 3.23)")
    out.append(f'project({ws} LANGUAGES CXX CSharp)')
    out.append(f"set(CMAKE_CXX_STANDARD {std})")
    out.append("set(CMAKE_CXX_STANDARD_REQUIRED ON)")
    if cstd:
        out.append("enable_language(C)")
        out.append(f"set(CMAKE_C_STANDARD {cstd})")
        out.append("set(CMAKE_C_STANDARD_REQUIRED ON)")
    out.append("")
    out.append('set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")')
    out.append("")
    out.append("if(MSVC)")
    if msvc.get("multiprocessor", False): out.append("  add_compile_options(/MP)")
    wl = msvc.get("warning_level")
    if wl:
        wflag = cmake_warning_level(wl)
        if wflag: out.append(f"  add_compile_options({wflag})")
    copts = msvc.get("compile_options", [])
    if copts: out.append(f"  add_compile_options({' '.join(copts)})")
    cdefs = msvc.get("common_defines", [])
    if cdefs: out.append("  add_definitions(" + " ".join([f"-D{d}" for d in cdefs]) + ")")
    ex_c = msvc.get("extra_compile_options", [])
    if ex_c: out.append(f"  add_compile_options({' '.join(ex_c)})")
    ex_l = msvc.get("extra_link_options", [])
    if ex_l: out.append(f"  add_link_options({' '.join(ex_l)})")
    out.append("endif()")
    out.append("")
    for c in cfg.get("configs", []):
        name = c["name"]
        defs = c.get("defines", [])
        if defs: out.append(f"add_compile_definitions($<$<CONFIG:{name}>:{' '.join(defs)}>)")
        m = c.get("msvc", {})
        rt = runtime_to_cmake(m.get("runtime"))
        if rt: out.append(f'set(CMAKE_MSVC_RUNTIME_LIBRARY "{rt}$<$<CONFIG:Debug>:Debug>")')
        wl = m.get("warning_level")
        if wl is not None:
            wflag = cmake_warning_level(wl)
            if wflag: out.append(f"add_compile_options($<$<CONFIG:{name}>:{wflag}>)")
        if m.get("treat_warnings_as_errors", False):
            out.append(f"add_compile_options($<$<CONFIG:{name}>:/WX>)")
    out.append("")
    out.append("set_property(GLOBAL PROPERTY USE_FOLDERS ON)")
    if solution.get("startup_project"):
        out.append(f'set_property(DIRECTORY ${{CMAKE_CURRENT_SOURCE_DIR}} PROPERTY VS_STARTUP_PROJECT {solution["startup_project"]})')
    out.append("")
    build_global = cfg.get("build", {})
    for t in cfg.get("targets", []):
        name = t["name"]; ttype = t["type"]
        kind, cm_kind, win32 = map_c_target_type(ttype)
        out.append(f"# ---- {name} ----")
        if is_csharp(ttype): out.append("enable_language(CSharp)")
        out.append(cmake_globs(t.get("sources", [])))
        if kind == "add_library":
            out.append(f'{kind}({name} {cm_kind} ${{srcs}})')
        else:
            out.append(f'{kind}({name} {"WIN32 " if win32 else ""}${{srcs}})'.replace("  "," "))
        incs = t.get("include_dirs", [])
        if incs:
            out.append(f"target_include_directories({name} PUBLIC " + " ".join([f'"{esc(p)}"' for p in incs]) + ")")
        links = t.get("links", [])
        if links: out.append(f"target_link_libraries({name} PUBLIC {' '.join(links)})")
        tm = t.get("msvc", {})
        if tm.get("disable_warnings"):
            out.append(f"target_compile_options({name} PRIVATE " + " ".join([f"/wd{w}" for w in tm["disable_warnings"]]) + ")")
        if tm.get("extra_compile_options"): out.append(f"target_compile_options({name} PRIVATE {' '.join(tm['extra_compile_options'])})")
        if tm.get("extra_link_options"): out.append(f"target_link_options({name} PRIVATE {' '.join(tm['extra_link_options'])})")
        tgt_build = {**build_global, **t.get("build", {})} if build_global or t.get("build") else {}
        if tgt_build.get("target_dir"):
            td = esc(tgt_build["target_dir"])
            out.append(f'set_target_properties({name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "{td}")')
            out.append(f'set_target_properties({name} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "{td}")')
            out.append(f'set_target_properties({name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "{td}")')
        wd = t.get("working_directory") or cfg.get("working_directory")
        if wd: out.append(f'set_property(TARGET {name} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "{esc(wd)}")')
        pch = t.get("pch")
        if pch and pch.get("enable", True) and pch.get("header"):
            out.append(f'target_precompile_headers({name} PRIVATE "{esc(pch["header"])}")')
            ex = pch.get("exclude", [])
            if ex:
                out.append("file(GLOB_RECURSE pch_excl")
                for pat in ex: out.append(f'  "{esc(pat)}"')
                out.append(")")
                out.append("set_source_files_properties(${pch_excl} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)")
            if pch.get("force_include", False):
                out.append(f'target_compile_options({name} PRIVATE "/FI{esc(pch["header"])}")')
        if is_csharp(ttype):
            cs = t.get("cs", {})
            tfm = cs.get("target_framework")
            if tfm: out.append(f'set_property(TARGET {name} PROPERTY DOTNET_TARGET_FRAMEWORK "{tfm}")')
            lv = cs.get("lang_version")
            if lv: out.append(f'set_property(TARGET {name} PROPERTY VS_GLOBAL_LangVersion "{lv}")')
            nullable = cs.get("nullable")
            if nullable: out.append(f'set_property(TARGET {name} PROPERTY VS_GLOBAL_Nullable "{nullable}")')
            if cs.get("allow_unsafe", False): out.append(f'set_property(TARGET {name} PROPERTY VS_GLOBAL_AllowUnsafeBlocks "true")')
        for pb in t.get("postbuild_copy_dirs", []):
            src = esc(pb["from"]); dst = esc(pb["to"])
            out.append(textwrap.dedent(f"""
            add_custom_command(TARGET {name} POST_BUILD
              COMMAND ${{CMAKE_COMMAND}} -E copy_directory "{src}" "$<TARGET_FILE_DIR:{name}>/{dst}")
            """).strip())
        out.append("")
    for f in solution.get("folders", []):
        fn = f["name"]
        for tn in f.get("targets", []):
            out.append(f'set_target_properties({tn} PROPERTIES FOLDER "{esc(fn)}")')
    out.append("")
    return "\n".join(out) + "\n"

def emit_lua_list(name: str, items):
    if not items: return []
    lines = [f'  {name} {{']
    for it in items:
        lines.append(f'    "{it}",')
    lines.append('  }')
    return lines

def map_optimize(val: str):
    if not val: return None
    v = str(val).lower()
    if v in ('none','off','0','false'): return 'Off'
    if v in ('on','true'): return 'On'
    if v in ('debug',): return 'Debug'
    if v in ('speed','fast','performance'): return 'Speed'
    if v in ('full','aggressive','max'): return 'Full'
    if v in ('size','small'): return 'Size'
    return val

def emit_premake(cfg: dict) -> str:
    ws = cfg.get("workspace","Theatria")
    platforms = cfg.get("platforms", ["x64"])
    configs = cfg.get("configs", [])
    msvc = cfg.get("msvc", {})
    build = cfg.get("build", {})
    solution = cfg.get("solution", {})

    out = []
    out.append(f'workspace "{ws}"')
    out.append('  location "../project"')
    cfg_names = [c["name"] for c in configs]
    plat_names = platforms
    out.extend(emit_lua_list("configurations", cfg_names))
    out.extend(emit_lua_list("platforms", plat_names))
    if solution.get("startup_project"):
        out.append(f'  startproject "{solution["startup_project"]}"')
    out.append("")
    out.append('  filter { "platforms:x64" }')
    out.append('    architecture "x86_64"')
    out.append('  filter {}')
    out.append("")
    for c in configs:
        out.append(f'  filter {{ "configurations:{c["name"]}" }}')
        if c.get("symbols", False): out.append('    symbols "On"')
        opt = map_optimize(c.get("optimize",""))
        if opt: out.append(f'    optimize "{opt}"')
        defs = c.get("defines", [])
        if defs:
            out.append('    defines {')
            for d in defs: out.append(f'      "{d}",')
            out.append('    }')
        cm = c.get("msvc", {})
        wl = cm.get("warning_level")
        if wl == "all": out.append('    warnings "Extra"')
        elif isinstance(wl, int): out.append('    warnings "High"' if wl >= 4 else '    warnings "Default"')
        if cm.get("treat_warnings_as_errors", False): out.append('    flags { "FatalWarnings" }')
        rt = cm.get("runtime")
        if rt in ("MT","MTd"):
            out.append('    staticruntime "On"')
            out.append('    runtime "Debug"' if rt == "MTd" else '    runtime "Release"')
        elif rt in ("MD","MDd"):
            out.append('    staticruntime "Off"')
            out.append('    runtime "Debug"' if rt == "MDd" else '    runtime "Release"')
        out.append('  filter {}')
    out.append("")
    out.append('  flags { "MultiProcessorCompile" }' if msvc.get("multiprocessor", True) else '  -- no /MP')
    out.append('  characterset "Unicode"')
    out.append(f'  cppdialect "C++{cfg.get("cpp_standard",20)}"')
    if msvc.get("common_defines"):
        out.extend(emit_lua_list("defines", msvc["common_defines"]))
    if msvc.get("compile_options"):
        out.extend(emit_lua_list("buildoptions", msvc["compile_options"]))
    if msvc.get("extra_compile_options"):
        out.extend(emit_lua_list("buildoptions", msvc["extra_compile_options"]))
    if msvc.get("extra_link_options"):
        out.extend(emit_lua_list("linkoptions", msvc["extra_link_options"]))
    out.append("")
    groups = {}
    for f in solution.get("folders", []):
        for t in f.get("targets", []):
            groups[t] = f["name"]
    for t in cfg.get("targets", []):
        name = t["name"]; ttype = t["type"]
        out.append(f'project "{name}"')
        out.append(f'  kind "{premake_kind(ttype)}"')
        out.append(f'  language {"\"C#\"" if is_csharp(ttype) else "\"C++\""}')
        out.append('  location ("../project/%{prj.name}")')
        g = groups.get(name)
        if g: out.append(f'  group "{g}"')
        tgt_build = {**build, **t.get("build", {})} if build or t.get("build") else {}
        tdir = tgt_build.get("target_dir", "%{wks.location}/bin/%{cfg.buildcfg}/%{prj.name}")
        odir = tgt_build.get("obj_dir",    "%{wks.location}/obj/%{cfg.buildcfg}/%{prj.name}")
        out.append(f'  targetdir ("{tdir}")')
        out.append(f'  objdir    ("{odir}")')
        out.append('  debugdir  "%{cfg.targetdir}"')
        files = t.get("sources", [])
        if files:
            out.append("  files {")
            for p in files: out.append(f'    "{p}",')
            out.append("  }")
        incs = t.get("include_dirs", [])
        if incs and not is_csharp(ttype):
            out.extend(emit_lua_list("includedirs", incs))
        links = t.get("links", [])
        if links:
            out.extend(emit_lua_list("links", links))
        tm = t.get("msvc", {})
        if tm.get("disable_warnings"):
            out.extend(emit_lua_list("disablewarnings", [str(w) for w in tm["disable_warnings"]]))
        if tm.get("extra_compile_options"):
            out.extend(emit_lua_list("buildoptions", tm["extra_compile_options"]))
        if tm.get("extra_link_options"):
            out.extend(emit_lua_list("linkoptions", tm["extra_link_options"]))
        pch = t.get("pch")
        if pch and pch.get("enable", True) and pch.get("header") and not is_csharp(ttype):
            out.append(f'  pchheader "{pch["header"]}"')
            if pch.get("source"): out.append(f'  pchsource "{pch["source"]}"')
            if pch.get("exclude"):
                for pat in pch["exclude"]:
                    out.append(f'  filter {{ "files:{pat}" }}')
                    out.append('    flags { "NoPCH" }')
                out.append('  filter {}')
            if pch.get("force_include", False):
                out.extend(emit_lua_list("buildoptions", [f'/FI{pch["header"]}']))
        if is_csharp(ttype):
            cs = t.get("cs", {})
            tfm = cs.get("target_framework")
            if tfm:
                out.append(f'  dotnetframework "{tfm}"')
        for pb in t.get("postbuild_copy_dirs", []):
            src = pb["from"]; dst = pb["to"]
            out.append('  postbuildcommands {')
            out.append(f'    \'{{COPYDIR}} "{src}" "%{{cfg.targetdir}}/{dst}"\',')
            out.append('  }')
        out.append("")
    return "\n".join(out) + "\n"

def main():
    if len(sys.argv) < 2:
        print("Usage: gen_build.py <theatria.build.yml>")
        sys.exit(1)
    yml = Path(sys.argv[1])
    data = yaml.safe_load(yml.read_text(encoding="utf-8"))
    root = Path.cwd()
    (root / "CMakeLists.txt").write_text(emit_cmake(data), encoding="utf-8")
    (root / "premake5.lua").write_text(emit_premake(data), encoding="utf-8")
    print(f"[ok] wrote CMakeLists.txt and premake5.lua in {root}")

if __name__ == "__main__":
    main()
