#include "pch.h"
#include "ChoEngine.h"

// Windowアプリケーション
#include "Cho/OS/Windows/WinApp/WinApp.h"

void ChoEngine::Initialize()
{
	// ウィンドウの作成
	WinApp::CreateGameWindow();
}

void ChoEngine::Finalize()
{
	// ウィンドウの破棄
	WinApp::TerminateWindow();
}

void ChoEngine::Operation()
{
	/*初期化*/
	Initialize();

	/*メインループ*/
	while (true) {
		if (WinApp::ProcessMessage()) {
			break;
		}
	}

	/*終了処理*/
	Finalize();
}
