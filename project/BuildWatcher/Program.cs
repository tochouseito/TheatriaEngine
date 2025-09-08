using EnvDTE;
using EnvDTE80;
using System;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Runtime.InteropServices;
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
                waitHandle = EventWaitHandle.OpenExisting("ChoEngineReadyEvent");
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
                        default:
                            writer.WriteLine("ACK:UNKNOWN");
                            break;
                    }
                }
            }
        }

        
    }

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
