#include "Cho.h"
#include"LogGenerator/LogGenerator.h"

// System
#include"WinApp/WinApp.h"
#include"D3D12/ResourceLeakChecker/ResourceLeakChecker.h"
#include"FrameRate/FrameRate.h"
#include"D3D12/DXGIFactory/DXGIFactory.h"
#include"D3D12/D3DDevice/D3DDevice.h"
#include"D3D12/D3DCommand/D3DCommand.h"
#include"D3D12/D3DSwapChain/D3DSwapChain.h"
#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"D3D12/RTVManager/RTVManager.h"
#include"D3D12/DSVManager/DSVManager.h"
#include"D3D12/DrawExecution/DrawExecution.h"

// ThreadManager
#include"ThreadManager/ThreadManager.h"

// GraphicsSystem
#include"Graphics/GraphicsSystem/GraphicsSystem.h"

// Loader
#include"Load/TextureLoader/TextureLoader.h"
#include"Load/MeshLoader/MeshLoader.h"
#include"Load/ModelLoader/ModelLoader.h"

// Script
#include "Script/ScriptManager/ScriptManager.h"
#include"Generator/ScriptProject/ScriptProject.h"

// ECS
#include"ECS/EntityManager/EntityManager.h"
#include"ECS/ComponentManager/ComponentManager.h"
#include"ECS/System/SystemManager/SystemManager.h"
#include"ECS/PrefabManager/PrefabManager.h"

// Scene
#include"Scene/SceneManager/SceneManager.h"

// 汎用機能
#include"Input/InputManager.h"
#include"Audio/AudioManager.h"
#include"UI/ImGuiManager/ImGuiManager.h"

// Editor
#include"Editor/EditorManager/EditorManager.h"

// StartSetting
#include"StartSetting/StartSetting.h"

// Json
#include"Load/JsonFileLoader/JsonFileLoader.h"

// Save
#include"SaveManager/SaveManager.h"

#pragma region 静的メンバー変数の定義
SystemState& Cho::systemState = SystemState::GetInstance();

std::unique_ptr <WinApp> Cho::win = nullptr;
std::unique_ptr <ResourceLeakChecker> Cho::resourceLeakChecker = nullptr;
std::unique_ptr <FrameRate> Cho::frameRate = nullptr;
std::unique_ptr <DXGIFactory> Cho::dxgiFactory = nullptr;
std::unique_ptr <D3DDevice> Cho::d3dDevice = nullptr;
std::unique_ptr<D3DCommand> Cho::d3dCommand = nullptr;
std::unique_ptr<D3DSwapChain> Cho::d3dSwapChain = nullptr;
std::unique_ptr<ResourceViewManager>Cho::resourceViewManager = nullptr;
std::unique_ptr<RTVManager>Cho::rtvManager = nullptr;
std::unique_ptr<DSVManager> Cho::dsvManager = nullptr;
std::unique_ptr<DrawExecution> Cho::drawExecution = nullptr;

// ThreadManager
std::unique_ptr<ThreadManager> Cho::threadManager = nullptr;

// GraphicsSystem
std::unique_ptr<GraphicsSystem> Cho::graphicsSystem = nullptr;

// Loader
std::unique_ptr<TextureLoader> Cho::textureLoader = nullptr;
std::unique_ptr<MeshLoader> Cho::meshLoader = nullptr;
std::unique_ptr<ModelLoader> Cho::modelLoader = nullptr;

// Script
std::unique_ptr<ScriptManager> Cho::scriptManager = nullptr;

// ECS
std::unique_ptr<EntityManager>Cho::entityManager = nullptr;
std::unique_ptr<ComponentManager> Cho::componentManager = nullptr;
std::unique_ptr<SystemManager> Cho::systemManager = nullptr;
std::unique_ptr<PrefabManager> Cho::prefabManager = nullptr;

// Scene
std::unique_ptr<SceneManager> Cho::sceneManager = nullptr;

// 汎用機能
std::unique_ptr<InputManager> Cho::inputManager = nullptr;
std::unique_ptr<AudioManager> Cho::audioManager = nullptr;
std::unique_ptr<ImGuiManager> Cho::imguiManager = nullptr;

// Editor
std::unique_ptr<EditorManager> Cho::editorManager = nullptr;

/*Starter*/
std::unique_ptr<StartSetting> Cho::startSetting = nullptr;

/*Json*/
std::unique_ptr<JsonFileLoader> Cho::jsonFileLoader = nullptr;

/*Save*/
std::unique_ptr<SaveManager> Cho::saveManager = nullptr;

// ThreadTest用
bool Cho::isLoadTaskEnd = false;
std::mutex Cho::loadTaskMutex;

#pragma endregion

void Cho::Initialize()
{
	// リソースリークチェッカー
	resourceLeakChecker = std::make_unique<ResourceLeakChecker>();

	// 共有クラスの初期化
	systemState.Initialize();

	// ウィンドウの作成
	win = std::make_unique<WinApp>();
	win->CreateGameWindow();

	WriteLog("ウィンドウ作成");

	// フレーム
	frameRate = std::make_unique<FrameRate>();
	frameRate->Initialize();

#pragma region DirectX

	// DXGIファクトリー
	dxgiFactory = std::make_unique<DXGIFactory>();
	dxgiFactory->Initialize(true);

	// Device
	d3dDevice = std::make_unique<D3DDevice>();
	d3dDevice->Initialize(*dxgiFactory->GetDXGIFactory());

	// Command
	d3dCommand = std::make_unique<D3DCommand>();
	d3dCommand->Initialize(*d3dDevice->GetDevice());

	// SwapChain
	d3dSwapChain = std::make_unique<D3DSwapChain>();
	d3dSwapChain->Initialize(
		win.get(),
		*dxgiFactory->GetDXGIFactory(),
		*d3dCommand->GetCommandQueue(DIRECT)
	);

	// ResourceViewManager
	resourceViewManager = std::make_unique<ResourceViewManager>();
	resourceViewManager->Initialize(d3dDevice.get(), d3dCommand.get());

	// RTVManager
	rtvManager = std::make_unique<RTVManager>();
	rtvManager->Initialize(
		d3dDevice.get(),
		d3dSwapChain.get()
	);

	// DSVManager
	dsvManager = std::make_unique<DSVManager>();
	dsvManager->Initialize(
		win.get(),
		d3dDevice.get(),
		resourceViewManager.get()
	);

	WriteLog("DirectX初期化");
#pragma endregion

	// ThreadManager
	threadManager = std::make_unique<ThreadManager>(4);// 4スレッド

#pragma region グラフィックスシステム

	// GraphicsSystem
	graphicsSystem = std::make_unique<GraphicsSystem>();
	graphicsSystem->Initialize(d3dDevice.get());

	// DrawExecution
	drawExecution = std::make_unique<DrawExecution>();
	drawExecution->Initialize(
		d3dCommand.get(),
		d3dSwapChain.get(),
		resourceViewManager.get(),
		rtvManager.get(),
		dsvManager.get(),
		graphicsSystem.get()
	);
	WriteLog("GraphicsSystem初期化");
#pragma endregion

#pragma region 汎用機能初期化

	// Input
	inputManager = std::make_unique<InputManager>();
	inputManager->Initialize(win.get());

	// Audio
	audioManager = std::make_unique<AudioManager>();
	audioManager->Initialize();

	// TextureLoader
	textureLoader = std::make_unique<TextureLoader>();
	textureLoader->Initialize(
		d3dDevice.get(),
		d3dCommand.get(),
		resourceViewManager.get()
	);

	WriteLog("汎用機能初期化");

	// MeshLoader
	meshLoader = std::make_unique<MeshLoader>();
	meshLoader->Initialize(resourceViewManager.get());

	// ModelLoader
	modelLoader = std::make_unique<ModelLoader>();
	modelLoader->Initialize(resourceViewManager.get(), textureLoader.get(), meshLoader.get());

	WriteLog("Loader初期化");

	// ScriptManager
	scriptManager = std::make_unique<ScriptManager>();
	scriptManager->Initialize();

	// ImGuiManager
	imguiManager = std::make_unique<ImGuiManager>();
	imguiManager->Initialize(
		win.get(),
		d3dDevice.get(),
		d3dCommand.get(),
		resourceViewManager.get()
	);

	WriteLog("ImGui初期化");

#pragma endregion

#pragma region ECS

	// EntityManager
	entityManager = std::make_unique<EntityManager>();

	// ComponentManager
	componentManager = std::make_unique<ComponentManager>();
	componentManager->SetRVManager(resourceViewManager.get());

	// SystemManager
	systemManager = std::make_unique<SystemManager>();
	systemManager->Initialize(
		d3dCommand.get(),
		drawExecution.get(),
		resourceViewManager.get(),
		graphicsSystem.get(),
		textureLoader.get(),
		entityManager.get(),
		componentManager.get()
	);

	// PrefabManager
	prefabManager = std::make_unique<PrefabManager>();

	WriteLog("ECS初期化");

#pragma endregion

	// SceneManager
	sceneManager = std::make_unique<SceneManager>();
	sceneManager->Initialize(
		resourceViewManager.get(),
		entityManager.get(),
		componentManager.get(),
		systemManager.get(),
		prefabManager.get(),
		inputManager.get(),
		audioManager.get()
	);

	WriteLog("SceneManager初期化");

#pragma region プロジェクトデータの読み込み

#pragma endregion

#pragma region エディター

	// EditorManager
	editorManager = std::make_unique<EditorManager>();
	editorManager->Initialize(
		win.get(),
		resourceViewManager.get(),
		rtvManager.get(),
		drawExecution.get(),
		textureLoader.get(),
		modelLoader.get(),
		entityManager.get(),
		componentManager.get(),
		systemManager.get(),
		prefabManager.get(),
		inputManager.get(),
		sceneManager.get(),
		scriptManager.get()
	);

	WriteLog("Editor初期化");

#pragma endregion

#pragma region JSON

	// Json
	jsonFileLoader = std::make_unique<JsonFileLoader>();
	jsonFileLoader->Initialize(
		scriptManager.get(),
		entityManager.get(),
		componentManager.get(),
		prefabManager.get(),
		sceneManager.get()
	);

	WriteLog("Json初期化");

#pragma endregion

	// スタート設定
	startSetting = std::make_unique<StartSetting>();

	// デフォルトメッシュ生成
	resourceViewManager->CreateMeshPattern();

	WriteLog("デフォルトメッシュ生成");

	// ライトリソース生成
	resourceViewManager->CreateLightResource();

	/*読み込み*/
	//Load();
	threadManager->EnqueueTask(Load);

	while (true) {
		{
			std::unique_lock<std::mutex> lock(loadTaskMutex);
			if (isLoadTaskEnd) {
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	WriteLog("読み込み");
}

void Cho::Finalize()
{
#ifdef _DEBUG
	// セーブ
	Save();
#endif // _DEBUG
	// ThreadManager解放
	threadManager->StopAllThreads();

	// シーン解放
	sceneManager->Finalize();
	// ImGui解放
	imguiManager->Finalize();
	// Audio解放
	audioManager->Finalize();
	/*フェンスの終了*/
	d3dCommand->Finalize();
	// ウィンドウの破棄
	win->TerminateWindow();
}

void Cho::PreUpdate()
{
	// ComputeCommand記録開始
	d3dCommand->Reset(CommandType::Compute);

	// 使うディスクリプタヒープをセット
	resourceViewManager->SetDescriptorHeap(
		d3dCommand->GetCommand(
			CommandType::Compute).list.Get()
	);
}

void Cho::Operation()
{
	/*初期化*/
	Initialize();
	WriteLog("初期化終了");
	/*メインループ*/
	while (true) {
		frameRate->StartFrame();
		/*ウィンドウ終了リクエスト*/
		if (win->ProcessMessage()) {
			break;
		}
		SystemStateEvent();
		if (!win->IsEndApp()) {
			/*更新前処理*/
			PreUpdate();
			/*毎フレーム更新*/
			Update();
			/*更新後処理*/
			PostUpdate();
			/*描画前処理*/
			PreDraw();
#ifdef _DEBUG
			/*デバッグ描画前処理*/
			DebugPreDraw();
#endif // _DEBUG
			/*描画*/
			Draw();
			/*描画後処理*/
			PostDraw();
#ifdef _DEBUG
			/*デバッグ描画*/
			DebugDraw();
			/*デバッグ描画後処理*/
			DebugPostDraw();
#endif // _DEBUG
			/*フレーム更新*/
			frameRate->Update();
		}
		frameRate->EndFrame();
	}
	/*終了処理*/
	Finalize();
}

void Cho::Update()
{
	// ImGui受付開始
	imguiManager->Begin();

	SelectGameProject();

	inputManager->Update();

#ifdef _DEBUG
	// エディタを更新
	editorManager->Update();
#endif // _DEBUG

	// シーンを更新
	sceneManager->Update();

	systemManager->Update(*entityManager.get(), *componentManager.get(),0.01f);
}

void Cho::PostUpdate()
{
	d3dCommand->Close(COMPUTE, CommandType::Compute);

	d3dCommand->Signal(COMPUTE);
}

void Cho::PreDraw()
{
	// ImGui受付終了
	imguiManager->End();

	// コマンドリスト記録開始
	d3dCommand->Reset(CommandType::Draw);

	// 使うディスクリプタヒープをセット
	resourceViewManager->SetDescriptorHeap(
		d3dCommand->GetCommand(
			CommandType::Draw).list.Get()
	);
}

void Cho::Draw()
{
	// GBuffer描画前処理
	drawExecution->PreDrawGBuffer();
	systemManager->GBufferDraw(
		*entityManager.get(),
		*componentManager.get(),
		d3dCommand.get(),
		resourceViewManager.get(),
		dsvManager.get(),
		graphicsSystem.get(),
		textureLoader.get()
	);

	// GBufferMix描画前処理
	drawExecution->PreDrawGBufferMix();

	systemManager->GBufferMixDraw(
		*entityManager.get(),
		*componentManager.get(),
		d3dCommand.get(),
		resourceViewManager.get(),
		dsvManager.get(),
		graphicsSystem.get(),
		textureLoader.get()
	);

	// シーン描画前処理
	drawExecution->PreDraw();

	// シーン描画
	//sceneManager->Draw();

	// シーン描画
	systemManager->Draw(
		*entityManager.get(),
		*componentManager.get(),
		d3dCommand.get(),
		resourceViewManager.get(),
		graphicsSystem.get(),
		textureLoader.get()
	);

	/*drawExecution->DebugPreDrawGBufferMix();
	systemManager->DebugGBufferMixDraw(
		*entityManager.get(),
		*componentManager.get(),
		d3dCommand.get(),
		resourceViewManager.get(),
		graphicsSystem.get(),
		textureLoader.get(),
		editorManager->GetDebugCameraComp()
	);*/

	drawExecution->DebugPreDraw();

	// デバッグ用
	systemManager->DebugDraw(
		*entityManager.get(),
		*componentManager.get(),
		d3dCommand.get(),
		resourceViewManager.get(),
		graphicsSystem.get(),
		textureLoader.get(),
		editorManager->GetDebugCameraComp()
	);
}

void Cho::PostDraw()
{
	systemManager->PostDraw(
		*entityManager.get(),
		*componentManager.get(),
		d3dCommand.get(),
		resourceViewManager.get()
	);

	// 描画後処理
	drawExecution->PostDraw();

	// ImGui描画
	imguiManager->Draw();

	// 描画終了処理
	drawExecution->End();

	resourceViewManager->Reset();
}

#ifdef _DEBUG
void Cho::DebugPreDraw()
{
	
}

void Cho::DebugDraw()
{
	
}

void Cho::DebugPostDraw()
{
}
#endif // _DEBUG

void Cho::Load()
{
#ifdef _DEBUG
	/*ImGuiのスタイル読み込み*/
	jsonFileLoader->LoadStyleFromProject();
#endif // _DEBUG

	/*テクスチャリソースの読み込み*/
	textureLoader->FirstResourceLoad("Cho\\Resources\\Texture\\");
	WriteLog("テクスチャ読み込み");

	if (SystemState::GetInstance().IsEngineTest()) {
		textureLoader->FirstResourceLoad("EngineTest\\Assets\\Textures\\");
		WriteLog("Gameテクスチャ読み込み");
		modelLoader->FirstResourceLoad("EngineTest\\Assets\\Models\\");
		WriteLog("Gameモデル読み込み");
	}
	else {
		textureLoader->FirstResourceLoad("Game\\Assets\\Textures\\");
		WriteLog("Gameテクスチャ読み込み");
		modelLoader->FirstResourceLoad("Game\\Assets\\Models\\");
		WriteLog("Gameモデル読み込み");
	}
	{
		std::unique_lock<std::mutex> lock(loadTaskMutex);
		isLoadTaskEnd = true;
	}
}

void Cho::StartSetUp()
{
#ifdef _DEBUG

	startSetting->Initialize(
		resourceViewManager.get(),
		rtvManager.get(),
		drawExecution.get(),
		entityManager.get(),
		componentManager.get(),
		systemManager.get(),
		prefabManager.get(),
		sceneManager.get(),
		editorManager.get()
	);

#endif // _DEBUG


	/*テクスチャリソースの読み込み*/
	/*std::string textureRoot = startSetting->GetProjectRoot();
	std::string projectName = ProjectName();
	textureRoot = textureRoot + "\\" + projectName + "\\" + "Assets\\Texture\\";
	textureLoader->FirstResourceLoad(textureRoot);*/
}

void Cho::Save()
{
	saveManager = std::make_unique<SaveManager>();
	saveManager->Initialize(
		scriptManager.get(),
		entityManager.get(),
		componentManager.get(),
		prefabManager.get(),
		sceneManager.get()
	);

	saveManager->Save(jsonFileLoader.get());

	/*ImGuiのスタイルを保存*/
	jsonFileLoader->SaveStyleToProject();

	saveManager.reset();
	saveManager = nullptr;
}

void Cho::SystemStateEvent()
{
	// ウィンドウサイズ変更時スワップチェーン、RTVのリサイズ
	if (SystemState::GetInstance().WindowResize()) {
		d3dCommand->Signal(DIRECT);

		// 既存のリソースの解放,再作成
		dsvManager->Resize();
		drawExecution->ResizeOffscreenRenderTex();
		rtvManager->ResizeSwapChain();
	}
}

void Cho::SelectGameProject()
{
	static bool end = false;
	if (end) {
		return;
	}
	{
		// プロジェクトの選択、作成
		if (!startSetting->IsProject()) {
			startSetting->SelectedProject();
		} else
		{
			if (startSetting->IsNew()) {
				SystemState::GetInstance().SetProjectName(startSetting->GetProjectName());
				SystemState::GetInstance().SetProjectRoot(startSetting->GetProjectRoot());

				// 最初のシーンを作成
				if (SystemState::GetInstance().IsEngineTest()) {
					sceneManager->ChangeScene("TestTitle");
				}
				else {
					sceneManager->ChangeScene("GameScene");
				}

				// 新プロジェクトの時のみ
				/*スターター*/
				StartSetUp();
				//startSetting->LoadProject(jsonFileLoader.get());
				//startSetting->CreateProject();
#ifdef _DEBUG
				//ScriptProject::GenerateSolutionAndProject();
#endif // _DEBUG
				// 解放
				startSetting.reset();

				end = true;
			} else {
				SystemState::GetInstance().SetProjectName(startSetting->GetProjectName());
				SystemState::GetInstance().SetProjectRoot(startSetting->GetProjectRoot());

				// 最初のシーンを作成
				sceneManager->ChangeScene("MainScene");

				startSetting->LoadProject(jsonFileLoader.get());

				// 解放
				startSetting.reset();

				end = true;
			}
		}
	}
}
