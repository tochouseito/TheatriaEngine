#include "pch.h"
#include "GameCore.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Resources/ResourceManager/ResourceManager.h"

void GameCore::Initialize(ResourceManager* resourceManager)
{
	// シーンマネージャーの生成
	m_pSceneManager = std::make_unique<SceneManager>(resourceManager);
	// ECSマネージャの生成
	m_pECSManager = std::make_unique<ECSManager>();
	// Systemマネージャの生成
	m_pSystemManager = std::make_unique<SystemManager>();
	// オブジェクトコンテナの生成
	m_pObjectContainer = std::make_unique<ObjectContainer>();
	// コマンドの生成
	m_pGameCoreCommand = std::make_unique<GameCoreCommand>(m_pSceneManager.get(), m_pECSManager.get(), m_pSystemManager.get(), m_pObjectContainer.get());
}

void GameCore::Start(ResourceManager& resourceManager)
{
	resourceManager;
}

void GameCore::Update(ResourceManager& resourceManager, GraphicsEngine& graphicsEngine)
{
	m_pSceneManager->Update();
	m_pSystemManager->UpdateAll(m_pECSManager.get());
	resourceManager;
	graphicsEngine;
}

void GameCore::CreateSystems(ResourceManager* resourceManager)
{
	std::unique_ptr<ECSManager::ISystem> transformSystem = std::make_unique<ObjectSystem>(m_pECSManager.get(),resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pSystemManager->RegisterSystem(std::move(transformSystem));
	std::unique_ptr<ECSManager::ISystem> cameraSystem = std::make_unique<CameraSystem>(m_pECSManager.get(),resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pSystemManager->RegisterSystem(std::move(cameraSystem));
}

