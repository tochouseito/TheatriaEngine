using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using System;
using System.ComponentModel.Design;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;
using EnvDTE;
using EnvDTE80;
using Task = System.Threading.Tasks.Task;

namespace ChoEngineVSIX
{
    [PackageRegistration(UseManagedResourcesOnly = true, AllowsBackgroundLoading = true)]
    [InstalledProductRegistration("Attach to ChoEngine", "One-click attach to your engine", "1.0")]
    // ▼ これがないと VSCT が読まれずボタンが出ません
    [ProvideMenuResource("Menus.ctmenu", 1)]
    [Guid(ChoEngineVSIXPackage.PackageGuidString)]
    public sealed class ChoEngineVSIXPackage : AsyncPackage
    {
        public const string PackageGuidString = "44cacfaf-4dae-4c20-97d3-f3b7e1a41565";

        // VSCT と一致させるコマンド情報
        public static readonly Guid CommandSet = new Guid("b8d3e7c5-8d2d-4f5a-9d3d-9a9f1e7b2a11");
        public const int CommandId = 0x0100;

        protected override async Task InitializeAsync(CancellationToken cancellationToken, IProgress<ServiceProgressData> progress)
        {
            // UI スレッドへ
            await JoinableTaskFactory.SwitchToMainThreadAsync(cancellationToken);

            // コマンド登録
            var mcs = await GetServiceAsync(typeof(IMenuCommandService)) as OleMenuCommandService;
            if (mcs != null)
            {
                var cmdId = new CommandID(CommandSet, CommandId);
                var menuItem = new OleMenuCommand(async (s, e) => await AttachAsync(), cmdId);
                mcs.AddCommand(menuItem);
            }
        }

        private async Task AttachAsync()
        {
            await JoinableTaskFactory.SwitchToMainThreadAsync();

            // DTE 取得
            var dte = (DTE2)await GetServiceAsync(typeof(DTE));
            if (dte == null)
            {
                ShowInfo("DTE サービスが取得できませんでした。");
                return;
            }

            var dbg2 = (Debugger2)dte.Debugger;

            // 自作エンジンの候補プロセス名（必要に応じて編集）
            string[] targets = { "ChoEditor.exe", "GameTemplate.exe", "Engine.exe" };

            // ネイティブデバッガエンジン
            Engine nativeEngine = null;
            try
            {
                nativeEngine = dbg2.Transports.Item("Default")
                                   .Engines
                                   .OfType<Engine>()
                                   .FirstOrDefault(e => e.Name.IndexOf("Native", StringComparison.OrdinalIgnoreCase) >= 0);
            }
            catch { /* 一部環境で Transports 未初期化のことがある */ }

            if (nativeEngine == null)
            {
                ShowError("Native デバッグエンジンが見つかりません。オプションでネイティブデバッグを有効にしてください。");
                return;
            }

            // 起動中プロセスから対象を検索
            var processes = dbg2.LocalProcesses.OfType<Process2>().ToList();
            var candidates = processes.Where(p =>
            {
                try { return targets.Any(t => string.Equals(System.IO.Path.GetFileName(p.Name), t, StringComparison.OrdinalIgnoreCase)); }
                catch { return false; }
            }).ToList();

            if (candidates.Count == 0)
            {
                ShowInfo("対象プロセスが見つかりません。\n(例) ChoEditor.exe を起動してから実行してください。");
                return;
            }

            int ok = 0, ng = 0;
            foreach (var proc in candidates)
            {
                try
                {
                    // ネイティブでアタッチ（必要なら .NET など複数エンジン配列に）
                    proc.Attach2(new object[] { nativeEngine });
                    ok++;
                }
                catch (Exception ex)
                {
                    ng++;
                    ShowWarning($"アタッチ失敗: {proc.Name}\n{ex.Message}");
                }
            }

            if (ok > 0 && ng == 0)
                ShowStatus($"Attach to ChoEngine: {ok} プロセスに接続しました。");
            else
                ShowStatus($"Attach to ChoEngine: 成功 {ok}, 失敗 {ng}");
        }

        private void ShowInfo(string msg) =>
            VsShellUtilities.ShowMessageBox(this, msg, "Attach to ChoEngine",
                OLEMSGICON.OLEMSGICON_INFO, OLEMSGBUTTON.OLEMSGBUTTON_OK, OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);

        private void ShowWarning(string msg) =>
            VsShellUtilities.ShowMessageBox(this, msg, "Attach to ChoEngine",
                OLEMSGICON.OLEMSGICON_WARNING, OLEMSGBUTTON.OLEMSGBUTTON_OK, OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);

        private void ShowError(string msg) =>
            VsShellUtilities.ShowMessageBox(this, msg, "Attach to ChoEngine",
                OLEMSGICON.OLEMSGICON_CRITICAL, OLEMSGBUTTON.OLEMSGBUTTON_OK, OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);

        private void ShowStatus(string msg)
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            var statusBar = GetService(typeof(SVsStatusbar)) as IVsStatusbar;
            statusBar?.SetText(msg);
        }
    }
}
