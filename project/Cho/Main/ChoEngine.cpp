#include "pch.h"
#include "ChoEngine.h"

// Windowアプリケーション
#include "Cho/OS/Windows/WinApp/WinApp.h"

void ChoEngine::Initialize()
{
	// ResourceLeakChecker
	resourceLeakChecker = std::make_unique<ResourceLeakChecker>();

	// ウィンドウの作成
	WinApp::CreateGameWindow();

	// DirectX12初期化
	dx12 = std::make_unique<DirectX12Common>();
	dx12->Initialize();
}

void ChoEngine::Finalize()
{
	// DirectX12終了処理
	dx12->Finalize();
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
