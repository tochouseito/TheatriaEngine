#pragma once
#include "GameCore/SceneManager/SceneManager.h"
#include "GameCore/ECS/ECSManager.h"
#include "GameCore/SingleSystemManager/SingleSystemManager.h"
#include "GameCore/MultiSystemManager/MultiSystemManager.h"
#include "GameCore/ObjectContainer/ObjectContainer.h"
#include "GameCore/PhysicsEngine/PhysicsEngine.h"
#include "GameCore/Systems/SingleSystems.h"
class InputManager;
class ResourceManager;
class GraphicsEngine;
class EngineCommand;

class TransformInitializeSystem;
class ScriptGenerateInstanceSystem;
class ScriptInitializeSystem;
class Rigidbody2DInitSystem;
class BoxCollider2DInitSystem;

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
	void Initialize(InputManager* input,ResourceManager* resourceManager, GraphicsEngine* graphicsEngine);
	void SetEngineCommandPtr(EngineCommand* engineCommand) { m_EngineCommand = engineCommand; }
	void Start(ResourceManager& resourceManager);
	void Update(ResourceManager& resourceManager, GraphicsEngine& graphicsEngine);
	SceneManager* GetSceneManager() { return m_pSceneManager.get(); }
	ECSManager* GetECSManager() { return m_pECSManager.get(); }
	ObjectContainer* GetObjectContainer() { return m_pObjectContainer.get(); }
	bool IsRunning() const { return isRunning; }
	void GameRun();
	void GameStop();
	void AddGameGenerateObject(const ObjectID& id) { m_GameGenerateID.push_back(id); m_pObjectContainer->GetGameObject(id).Initialize(); }
	void InitializeGenerateObject();
	void ClearGenerateObject();
	b2World* GetPhysicsWorld() { return m_pPhysicsWorld.get(); }
private:
	void CreateSystems(InputManager* input, ResourceManager* resourceManager,GraphicsEngine* graphicsEngine);

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

	// ゲーム更新中に生成されたidを保持するコンテナ
	std::vector<ObjectID> m_GameGenerateID;
	// ゲーム実行中に生成され初期化済みのidを保持するコンテナ
	std::vector<ObjectID> m_GameInitializedID;
	std::unique_ptr<TransformInitializeSystem> tfOnceSystem;
	std::unique_ptr<ScriptGenerateInstanceSystem> scriptGenerateOnceSystem;
	std::unique_ptr<ScriptInitializeSystem> scriptInitializeOnceSystem;
	std::unique_ptr<Rigidbody2DInitSystem> physicsOnceSystem;
	std::unique_ptr<BoxCollider2DInitSystem> boxInitOnceSystem;
	EngineCommand* m_EngineCommand = nullptr;
};

