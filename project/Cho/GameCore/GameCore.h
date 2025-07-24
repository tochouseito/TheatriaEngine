#pragma once
#include "GameCore/SceneManager/SceneManager.h"
#include "GameCore/ECS/ECSManager.h"
#include "GameCore/GameWorld/GameWorld.h"
#include "GameCore/PhysicsEngine/PhysicsEngine.h"
#include "Core/Utility/CompBufferData.h"
class InputManager;
class ResourceManager;
class GraphicsEngine;
class EngineCommand;

// ECSのイベントリスナー
class ComponentEventDispatcher : public IComponentEventListener
{
public:
	ComponentEventDispatcher(EngineCommand* engineCommand) : m_EngineCommand(engineCommand) {}
private:
	EngineCommand* m_EngineCommand = nullptr;
};

class GameCore
{
	friend class ChoEngine;
public:
	GameCore()
	{
	}
	~GameCore() 
	{
	}
	void Initialize(ResourceManager* resourceManager, GraphicsEngine* graphicsEngine);
	void SetEngineCommandPtr(EngineCommand* engineCommand) { m_EngineCommand = engineCommand; }
	void Start();
	void Update();
	bool IsRunning() const { return isRunning; }
	void GameRun();
	void GameStop();
	b2World* GetPhysicsWorld() { return m_pPhysicsWorld.get(); }
	// 環境設定更新
	void UpdateEnvironmentSetting();

	GameWorld* GetGameWorld() { return m_pGameWorld.get(); }
	SceneManager* GetSceneManager() { return m_pSceneManager.get(); }
	ECSManager* GetECSManager() { return m_pECSManager.get(); }
private:
	void RegisterECSEvents();
	void RegisterECSSystems(ResourceManager* resourceManager, GraphicsEngine* graphicsEngine);
	
	EngineCommand* m_EngineCommand = nullptr;

	// シーンマネージャー
	std::unique_ptr<SceneManager> m_pSceneManager = nullptr;
	// ECSマネージャ
	std::unique_ptr<ECSManager> m_pECSManager = nullptr;
	// ゲームワールド
	std::unique_ptr<GameWorld> m_pGameWorld = nullptr;
	// ゲーム実行フラグ
	bool isRunning = false;
	// 環境情報
	BUFFER_DATA_ENVIRONMENT m_EnvironmentData;
	// box2d
	std::unique_ptr<physics::d2::Id2World> m_pPy2dWorld = nullptr;
	std::unique_ptr<ContactListener2D> m_pContactListener = nullptr;
	// ECSイベントディスパッチャー
	std::shared_ptr<ComponentEventDispatcher> m_pComponentEventDispatcher = nullptr;
};

