using EnvDTE;
using EnvDTE80;
using Microsoft.VisualStudio.VCProjectEngine;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Text;
using System.Threading;

class Program
{
    static System.Diagnostics.Process FindEditorProcess()
    {
        foreach (var p in System.Diagnostics.Process.GetProcessesByName("ChoEditor"))
        {
            return p; // ChoEditor.exe の最初の1つを返す
        }
        return null;
    }

    // ソリューション内からプロジェクトを名前で探す（ソリューションフォルダ対応）
    static Project FindProjectByName(DTE2 dte, string projName)
    {
        foreach (Project p in dte.Solution.Projects)
        {
            var found = FindProjectRecursive(p, projName);
            if (found != null) return found;
        }
        return null;
    }
    static Project FindProjectRecursive(Project p, string projName)
    {
        if (p == null) return null;
        if (string.Equals(p.Name, projName, StringComparison.OrdinalIgnoreCase))
            return p;

        if (p.Kind == EnvDTE80.ProjectKinds.vsProjectKindSolutionFolder && p.ProjectItems != null)
        {
            foreach (ProjectItem item in p.ProjectItems)
            {
                var sub = item.SubProject;
                var found = FindProjectRecursive(sub, projName);
                if (found != null) return found;
            }
        }
        return null;
    }

    // フィルタを確実に用意
    static VCFilter EnsureFilter(VCProject vcproj, string filterName)
    {
        foreach (VCFilter f in vcproj.Filters)
            if (string.Equals(f.Name, filterName, StringComparison.OrdinalIgnoreCase))
                return f;
        return vcproj.AddFilter(filterName);
    }
    static string GetTemplateDir()
    {
        // Watcher.exe と同じ場所の Templates フォルダ
        var exePath = System.Reflection.Assembly.GetExecutingAssembly().Location;
        var dir = Path.GetDirectoryName(exePath);
        if (string.IsNullOrEmpty(dir))
            dir = AppDomain.CurrentDomain.BaseDirectory; // 予備
        return Path.Combine(dir, "Cho/Resources/EngineAssets/TemplateScript");
    }

    static string GetTemplatePath(string fileName)
        => Path.Combine(GetTemplateDir(), fileName);

    // テンプレ読込（UTF-8/BOM/他 だいたいOK）
    static string LoadTemplateText(string fileName)
    {
        var path = GetTemplatePath(fileName);
        if (!File.Exists(path)) throw new FileNotFoundException("Template not found", path);
        return File.ReadAllText(path, Encoding.UTF8);
    }

    // { SCRIPT_NAME } をクラス名に置換
    static string ApplyScriptName(string text, string className)
        => text.Replace("{ SCRIPT_NAME }", className);

    // 新規/空のときだけテンプレを書き込む
    static bool WriteFromTemplateIfEmpty(string absPath, string templName, string className)
    {
        var parent = Path.GetDirectoryName(absPath);
        if (string.IsNullOrEmpty(parent))
            throw new ArgumentException("Invalid path: " + absPath);

        Directory.CreateDirectory(parent);

        if (File.Exists(absPath) && new FileInfo(absPath).Length > 0)
            return false;

        var src = LoadTemplateText(templName);
        var outText = ApplyScriptName(src, className);

        using (var sw = new StreamWriter(absPath, false, new UTF8Encoding(true)))
            sw.Write(outText);

        return true;
    }
    static string GetProjectDirectory(Project p)
    {
        // p.FullName は .vcxproj のフルパス
        if (!string.IsNullOrEmpty(p.FullName))
        {
            var dir = Path.GetDirectoryName(p.FullName);
            if (!string.IsNullOrEmpty(dir)) return dir;
        }

        // VCProject 側からも試す
        var vcproj = p.Object as Microsoft.VisualStudio.VCProjectEngine.VCProject;
        if (vcproj != null)
        {
            var dir2 = vcproj.ProjectDirectory;
            if (!string.IsNullOrEmpty(dir2))
                return Path.GetFullPath(dir2);
        }

        // 最後の砦（まず来ないはず）
        return AppDomain.CurrentDomain.BaseDirectory;
    }

    static int AddFilesToVcProjectAtDir(
    DTE2 dte, string projectName, string filterName, IEnumerable<string> relativeFilePaths, out string error)
    {
        error = null;
        var p = FindProjectByName(dte, projectName);
        if (p == null) { error = $"Project '{projectName}' not found."; return 0; }

        var vcproj = p.Object as Microsoft.VisualStudio.VCProjectEngine.VCProject;
        if (vcproj == null) { error = $"Project '{projectName}' is not a VC++ project."; return 0; }

        var projDir = GetProjectDirectory(p);

        bool old = dte.SuppressUI;
        dte.SuppressUI = true;

        int added = 0;
        try
        {
            var filter = !string.IsNullOrEmpty(filterName) ? EnsureFilter(vcproj, filterName) : null;

            foreach (var rel in relativeFilePaths)
            {
                if (string.IsNullOrWhiteSpace(rel)) continue;
                var relNorm = rel.Trim().Trim('"').Replace('/', '\\');
                var abs = Path.GetFullPath(Path.Combine(projDir, relNorm));

                try
                {
                    // テンプレ適用（.h/.cpp のみ。必要なら他拡張子も分岐追加）
                    string stem = Path.GetFileNameWithoutExtension(abs);
                    string ext = Path.GetExtension(abs).ToLowerInvariant();
                    switch (ext)
                    {
                        case ".h":
                        case ".hpp":
                        case ".hh":
                            WriteFromTemplateIfEmpty(abs, "TemplateScript.h", stem);
                            break;
                        case ".cpp":
                        case ".cxx":
                            WriteFromTemplateIfEmpty(abs, "TemplateScript.cpp", stem);
                            break;
                        default:
                            var parent = Path.GetDirectoryName(abs);
                            if (string.IsNullOrEmpty(parent))
                                throw new ArgumentException("Invalid path: " + abs);
                            Directory.CreateDirectory(parent);
                            if (!File.Exists(abs))
                                File.WriteAllText(abs, "", new UTF8Encoding(true));
                            break;
                    }

                    if (filter != null) filter.AddFile(abs);
                    else vcproj.AddFile(abs);

                    added++;
                }
                catch (Exception ex)
                {
                    error = (error == null) ? ex.Message : (error + "; " + ex.Message);
                }
            }

            dte.ExecuteCommand("File.SaveAll");
        }
        finally
        {
            dte.SuppressUI = old;
        }
        return added;
    }

    static void Main()
    {
        int parentPid = GetParentProcessId();
        System.Diagnostics.Process parent = null;
        try {
            parent = System.Diagnostics.Process.GetProcessById(parentPid);
            if (parent.ProcessName.Equals("msvsmon", StringComparison.OrdinalIgnoreCase))
            {
                // 実際には ChoEditor.exe を探して監視する
                parent = FindEditorProcess();
            }
        }
        catch {
            Environment.Exit(0); 
        }
        // Engine が終了したら自分も終了する監視スレッド
        new System.Threading.Thread(() =>
        {
            while (true)
            {
                if (parent.HasExited)
                    Environment.Exit(0);
                System.Threading.Thread.Sleep(1000);
            }
        })
        { IsBackground = true }.Start();

        // パイプ接続
        // エンジンがイベントを作るまで待つ
        EventWaitHandle waitHandle = null;
        while (true)
        {
            try
            {
                waitHandle = EventWaitHandle.OpenExisting("TheatriaEngineReadyEvent");
                break;
            }
            catch
            {
                System.Threading.Thread.Sleep(100); // エンジンがまだイベントを作っていない
            }
        }

        // 合図が来るまで待機
        waitHandle.WaitOne();
        Console.WriteLine("合図を受け取りました。パイプ接続します。");

        // パイプ接続
        using (var pipeIn = new NamedPipeClientStream(".", "EngineToWatcher", PipeDirection.In))
        using (var pipeOut = new NamedPipeClientStream(".", "WatcherToEngine", PipeDirection.Out))
        {
            try
            {
                // 両方に接続
                pipeIn.Connect(5000);
                pipeOut.Connect(5000);
                // 5秒タイムアウト
                Console.WriteLine("パイプ接続成功！");
            }
            catch (TimeoutException)
            {
                Console.WriteLine("パイプ接続タイムアウト");
                return;
            }

            Console.WriteLine("Connected to Engine.");

            using (var reader = new StreamReader(pipeIn, Encoding.Unicode))
            using (var writer = new StreamWriter(pipeOut, Encoding.Unicode) { AutoFlush = true })
            {
                while (true)
                {
                    string line = reader.ReadLine();
                    if (line == null)
                    {
                        Console.WriteLine("Engine が終了しました。");
                        break;
                    }

                    Console.WriteLine("[Engineから受信] " + line);

                    switch (line)
                    {
                        // 必ず ACK: で始まる応答を返すこと
                        case "TEST_MESSAGE":
                            writer.WriteLine("ACK:TEST_MESSAGE");
                            break;
                        case "BUILD_START":
                            writer.WriteLine("ACK:BUILD_START");
                            break;
                        case "BUILD_DONE":
                            writer.WriteLine("ACK:BUILD_DONE");
                            break;
                        case string s when s.StartsWith("BUILD_SLN|"):// "BUILD_SLN|Game|Debug|x64"
                            {
                                var parts = s.Split('|');
                                if (parts.Length >= 4)
                                {
                                    string slnName = parts[1];
                                    string config = parts[2];
                                    string platform = parts[3];

                                    // writer.WriteLine("ACK:BUILD_SLN");

                                    bool found = false;
                                    foreach (var dte in GetRunningVisualStudios())
                                    {
                                        try
                                        {
                                            string opened = System.IO.Path.GetFileNameWithoutExtension(dte.Solution.FullName);
                                            if (string.Equals(opened, slnName, StringComparison.OrdinalIgnoreCase))
                                            {
                                                Console.WriteLine($"ビルド対象ソリューション発見: {dte.Solution.FullName}");
                                                found = true;
                                                var configs = dte.Solution.SolutionBuild.SolutionConfigurations;
                                                EnvDTE.SolutionConfiguration targetConfig = null;

                                                foreach (EnvDTE.SolutionConfiguration cfg in configs)
                                                {
                                                    if (string.Equals(cfg.Name, config, StringComparison.OrdinalIgnoreCase))
                                                    {
                                                        // この構成が対象のプラットフォームを持つかチェック
                                                        foreach (EnvDTE.SolutionContext ctx in cfg.SolutionContexts)
                                                        {
                                                            if (string.Equals(ctx.PlatformName, platform, StringComparison.OrdinalIgnoreCase))
                                                            {
                                                                targetConfig = cfg;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    if (targetConfig != null)
                                                    {
                                                        break;
                                                    }
                                                }
                                                if (targetConfig != null)
                                                {
                                                    targetConfig.Activate();
                                                    dte.Solution.SolutionBuild.Build(true);

                                                    // ビルド完了待ち
                                                    while (dte.Solution.SolutionBuild.BuildState == vsBuildState.vsBuildStateInProgress)
                                                        System.Threading.Thread.Sleep(500);

                                                    Console.WriteLine("Build 完了");
                                                    writer.WriteLine("ACK:BUILD_SLN");
                                                }
                                                else
                                                {
                                                    Console.WriteLine($"構成 {config}|{platform} が見つかりませんでした。");
                                                }
                                                
                                                break;
                                            }
                                        }
                                        catch (Exception ex)
                                        {
                                            Console.WriteLine("DTE 操作中エラー: " + ex.Message);
                                        }
                                    }

                                    if (!found)
                                    {
                                        Console.WriteLine($"指定されたソリューション {slnName} が見つかりません。");
                                    }
                                }
                                break;
                            }
                        case string s when s.StartsWith("BUILD_SLN_DEBUGGER|"):// "BUILD_SLN_DEBUGGER|Game|Debug|x64"
                            {
                                var parts = s.Split('|');
                                if (parts.Length >= 4)
                                {
                                    string slnName = parts[1];
                                    string config = parts[2];
                                    string platform = parts[3];

                                    // writer.WriteLine("ACK:BUILD_SLN");

                                    bool found = false;
                                    foreach (var dte in GetRunningVisualStudios())
                                    {
                                        try
                                        {
                                            string opened = System.IO.Path.GetFileNameWithoutExtension(dte.Solution.FullName);
                                            if (string.Equals(opened, slnName, StringComparison.OrdinalIgnoreCase))
                                            {
                                                Console.WriteLine($"ビルド対象ソリューション発見: {dte.Solution.FullName}");
                                                found = true;
                                                var configs = dte.Solution.SolutionBuild.SolutionConfigurations;
                                                EnvDTE.SolutionConfiguration targetConfig = null;

                                                foreach (EnvDTE.SolutionConfiguration cfg in configs)
                                                {
                                                    if (string.Equals(cfg.Name, config, StringComparison.OrdinalIgnoreCase))
                                                    {
                                                        // この構成が対象のプラットフォームを持つかチェック
                                                        foreach (EnvDTE.SolutionContext ctx in cfg.SolutionContexts)
                                                        {
                                                            if (string.Equals(ctx.PlatformName, platform, StringComparison.OrdinalIgnoreCase))
                                                            {
                                                                targetConfig = cfg;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    if (targetConfig != null)
                                                    {
                                                        break;
                                                    }
                                                }
                                                if (targetConfig != null)
                                                {
                                                    targetConfig.Activate();
                                                    dte.Solution.SolutionBuild.Build(true);

                                                    // ビルド完了待ち
                                                    while (dte.Solution.SolutionBuild.BuildState == vsBuildState.vsBuildStateInProgress)
                                                        System.Threading.Thread.Sleep(500);

                                                    Console.WriteLine("Build 完了");

                                                    // 親プロセスIDを取得（すでに持っているなら使う）
                                                    int id = parent.Id;
                                                    Console.WriteLine($"親プロセスID: {id}");

                                                    try
                                                    {
                                                        // VS のデバッガが見ているローカルプロセスを列挙
                                                        foreach (EnvDTE.Process proc in dte.Debugger.LocalProcesses)
                                                        {
                                                            if (proc.ProcessID == id)
                                                            {
                                                                proc.Attach();
                                                                Console.WriteLine("親プロセスにデバッガをアタッチしました。");
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    catch (Exception ex)
                                                    {
                                                        Console.WriteLine("デバッガアタッチ失敗: " + ex.Message);
                                                    }

                                                    writer.WriteLine("ACK:BUILD_SLN");
                                                }
                                                else
                                                {
                                                    Console.WriteLine($"構成 {config}|{platform} が見つかりませんでした。");
                                                }

                                                break;
                                            }
                                        }
                                        catch (Exception ex)
                                        {
                                            Console.WriteLine("DTE 操作中エラー: " + ex.Message);
                                        }
                                    }

                                    if (!found)
                                    {
                                        Console.WriteLine($"指定されたソリューション {slnName} が見つかりません。");
                                    }
                                }
                                break;
                            }
                        case string s when s.StartsWith("STOP_DEBUGGER|"):
                            {
                                var parts = s.Split('|');
                                if (parts.Length >= 2)
                                {
                                    string slnName = parts[1];

                                    foreach (var dte in GetRunningVisualStudios())
                                    {
                                        try
                                        {
                                            string opened = Path.GetFileNameWithoutExtension(dte.Solution.FullName);
                                            if (string.Equals(opened, slnName, StringComparison.OrdinalIgnoreCase))
                                            {
                                                if (dte.Debugger.DebuggedProcesses.Count > 0)
                                                {
                                                    try
                                                    {
                                                        dte.Debugger.Stop(true); // ← Shift+F5 相当
                                                        Console.WriteLine("デバッグを停止しました。");
                                                        writer.WriteLine("ACK:STOP_DEBUGGER");
                                                    }
                                                    catch (Exception ex)
                                                    {
                                                        Console.WriteLine("デバッグ停止失敗: " + ex.Message);
                                                    }
                                                }
                                                else
                                                {
                                                    Console.WriteLine("現在デバッグ中のプロセスはありません。");
                                                    writer.WriteLine("ACK:STOP_DEBUGGER");
                                                }
                                            }
                                        }
                                        catch (Exception ex)
                                        {
                                            Console.WriteLine("デバッガ操作中エラー: " + ex.Message);
                                        }
                                    }
                                }
                                break;
                            }
                        case string s when s.StartsWith("ADD_FILES_PROJ|"):
                            {
                                var parts = s.Split('|');
                                if (parts.Length >= 5)
                                {
                                    string slnName = parts[1];
                                    string projName = parts[2];
                                    string filter = parts[3];
                                    var files = parts[4].Split(new[] { ';' }, StringSplitOptions.RemoveEmptyEntries);

                                    bool slnFound = false;
                                    foreach (var dte in GetRunningVisualStudios())
                                    {
                                        try
                                        {
                                            string opened = Path.GetFileNameWithoutExtension(dte.Solution.FullName);
                                            if (!string.Equals(opened, slnName, StringComparison.OrdinalIgnoreCase))
                                                continue;

                                            slnFound = true;

                                            string error;
                                            int added = AddFilesToVcProjectAtDir(dte, projName, filter, files, out error);
                                            if (string.IsNullOrEmpty(error))
                                                writer.WriteLine($"ACK:ADD_FILES|OK|{added}");
                                            else
                                                writer.WriteLine($"ACK:ADD_FILES|ERROR|{error}");
                                            break;
                                        }
                                        catch (Exception ex)
                                        {
                                            writer.WriteLine($"ACK:ADD_FILES|ERROR|{ex.Message}");
                                            break;
                                        }
                                    }
                                    if (!slnFound)
                                        writer.WriteLine($"ACK:ADD_FILES|ERROR|Solution '{slnName}' not found.");
                                }
                                else writer.WriteLine("ACK:ADD_FILES|ERROR|Bad arguments.");
                                break;
                            }
                        case string s when s.StartsWith("ADD_SCRIPT_PROJ|"):
                            {
                                var parts = s.Split('|');
                                if (parts.Length >= 5)
                                {
                                    string slnName = parts[1];
                                    string projName = parts[2];
                                    string filter = parts[3];    // 例: "Source Files"
                                    string cls = parts[4];    // 例: "Player"

                                    bool slnFound = false;
                                    foreach (var dte in GetRunningVisualStudios())
                                    {
                                        try
                                        {
                                            string opened = Path.GetFileNameWithoutExtension(dte.Solution.FullName);
                                            if (!string.Equals(opened, slnName, StringComparison.OrdinalIgnoreCase))
                                                continue;

                                            slnFound = true;

                                            // ヘッダは Header Files、ソースは指定フィルタへ
                                            string err1, err2;
                                            int n1 = AddFilesToVcProjectAtDir(dte, projName, "Header Files", new[] { cls + ".h" }, out err1);
                                            int n2 = AddFilesToVcProjectAtDir(dte, projName, filter, new[] { cls + ".cpp" }, out err2);

                                            if (string.IsNullOrEmpty(err1) && string.IsNullOrEmpty(err2))
                                                writer.WriteLine($"ACK:ADD_SCRIPT|OK|{n1 + n2}");
                                            else
                                                writer.WriteLine($"ACK:ADD_SCRIPT|ERROR|{(err1 ?? "")} {(err2 ?? "")}");
                                            break;
                                        }
                                        catch (Exception ex)
                                        {
                                            writer.WriteLine($"ACK:ADD_SCRIPT|ERROR|{ex.Message}");
                                            break;
                                        }
                                    }
                                    if (!slnFound)
                                        writer.WriteLine($"ACK:ADD_SCRIPT|ERROR|Solution '{slnName}' not found.");
                                }
                                else writer.WriteLine("ACK:ADD_SCRIPT|ERROR|Bad arguments.");
                                break;
                            }
                        default:
                            writer.WriteLine("ACK:UNKNOWN");
                            break;
                    }
                }
            }
        }

        
    }

    // ROT から DTE を列挙
    static IEnumerable<EnvDTE80.DTE2> GetRunningVisualStudios()
    {
        IRunningObjectTable rot;
        GetRunningObjectTable(0, out rot);
        IEnumMoniker enumMoniker;
        rot.EnumRunning(out enumMoniker);

        IMoniker[] moniker = new IMoniker[1];
        IntPtr fetched = IntPtr.Zero;

        while (enumMoniker.Next(1, moniker, fetched) == 0)
        {
            IBindCtx bindCtx;
            CreateBindCtx(0, out bindCtx);
            string displayName;
            moniker[0].GetDisplayName(bindCtx, null, out displayName);

            if (displayName.StartsWith("!VisualStudio.DTE"))
            {
                object comObject;
                rot.GetObject(moniker[0], out comObject);
                yield return (EnvDTE80.DTE2)comObject;
            }
        }
    }

    [DllImport("ole32.dll")]
    private static extern int CreateBindCtx(uint reserved, out IBindCtx ppbc);

    [DllImport("ole32.dll")]
    private static extern int GetRunningObjectTable(uint reserved, out IRunningObjectTable prot);

    [DllImport("kernel32.dll")]
    static extern IntPtr GetCurrentProcess();

    [DllImport("ntdll.dll")]
    static extern int NtQueryInformationProcess(
        IntPtr processHandle, int processInformationClass,
        ref PROCESS_BASIC_INFORMATION pbi, int processInformationLength, out int returnLength);

    [StructLayout(LayoutKind.Sequential)]
    struct PROCESS_BASIC_INFORMATION
    {
        public IntPtr Reserved1;
        public IntPtr PebBaseAddress;
        public IntPtr Reserved2_0;
        public IntPtr Reserved2_1;
        public IntPtr UniqueProcessId;
        public IntPtr InheritedFromUniqueProcessId; // 親プロセスID
    }

    static int GetParentProcessId()
    {
        PROCESS_BASIC_INFORMATION pbi = new PROCESS_BASIC_INFORMATION();
        NtQueryInformationProcess(GetCurrentProcess(), 0, ref pbi, Marshal.SizeOf(pbi), out _);
        return pbi.InheritedFromUniqueProcessId.ToInt32();
    }
}
