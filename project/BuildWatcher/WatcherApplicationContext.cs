using System;
using System.IO;
using System.Windows.Forms;
using EnvDTE;
using EnvDTE80;
using System.Runtime.InteropServices;

namespace BuildWatcher
{
    public class WatcherApplicationContext : ApplicationContext
    {
        private DTE2 dte;

        public WatcherApplicationContext()
        {
            // Visual Studio の DTE を取得
            try
            {
                dte = (DTE2)Marshal.
                    GetActiveObject("VisualStudio.DTE.17.0"); // VS2022
            }
            catch
            {
                MessageBox.Show("Visual Studio が見つかりません。");
                ExitThread();
                return;
            }

            // ビルドイベント購読
            dte.Events.BuildEvents.OnBuildProjConfigDone += OnBuildDone;
        }

        private void OnBuildDone(string project, string projConfig, string platform, string solutionConfig, bool success)
        {
            if (success)
            {
                // DLL更新完了をログに出す（必要ならEngineへ通知も可能）
                File.AppendAllText("watcher.log", $"Build succeeded: {project}\n");
            }
            else
            {
                File.AppendAllText("watcher.log", $"Build failed: {project}\n");
            }
        }

        protected override void ExitThreadCore()
        {
            // 終了処理
            base.ExitThreadCore();
        }
    }
}
