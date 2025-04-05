#pragma once
#include "GameCore/SceneManager/SceneManager.h"
#include "GameCore/ECS/ECSManager.h"
#include "GameCore/SystemManager/SystemManager.h"
#include "GameCore/ObjectContainer/ObjectContainer.h"
#include "GameCore/GameCoreCommand/GameCoreCommand.h"
class ResourceManager;
class GraphicsEngine;

class GameCore
{
public:
	GameCore()
	{
		
	}
	~GameCore() 
	{
	}
	void Initialize(ResourceManager* resourceManager);
	void Start(ResourceManager& resourceManager);
	void Update(ResourceManager& resourceManager, GraphicsEngine& graphicsEngine);
	SceneManager* GetSceneManager() { return m_pSceneManager.get(); }
	ECSManager* GetECSManager() { return m_pECSManager.get(); }
	SystemManager* GetSystemManager() { return m_pSystemManager.get(); }
	ObjectContainer* GetObjectContainer() { return m_pObjectContainer.get(); }
	GameCoreCommand* GetGameCoreCommand() { return m_pGameCoreCommand.get(); }
private:
	void CreateSystems(ResourceManager* resourceManager);

	// シーンマネージャー
	std::unique_ptr<SceneManager> m_pSceneManager = nullptr;
	// ECSマネージャ
	std::unique_ptr<ECSManager> m_pECSManager = nullptr;
	// Systemマネージャ
	std::unique_ptr<SystemManager> m_pSystemManager = nullptr;
	// オブジェクトコンテナ
	std::unique_ptr<ObjectContainer> m_pObjectContainer = nullptr;
	// コマンド
	std::unique_ptr<GameCoreCommand> m_pGameCoreCommand = nullptr;
};

