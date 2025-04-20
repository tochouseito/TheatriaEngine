#include "pch.h"
#include "GameCore.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Platform/FileSystem/FileSystem.h"
#include "GameCore/Systems/SingleSystems.h"
#include "GameCore/Systems/MultiSystems.h"
#include "GameCore/Systems/EditorSystems.h"

void GameCore::Initialize(InputManager* input, ResourceManager* resourceManager)
{
	// シーンマネージャーの生成
	m_pSceneManager = std::make_unique<SceneManager>(resourceManager);
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
	CreateSystems(input,resourceManager);
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
	// ゲームが実行中でなければreturn
	if (isRunning)
	{
		Log::Write(LogLevel::Info, "GameCore Update");
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
	FileSystem::ScriptProject::LoadScriptDLL();
	isRunning = true;
	// StartSystemの実行
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
	// DLLのアンロード
	FileSystem::ScriptProject::UnloadScriptDLL();
	isRunning = false;
}

void GameCore::CreateSystems(InputManager* input, ResourceManager* resourceManager)
{
	// シングルシステム
	// 初期化システムの登録
	std::unique_ptr<ECSManager::ISystem> tfStateSystem = std::make_unique<TransformInitializeSystem>(m_pECSManager.get());
	m_pSingleSystemManager->RegisterSystem(std::move(tfStateSystem), SystemState::Initialize);
	std::unique_ptr<ECSManager::ISystem> scriptGenerateSystem = std::make_unique<ScriptGenerateInstanceSystem>(m_pObjectContainer.get(), input, m_pECSManager.get(), resourceManager);
	m_pSingleSystemManager->RegisterSystem(std::move(scriptGenerateSystem), SystemState::Initialize);
	std::unique_ptr<ECSManager::ISystem> scriptInitializeSystem = std::make_unique<ScriptInitializeSystem>(m_pObjectContainer.get(), input, m_pECSManager.get(), resourceManager);
	m_pSingleSystemManager->RegisterSystem(std::move(scriptInitializeSystem), SystemState::Initialize);
	std::unique_ptr<ECSManager::ISystem> physicsSystem = std::make_unique<Rigidbody2DInitSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(physicsSystem), SystemState::Initialize);
	std::unique_ptr<ECSManager::ISystem> boxInitSystem = std::make_unique<BoxCollider2DInitSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(boxInitSystem), SystemState::Initialize);
	// 更新システムの登録
	std::unique_ptr<ECSManager::ISystem> scriptUpdateSystem = std::make_unique<ScriptUpdateSystem>(m_pObjectContainer.get(), input, m_pECSManager.get(), resourceManager);
	m_pSingleSystemManager->RegisterSystem(std::move(scriptUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> tfUpdateSystem = std::make_unique<TransformUpdateSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pSingleSystemManager->RegisterSystem(std::move(tfUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> cameraSystem = std::make_unique<CameraUpdateSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pSingleSystemManager->RegisterSystem(std::move(cameraSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> collisionSystem = std::make_unique<CollisionSystem>(m_pECSManager.get(), resourceManager, input, m_pObjectContainer.get());
	m_pSingleSystemManager->RegisterSystem(std::move(collisionSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> rbUpdateSystem = std::make_unique<Rigidbody2DUpdateSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(rbUpdateSystem), SystemState::Update);
	// クリーンアップシステムの登録
	std::unique_ptr<ECSManager::ISystem> tfFinalizeSystem = std::make_unique<TransformFinalizeSystem>(m_pECSManager.get());
	m_pSingleSystemManager->RegisterSystem(std::move(tfFinalizeSystem), SystemState::Finalize);
	std::unique_ptr<ECSManager::ISystem> scriptFinalizeSystem = std::make_unique<ScriptFinalizeSystem>(m_pECSManager.get());
	m_pSingleSystemManager->RegisterSystem(std::move(scriptFinalizeSystem), SystemState::Finalize);
	std::unique_ptr<ECSManager::ISystem> physicsResetSystem = std::make_unique<Rigidbody2DResetSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(physicsResetSystem), SystemState::Finalize);
	// マルチシステム
	// 初期化システムの登録
	// 更新システムの登録
	std::unique_ptr<ECSManager::IMultiSystem> lineRendererSystem = std::make_unique<LineRendererSystem>(m_pECSManager.get(), resourceManager);
	m_pMultiSystemManager->RegisterSystem(std::move(lineRendererSystem), SystemState::Update);
	// クリーンアップシステムの登録

	// エディタシステム
	// シングルシステムの生成
	std::unique_ptr<ECSManager::ISystem> transformEditorSystem = std::make_unique<TransformEditorSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pEditorSingleSystem->RegisterSystem(std::move(transformEditorSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> cameraEditorSystem = std::make_unique<CameraEditorSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pEditorSingleSystem->RegisterSystem(std::move(cameraEditorSystem), SystemState::Update);
	// マルチシステムの生成
	std::unique_ptr<ECSManager::IMultiSystem> lineRendererEditorSystem = std::make_unique<LineRendererSystem>(m_pECSManager.get(), resourceManager);
	m_pEditorMultiSystem->RegisterSystem(std::move(lineRendererEditorSystem), SystemState::Update);
}

