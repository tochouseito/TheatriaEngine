#include "pch.h"
#include "GameCore.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Resources/ResourceManager/ResourceManager.h"

void GameCore::Initialize(ResourceManager* resourceManager)
{
	m_pSceneManager = std::make_unique<SceneManager>(resourceManager);
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
	std::unique_ptr<ECSManager::ISystem> transformSystem = std::make_unique<ObjectSystem>(m_pECSManager.get(), resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pSystemManager->RegisterSystem(std::move(transformSystem));
	std::unique_ptr<ECSManager::ISystem> cameraSystem = std::make_unique<CameraSystem>(m_pECSManager.get(), m_pIntegrationBuffer.get());
	m_pSystemManager->RegisterSystem(std::move(cameraSystem));
}

