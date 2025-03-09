#include "pch.h"
#include "ChoEngine.h"

// Windowアプリケーション
#include "Cho/OS/Windows/WinApp/WinApp.h"

ChoEngine::ChoEngine()
{
}

ChoEngine::~ChoEngine()
{
}

void ChoEngine::Initialize()
{
	// ResourceLeakChecker
	resourceLeakChecker = std::make_unique<ResourceLeakChecker>();

	// ウィンドウの作成
	WinApp::CreateGameWindow();

	// DirectX12初期化
	dx12 = std::make_unique<DirectX12Common>();

	// PlatformLayer初期化
	platformLayer = std::make_unique<PlatformLayer>();

	// CoreSystem初期化
	coreSystem = std::make_unique<CoreSystem>();

	// ResourceManager初期化
	resourceManager = std::make_unique<ResourceManager>(dx12->GetDevice());

	// GraphicsEngine初期化
	graphicsEngine = std::make_unique<GraphicsEngine>(dx12->GetDevice());
	// SwapChainの生成
	graphicsEngine->CreateSwapChain(dx12->GetDXGIFactory(), resourceManager.get());
}

void ChoEngine::Finalize()
{
	// PlatformLayer終了処理
	platformLayer->Finalize();
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
		// 開始
		Start();
		// 更新
		Update();
		// 終了
		End();
	}

	/*終了処理*/
	Finalize();
}

void ChoEngine::Update()
{
	// PlatformLayer更新
	platformLayer->Update();
}

void ChoEngine::Start()
{
	// PlatformLayer記録開始
	platformLayer->StartFrame();
}

void ChoEngine::End()
{
	// PlatformLayer記録終了
	platformLayer->EndFrame();
}
