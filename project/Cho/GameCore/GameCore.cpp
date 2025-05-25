#include "pch.h"
#include "GameCore.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Platform/FileSystem/FileSystem.h"
#include "GameCore/Systems/SingleSystems.h"
#include "GameCore/Systems/MultiSystems.h"
#include "GameCore/Systems/EditorSystems.h"
#include "EngineCommand/EngineCommands.h"

void GameCore::Initialize(InputManager* input, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine)
{
	// シーンマネージャーの生成
	m_pSceneManager = std::make_unique<SceneManager>(this,resourceManager);
	// ECSマネージャの生成
	m_pECSManager = std::make_unique<ECSManager>();
	// オブジェクトコンテナの生成
	m_pObjectContainer = std::make_unique<ObjectContainer>(m_pECSManager.get(), resourceManager, input);
	// Systemマネージャの生成
	m_pSingleSystemManager = std::make_unique<SingleSystemManager>();
	m_pMultiSystemManager = std::make_unique<MultiSystemManager>();
	// エディタのシステムマネージャの生成
	m_pEditorSingleSystem = std::make_unique<SingleSystemManager>();
	m_pEditorMultiSystem = std::make_unique<MultiSystemManager>();
	// box2dの生成
	//b2Vec2 gravity(0.0f, -9.8f);
	b2Vec2 gravity(0.0f, 0.0f);
	m_pPhysicsWorld = std::make_unique<b2World>(gravity);
	m_pContactListener = std::make_unique<ContactListener2D>(m_pECSManager.get(), resourceManager, input, m_pObjectContainer.get());
	m_pPhysicsWorld->SetContactListener(m_pContactListener.get());
	// システムの生成
	input;resourceManager;graphicsEngine;
	m_EnvironmentData.ambientColor = { 0.01f,0.01f,0.01f,1.0f };
}

void GameCore::Start(ResourceManager& resourceManager)
{
	Log::Write(LogLevel::Info, "GameCore Start");
	m_pSingleSystemManager->StartAll(m_pECSManager.get());
	m_pMultiSystemManager->StartAll(m_pECSManager.get());
	Log::Write(LogLevel::Info, "GameCore Start End");
	resourceManager;
}

void GameCore::Update(ResourceManager& resourceManager, GraphicsEngine& graphicsEngine)
{
	m_pSceneManager->Update();
	// 環境設定の更新
	UpdateEnvironmentSetting();
	// ゲームが実行中でなければreturn
	if (isRunning)
	{
		Log::Write(LogLevel::Info, "GameCore Update");
		InitializeGenerateObject();
		m_pSingleSystemManager->UpdateAll(m_pECSManager.get());
		m_pMultiSystemManager->UpdateAll(m_pECSManager.get());
		Log::Write(LogLevel::Info, "GameCore Update End");
	} else
	{
		m_pEditorSingleSystem->UpdateAll(m_pECSManager.get());
		m_pEditorMultiSystem->UpdateAll(m_pECSManager.get());
	}
	resourceManager;
	graphicsEngine;
}

void GameCore::GameRun()
{
	if (isRunning)
	{
		return;
	}
	// ゲームの初期化処理
	// スクリプトDLLの読み込み
	/*if (!FileSystem::ScriptProject::BuildScriptDLL())
	{
		return;
	}*/
	Cho::FileSystem::ScriptProject::LoadScriptDLL();
	isRunning = true;
	// StartSystemの実行
	m_pObjectContainer->InitializeAllGameObjects();
	m_pSingleSystemManager->StartAll(m_pECSManager.get());
	m_pMultiSystemManager->StartAll(m_pECSManager.get());
}

void GameCore::GameStop()
{
	if (!isRunning)
	{
		return;
	}
	// スクリプトのインスタンスを解放
	m_pSingleSystemManager->EndAll(m_pECSManager.get());
	m_pMultiSystemManager->EndAll(m_pECSManager.get());
	// 生成されたオブジェクトを削除
	ClearGenerateObject();
	m_GameGenerateID.clear();
	m_pObjectContainer->InitializeAllGameObjects();
	// DLLのアンロード
	Cho::FileSystem::ScriptProject::UnloadScriptDLL();
	isRunning = false;
}

void GameCore::InitializeGenerateObject()
{
	for (const ObjectID& id : m_GameGenerateID)
	{
		GameObject& object = m_pObjectContainer->GetGameObject(id);
		if (!object.IsActive()) { continue; }
		// オブジェクトの初期化
		Entity entity = object.GetEntity();
		// TransformComponentを取得
		TransformComponent* transform = m_pECSManager->GetComponent<TransformComponent>(entity);
		if (!transform) { continue; }
		// TransformComponentの初期化
		tfOnceSystem->Start(*transform);
		// スクリプトの取得
		ScriptComponent* script = m_pECSManager->GetComponent<ScriptComponent>(entity);
		if (script)
		{
			// スクリプトの初期化
			scriptGenerateOnceSystem->InstanceGenerate(*script);
			scriptInitializeOnceSystem->StartScript(*script);
		}
		// Rigidbody2DComponentの取得
		Rigidbody2DComponent* rb = m_pECSManager->GetComponent<Rigidbody2DComponent>(entity);
		if (rb)
		{
			// Rigidbody2DComponentの初期化
			physicsOnceSystem->CreateBody(entity, *transform, *rb);
		}
		// BoxCollider2DComponentの取得
		BoxCollider2DComponent* box = m_pECSManager->GetComponent<BoxCollider2DComponent>(entity);
		if (box)
		{
			// BoxCollider2DComponentの初期化
			boxInitOnceSystem->CreateFixture(*transform, *rb, *box);
		}
		// 初期化済みのIDを追加
		m_GameInitializedID.push_back(id);
	}
	// 初期化済みのIDをクリア
	m_GameGenerateID.clear();
}

void GameCore::ClearGenerateObject()
{
	for (const ObjectID& id : m_GameInitializedID)
	{
		GameObject& object = m_pObjectContainer->GetGameObject(id);
		std::unique_ptr<DeleteObjectCommand> command = std::make_unique<DeleteObjectCommand>(object.GetID().value());
		command->Execute(m_EngineCommand);
	}
	m_GameInitializedID.clear();
}

void GameCore::UpdateEnvironmentSetting()
{
	// 環境情報バッファ
	ConstantBuffer<BUFFER_DATA_ENVIRONMENT>* envBuffer = m_EngineCommand->GetResourceManager()->GetEnvironmentBuffer();
	envBuffer->UpdateData(m_EnvironmentData);
}

void GameCore::CreateSystems(InputManager* input, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine)
{
	// シングルシステム
	// 初期化システムの登録
	// TransformComponentの初期化
	std::unique_ptr<ECSManager::ISystem> tfStateSystem = std::make_unique<TransformInitializeSystem>(m_pECSManager.get());
	m_pSingleSystemManager->RegisterSystem(std::move(tfStateSystem), SystemState::Initialize);
	// Particleの初期化
	std::unique_ptr<ECSManager::ISystem> particleInitSystem = std::make_unique<ParticleInitializeSystem>(m_pECSManager.get(),resourceManager,graphicsEngine);
	m_pSingleSystemManager->RegisterSystem(std::move(particleInitSystem), SystemState::Initialize);
	// スクリプトのインスタンス生成
	std::unique_ptr<ECSManager::ISystem> scriptGenerateSystem = std::make_unique<ScriptGenerateInstanceSystem>(m_pObjectContainer.get(), input, m_pECSManager.get(), resourceManager);
	m_pSingleSystemManager->RegisterSystem(std::move(scriptGenerateSystem), SystemState::Initialize);
	// スクリプトの初期化
	std::unique_ptr<ECSManager::ISystem> scriptInitializeSystem = std::make_unique<ScriptInitializeSystem>(m_pObjectContainer.get(), input, m_pECSManager.get(), resourceManager);
	m_pSingleSystemManager->RegisterSystem(std::move(scriptInitializeSystem), SystemState::Initialize);
	// Rigidbody2DComponentの初期化
	std::unique_ptr<ECSManager::ISystem> physicsSystem = std::make_unique<Rigidbody2DInitSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(physicsSystem), SystemState::Initialize);
	// BoxCollider2DComponentの初期化
	std::unique_ptr<ECSManager::ISystem> boxInitSystem = std::make_unique<BoxCollider2DInitSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(boxInitSystem), SystemState::Initialize);
	// 更新システムの登録
	std::unique_ptr<ECSManager::ISystem> scriptUpdateSystem = std::make_unique<ScriptUpdateSystem>(m_pObjectContainer.get(), input, m_pECSManager.get(), resourceManager);
	m_pSingleSystemManager->RegisterSystem(std::move(scriptUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> materialUpdateSystem = std::make_unique<MaterialUpdateSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Material));
	m_pSingleSystemManager->RegisterSystem(std::move(materialUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> tfUpdateSystem = std::make_unique<TransformUpdateSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pSingleSystemManager->RegisterSystem(std::move(tfUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> animationUpdateSystem = std::make_unique<AnimationUpdateSystem>(m_pECSManager.get(), resourceManager,graphicsEngine);
	m_pSingleSystemManager->RegisterSystem(std::move(animationUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> cameraSystem = std::make_unique<CameraUpdateSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pSingleSystemManager->RegisterSystem(std::move(cameraSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> emitterUpdateSystem = std::make_unique<EmitterUpdateSystem>(m_pECSManager.get(), resourceManager, graphicsEngine);
	m_pSingleSystemManager->RegisterSystem(std::move(emitterUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> lightUpdateSystem = std::make_unique<LightUpdateSystem>(m_pECSManager.get(), resourceManager,graphicsEngine);
	m_pSingleSystemManager->RegisterSystem(std::move(lightUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> particleUpdateSystem = std::make_unique<ParticleUpdateSystem>(m_pECSManager.get(), resourceManager,graphicsEngine);
	m_pSingleSystemManager->RegisterSystem(std::move(particleUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> boxUpdateSystem = std::make_unique<BoxCollider2DUpdateSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(boxUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> collisionSystem = std::make_unique<CollisionSystem>(m_pECSManager.get(), resourceManager, input, m_pObjectContainer.get());
	m_pSingleSystemManager->RegisterSystem(std::move(collisionSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> rbUpdateSystem = std::make_unique<Rigidbody2DUpdateSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(rbUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> uiUpdateSystem = std::make_unique<UIUpdateSystem>(m_pECSManager.get(), resourceManager, graphicsEngine,resourceManager->GetIntegrationBuffer(IntegrationDataType::UISprite));
	m_pSingleSystemManager->RegisterSystem(std::move(uiUpdateSystem), SystemState::Update);
	// クリーンアップシステムの登録
	std::unique_ptr<ECSManager::ISystem> tfFinalizeSystem = std::make_unique<TransformFinalizeSystem>(m_pECSManager.get());
	m_pSingleSystemManager->RegisterSystem(std::move(tfFinalizeSystem), SystemState::Finalize);
	std::unique_ptr<ECSManager::ISystem> scriptFinalizeSystem = std::make_unique<ScriptFinalizeSystem>(m_pECSManager.get());
	m_pSingleSystemManager->RegisterSystem(std::move(scriptFinalizeSystem), SystemState::Finalize);
	std::unique_ptr<ECSManager::ISystem> physicsResetSystem = std::make_unique<Rigidbody2DResetSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(physicsResetSystem), SystemState::Finalize);
	std::unique_ptr<ECSManager::ISystem> particleFinaSystem = std::make_unique<ParticleInitializeSystem>(m_pECSManager.get(), resourceManager, graphicsEngine);
	m_pSingleSystemManager->RegisterSystem(std::move(particleFinaSystem), SystemState::Finalize);
	// マルチシステム
	// 初期化システムの登録
	// 更新システムの登録
	std::unique_ptr<ECSManager::IMultiSystem> lineRendererSystem = std::make_unique<LineRendererSystem>(m_pECSManager.get(), resourceManager);
	m_pMultiSystemManager->RegisterSystem(std::move(lineRendererSystem), SystemState::Update);
	// クリーンアップシステムの登録

	// エディタシステム
	// シングルシステムの生成
	std::unique_ptr<ECSManager::ISystem> materialEditorSystem = std::make_unique<MaterialEditorSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Material));
	m_pEditorSingleSystem->RegisterSystem(std::move(materialEditorSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> transformEditorSystem = std::make_unique<TransformEditorSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pEditorSingleSystem->RegisterSystem(std::move(transformEditorSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> cameraEditorSystem = std::make_unique<CameraEditorSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pEditorSingleSystem->RegisterSystem(std::move(cameraEditorSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> lightEditorSystem = std::make_unique<LightUpdateSystem>(m_pECSManager.get(), resourceManager, graphicsEngine);
	m_pEditorSingleSystem->RegisterSystem(std::move(lightEditorSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> emitterEditorSystem = std::make_unique<EmitterEditorUpdateSystem>(m_pECSManager.get(), resourceManager, graphicsEngine);
	m_pEditorSingleSystem->RegisterSystem(std::move(emitterEditorSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> effectEditorSystem = std::make_unique<EffectEditorUpdateSystem>(m_pECSManager.get(), m_EngineCommand);
	m_pEditorSingleSystem->RegisterSystem(std::move(effectEditorSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> uiUpdateEditorSystem = std::make_unique<UIUpdateSystem>(m_pECSManager.get(), resourceManager, graphicsEngine, resourceManager->GetIntegrationBuffer(IntegrationDataType::UISprite));
	m_pEditorSingleSystem->RegisterSystem(std::move(uiUpdateEditorSystem), SystemState::Update);

	// マルチシステムの生成
	std::unique_ptr<ECSManager::IMultiSystem> lineRendererEditorSystem = std::make_unique<LineRendererSystem>(m_pECSManager.get(), resourceManager);
	m_pEditorMultiSystem->RegisterSystem(std::move(lineRendererEditorSystem), SystemState::Update);

	// 更新中に生成されたオブジェクトの初期化システム
	tfOnceSystem = std::make_unique<TransformInitializeSystem>(m_pECSManager.get());
	scriptGenerateOnceSystem = std::make_unique<ScriptGenerateInstanceSystem>(m_pObjectContainer.get(), input, m_pECSManager.get(), resourceManager);
	scriptInitializeOnceSystem = std::make_unique<ScriptInitializeSystem>(m_pObjectContainer.get(), input, m_pECSManager.get(), resourceManager);
	physicsOnceSystem = std::make_unique<Rigidbody2DInitSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	boxInitOnceSystem = std::make_unique<BoxCollider2DInitSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
}

