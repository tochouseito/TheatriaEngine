#pragma once
#include <string>
class ECSManager;
class SystemManager;
class GameObject;
class ObjectContainer;
class SceneManager;
class ModelManager;
class ResourceManager;
class GameCore;
class GameCoreCommand
{
	friend class Add3DObjectCommand;
	friend class AddCameraObjectCommand;
	friend class AddMeshFilterComponent;
	friend class AddMeshRendererComponent;
	friend class AddCameraComponent;
	friend class SetMainCamera;
	friend class AddScriptComponent;
	friend class AddLineRendererComponent;
public:
	// Constructor
	GameCoreCommand(SceneManager* sceneManager, ECSManager* ecsManager, ObjectContainer* objectContainer,GameCore* gameCore) :
		m_pSceneManager(sceneManager),
		m_pECSManager(ecsManager),
		m_pObjectContainer(objectContainer),
		m_pGameCore(gameCore)
	{
	}
	// Destructor
	~GameCoreCommand()
	{
	}
	// ポインタを取得
	SceneManager* GetSceneManagerPtr() { return m_pSceneManager; }
	ECSManager* GetECSManagerPtr() { return m_pECSManager; }
	ObjectContainer* GetObjectContainerPtr() { return m_pObjectContainer; }
	GameCore* GetGameCorePtr() { return m_pGameCore; }
private:
	// 3DObjectを追加
	GameObject* Add3DObject(const uint32_t& mapID);
	// CameraObjectを追加
	GameObject* AddCameraObject(const uint32_t& mapID, ResourceManager* resourceManager);
	// MeshFilterComponentを追加
	void AddMeshFilterComponent(const uint32_t& entity,ModelManager* modelManager);
	// MeshRendererComponentを追加
	void AddMeshRendererComponent(const uint32_t& entity);
	// CameraComponentを追加
	void AddCameraComponent(const uint32_t& entity,ResourceManager* resourceManager);
	// スクリプトコンポーネントを追加
	void AddScriptComponent(const uint32_t& entity, ResourceManager* resourceManager);
	// ラインレンダラーコンポーネントを追加
	void AddLineRendererComponent(const uint32_t& entity, ResourceManager* resourceManager,const uint32_t& mapID);

	// SceneのMainCameraを設定
	std::optional<uint32_t> SetMainCamera(const uint32_t& setCameraID);

	SceneManager* m_pSceneManager = nullptr;
	ECSManager* m_pECSManager = nullptr;
	ObjectContainer* m_pObjectContainer = nullptr;
	GameCore* m_pGameCore = nullptr;
};

