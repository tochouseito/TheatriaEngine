#include "pch.h"
#include "GameCore.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"

void GameCore::Initialize()
{
	m_pSceneManager = std::make_unique<SceneManager>();
}

void GameCore::Start(ResourceManager& resourceManager)
{
	resourceManager;
}

void GameCore::Update(ResourceManager& resourceManager, GraphicsEngine& graphicsEngine)
{
	m_pSceneManager->Update();
	resourceManager;
	graphicsEngine;
}

void GameCore::Draw(GraphicsEngine& graphicsEngine)
{
	graphicsEngine;
}
