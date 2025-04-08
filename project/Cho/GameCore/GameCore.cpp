#include "pch.h"
#include "GameCore.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Platform/FileSystem/FileSystem.h"

void GameCore::Initialize(ResourceManager* resourceManager)
{
	// シーンマネージャーの生成
	m_pSceneManager = std::make_unique<SceneManager>(resourceManager);
	// ECSマネージャの生成
	m_pECSManager = std::make_unique<ECSManager>();
	// Systemマネージャの生成
	m_pUpdateSystem = std::make_unique<SystemManager>();
	m_pStartSystem = std::make_unique<SystemManager>();
	m_pCleanupSystem = std::make_unique<SystemManager>();
	// システムの生成
	CreateSystems(resourceManager);
	// オブジェクトコンテナの生成
	m_pObjectContainer = std::make_unique<ObjectContainer>();
	// コマンドの生成
	m_pGameCoreCommand = std::make_unique<GameCoreCommand>(m_pSceneManager.get(), m_pECSManager.get(), m_pUpdateSystem.get(), m_pObjectContainer.get(),this);
}

void GameCore::Start(ResourceManager& resourceManager)
{
	m_pStartSystem->UpdateAll(m_pECSManager.get());
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
	m_pUpdateSystem->UpdateAll(m_pECSManager.get());
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
	m_pStartSystem->UpdateAll(m_pECSManager.get());
}

void GameCore::GameStop()
{
	if (!isRunning)
	{
		return;
	}
	// スクリプトのインスタンスを解放
	m_pCleanupSystem->UpdateAll(m_pECSManager.get());
	// DLLのアンロード
	FileSystem::ScriptProject::UnloadScriptDLL();
	isRunning = false;
}

void GameCore::CreateSystems(ResourceManager* resourceManager)
{
	// UpdateSystem
	std::unique_ptr<ECSManager::ISystem> transformSystem = std::make_unique<ObjectSystem>(m_pECSManager.get(),resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pUpdateSystem->RegisterSystem(std::move(transformSystem));
	std::unique_ptr<ECSManager::ISystem> cameraSystem = std::make_unique<CameraSystem>(m_pECSManager.get(),resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pUpdateSystem->RegisterSystem(std::move(cameraSystem));
	std::unique_ptr<ECSManager::ISystem> updateSystem = std::make_unique<ScriptUpdateSystem>(m_pECSManager.get());
	m_pUpdateSystem->RegisterSystem(std::move(updateSystem));
	// StartSystem
	std::unique_ptr<ECSManager::ISystem> transformStartSystem = std::make_unique<TransformStartSystem>(m_pECSManager.get());
	m_pStartSystem->RegisterSystem(std::move(transformStartSystem));
	std::unique_ptr<ECSManager::ISystem> startSystem = std::make_unique<ScriptStartSystem>(m_pECSManager.get());
	m_pStartSystem->RegisterSystem(std::move(startSystem));
	// CleanupSystem
	std::unique_ptr<ECSManager::ISystem> transformCleanupSystem = std::make_unique<TransformCleanupSystem>(m_pECSManager.get());
	m_pCleanupSystem->RegisterSystem(std::move(transformCleanupSystem));
	std::unique_ptr<ECSManager::ISystem> cleanupSystem = std::make_unique<ScriptCleanupSystem>(m_pECSManager.get());
	m_pCleanupSystem->RegisterSystem(std::move(cleanupSystem));
}

