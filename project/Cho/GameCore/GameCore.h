#pragma once
#include "GameCore/SceneManager/SceneManager.h"
#include "GameCore/ECS/ECSManager.h"
#include "GameCore/SingleSystemManager/SingleSystemManager.h"
#include "GameCore/MultiSystemManager/MultiSystemManager.h"
#include "GameCore/ObjectContainer/ObjectContainer.h"
#include "GameCore/PhysicsEngine/PhysicsEngine.h"
class InputManager;
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
	void Initialize(InputManager* input,ResourceManager* resourceManager);
	void Start(ResourceManager& resourceManager);
	void Update(ResourceManager& resourceManager, GraphicsEngine& graphicsEngine);
	SceneManager* GetSceneManager() { return m_pSceneManager.get(); }
	ECSManager* GetECSManager() { return m_pECSManager.get(); }
	ObjectContainer* GetObjectContainer() { return m_pObjectContainer.get(); }
	bool IsRunning() const { return isRunning; }
	void GameRun();
	void GameStop();
private:
	void CreateSystems(InputManager* input, ResourceManager* resourceManager);

	// シーンマネージャー
	std::unique_ptr<SceneManager> m_pSceneManager = nullptr;
	// ECSマネージャ
	std::unique_ptr<ECSManager> m_pECSManager = nullptr;
	// システムマネージャ
	std::unique_ptr<SingleSystemManager> m_pSingleSystemManager = nullptr;
	// マルチシステムマネージャ
	std::unique_ptr<MultiSystemManager> m_pMultiSystemManager = nullptr;
	// エディタの更新システム
	std::unique_ptr<SingleSystemManager> m_pEditorSingleSystem = nullptr;
	std::unique_ptr<MultiSystemManager> m_pEditorMultiSystem = nullptr;
	// オブジェクトコンテナ
	std::unique_ptr<ObjectContainer> m_pObjectContainer = nullptr;
	// ゲーム実行フラグ
	bool isRunning = false;

	// box2d
	std::unique_ptr<b2World> m_pPhysicsWorld = nullptr;
	std::unique_ptr<ContactListener2D> m_pContactListener = nullptr;
};

