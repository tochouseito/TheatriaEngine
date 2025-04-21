#pragma warning(push)
#pragma warning(disable:28251)
// エンジン起動関数を使う
#define ENGINECREATE_FUNCTION
// Engine
#include "Cho/ChoEngineAPI.h"
// Windows
#include <Windows.h>
#include <memory>
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#pragma warning(pop)
	// エンジンのインスタンス
	std::unique_ptr<Engine, decltype(&Cho::DestroyEngine)> engine(
		Cho::CreateEngine(RuntimeMode::Editor), Cho::DestroyEngine);// エンジンの生成
	Cho::SetEngine(engine.get());// エンジンのポインタをセット
	// エンジンの稼働
	engine->CrashHandlerEntry();// クラッシュ時の処理のハンドラー
	engine->Operation();// エンジンの稼働
	return 0;
}