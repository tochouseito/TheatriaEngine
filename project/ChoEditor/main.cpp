#pragma warning(push)
#pragma warning(disable:28251)
// エンジン起動関数を使う
#define ENGINECREATE_FUNCTION
// Engine
#include "Cho/ChoEngineAPI.h"
// Windows
#include <Windows.h>
#include <memory>
#include <string>

class BuildWatcherController
{
public:
    BuildWatcherController() { ZeroMemory(&pi, sizeof(pi)); }
    ~BuildWatcherController() { TerminateBuildWatcher(); }

    // BuildWatcher起動
    void LaunchBuildWatcher()
    {
        STARTUPINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);

        if (!CreateProcessW(
            L"BuildWatcher.exe",  // 実行ファイル
            nullptr,              // コマンドライン引数
            nullptr, nullptr,     // セキュリティ属性
            FALSE,                // ハンドル継承
            CREATE_NO_WINDOW,     // 完全非表示
            nullptr,              // 環境変数
            nullptr,              // カレントディレクトリ
            &si, &pi))
        {
            DWORD err = GetLastError();
            std::wstring msg = L"BuildWatcher.exe 起動失敗\nエラーコード: " + std::to_wstring(err);
            MessageBoxW(nullptr, msg.c_str(), L"Error", MB_ICONERROR);
        }
    }

    // BuildWatcher終了
    void TerminateBuildWatcher()
    {
        if (pi.hProcess)
        {
            TerminateProcess(pi.hProcess, 0); // ★強制終了（将来的にIPCで置換すると良い）
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            pi.hProcess = nullptr;
            pi.hThread = nullptr;
        }
    }

private:
    PROCESS_INFORMATION pi;
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#pragma warning(pop)
	// エンジンのインスタンス
	std::unique_ptr<Engine, decltype(&cho::DestroyEngine)> engine(
		cho::CreateEngine(RuntimeMode::Editor), cho::DestroyEngine);// エンジンの生成
	cho::SetEngine(engine.get());// エンジンのポインタをセット

	// BuildWatcherコントローラー
	//BuildWatcherController buildWatcher;

	// エンジンの稼働
	engine->CrashHandlerEntry();// クラッシュ時の処理のハンドラー
	//buildWatcher.LaunchBuildWatcher();// BuildWatcher起動
	engine->Operation();// エンジンの稼働
	return 0;
}