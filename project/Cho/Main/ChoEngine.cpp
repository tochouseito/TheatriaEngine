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
	graphicsEngine = std::make_unique<GraphicsEngine>(dx12->GetDevice(),resourceManager.get());
	graphicsEngine->CreateSwapChain(dx12->GetDXGIFactory());
	graphicsEngine->Init();

	// ImGuiManager初期化
	imGuiManager = std::make_unique<ImGuiManager>();
	imGuiManager->Initialize(dx12->GetDevice(), resourceManager.get());

	// GameCore初期化
	gameCore = std::make_unique<GameCore>();
	gameCore->Initialize(resourceManager.get());

	resourceManager->GenerateManager(graphicsEngine.get(), gameCore->GetSceneManager()->GetIntegrationBuffer());

	// EditorCommand初期化
	editorCommand = std::make_unique<EditorCommand>(resourceManager.get(), graphicsEngine.get(), gameCore.get());
	// EditorManager初期化
	editorManager = std::make_unique<EditorManager>(editorCommand.get());
	editorManager->Initialize();
}

void ChoEngine::Finalize()
{
	// GPUの完了待ち
	graphicsEngine->Finalize();
	// GameCore終了処理
	
	// ImGuiManager終了処理
	imGuiManager->Finalize();
	// PlatformLayer終了処理
	platformLayer->Finalize();
	// DirectX12終了処理
	dx12->Finalize();
	// スレッドクローズ
	coreSystem->Finalize();
	// ウィンドウの破棄
	WinApp::TerminateWindow();

	// 各種手動解放
	editorManager.reset();
	editorCommand.reset();
	gameCore.reset();
	imGuiManager.reset();
	graphicsEngine.reset();
	resourceManager.reset();
	coreSystem.reset();
	platformLayer.reset();
	dx12.reset();
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
		// 描画
		Draw();
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
	// ImGuiManager開始
	imGuiManager->Begin();
	// EditorManager更新
	editorManager->Update();
	// GameCore更新
	gameCore->Update(*resourceManager, *graphicsEngine);
	// ImGuiManager終了
	imGuiManager->End();
}

void ChoEngine::Draw()
{
	//　描画準備
	graphicsEngine->PreRender();
	// 描画
	//graphicsEngine->Render(*resourceManager, *gameCore);
	// 描画後片付け
	graphicsEngine->PostRender();
	//graphicsEngine->PostRenderWithImGui(imGuiManager.get());
}

void ChoEngine::Start()
{
	// PlatformLayer記録開始
	platformLayer->StartFrame();
	// GameCore開始
	gameCore->Start(*resourceManager);
}

void ChoEngine::End()
{
	// PlatformLayer記録終了
	platformLayer->EndFrame();
}
