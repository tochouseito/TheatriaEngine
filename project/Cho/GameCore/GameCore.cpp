#include "pch.h"
#include "GameCore.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Platform/FileSystem/FileSystem.h"
#include "GameCore/Systems/SingleSystems.h"
#include "GameCore/Systems/MultiSystems.h"

void GameCore::Initialize(ResourceManager* resourceManager)
{
	// シーンマネージャーの生成
	m_pSceneManager = std::make_unique<SceneManager>(resourceManager);
	// ECSマネージャの生成
	m_pECSManager = std::make_unique<ECSManager>();
	// Systemマネージャの生成
	m_pSingleSystemManager = std::make_unique<SingleSystemManager>();
	m_pMultiSystemManager = std::make_unique<MultiSystemManager>();
	// box2dの生成
	b2Vec2 gravity(0.0f, -9.8f);
	m_pPhysicsWorld = std::make_unique<b2World>(gravity);
	// システムの生成
	CreateSystems(resourceManager);
	// オブジェクトコンテナの生成
	m_pObjectContainer = std::make_unique<ObjectContainer>();
	// コマンドの生成
	m_pGameCoreCommand = std::make_unique<GameCoreCommand>(m_pSceneManager.get(), m_pECSManager.get(), m_pObjectContainer.get(),this);
}

void GameCore::Start(ResourceManager& resourceManager)
{
	m_pSingleSystemManager->StartAll(m_pECSManager.get());
	m_pMultiSystemManager->StartAll(m_pECSManager.get());
	resourceManager;
}

void GameCore::Update(ResourceManager& resourceManager, GraphicsEngine& graphicsEngine)
{
	m_pSceneManager->Update();
	// ゲームが実行中でなければreturn
	if (!isRunning)
	{
		return;
	}
	//static_cast<Rigidbody2DUpdateSystem*>(rbUpdateSystemRawPtr)->StepSimulation();
	m_pSingleSystemManager->UpdateAll(m_pECSManager.get());
	m_pMultiSystemManager->UpdateAll(m_pECSManager.get());
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

void GameCore::CreateSystems(ResourceManager* resourceManager)
{
	// シングルシステム
	// 初期化システムの登録
	std::unique_ptr<ECSManager::ISystem> tfStateSystem = std::make_unique<TransformInitializeSystem>(m_pECSManager.get());
	m_pSingleSystemManager->RegisterSystem(std::move(tfStateSystem), SystemState::Initialize);
	std::unique_ptr<ECSManager::ISystem> scriptInitializeSystem = std::make_unique<ScriptInitializeSystem>(m_pECSManager.get(),resourceManager->GetScriptContainer());
	m_pSingleSystemManager->RegisterSystem(std::move(scriptInitializeSystem), SystemState::Initialize);
	std::unique_ptr<ECSManager::ISystem> physicsSystem = std::make_unique<Rigidbody2DInitSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(physicsSystem), SystemState::Initialize);
	std::unique_ptr<ECSManager::ISystem> boxInitSystem = std::make_unique<BoxCollider2DInitSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(boxInitSystem), SystemState::Initialize);
	// 更新システムの登録
	std::unique_ptr<ECSManager::ISystem> tfUpdateSystem = std::make_unique<TransformUpdateSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pSingleSystemManager->RegisterSystem(std::move(tfUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> cameraSystem = std::make_unique<CameraUpdateSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pSingleSystemManager->RegisterSystem(std::move(cameraSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> scriptUpdateSystem = std::make_unique<ScriptUpdateSystem>(m_pECSManager.get());
	m_pSingleSystemManager->RegisterSystem(std::move(scriptUpdateSystem), SystemState::Update);
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

}

