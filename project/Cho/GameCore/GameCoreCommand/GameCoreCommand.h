#pragma once
#include <string>
class ECSManager;
class SystemManager;
class ObjectContainer;
class SceneManager;
class ModelManager;
class ResourceManager;
class GameCoreCommand
{
	friend class AddGameObjectCommand;
	friend class AddTransformComponent;
	friend class AddMeshComponent;
	friend class AddRenderComponent;
	friend class AddCameraComponent;
	friend class SetMainCamera;
public:
	// Constructor
	GameCoreCommand(SceneManager* sceneManager, ECSManager* ecsManager, SystemManager* systemManager, ObjectContainer* objectContainer) :
		m_pSceneManager(sceneManager),
		m_pECSManager(ecsManager),
		m_pSystemManager(systemManager),
		m_pObjectContainer(objectContainer)
	{
	}
	// Destructor
	~GameCoreCommand()
	{
	}
	// ポインタを取得
	SceneManager* GetSceneManagerPtr() { return m_pSceneManager; }
	ECSManager* GetECSManagerPtr() { return m_pECSManager; }
	SystemManager* GetSystemManagerPtr() { return m_pSystemManager; }
	ObjectContainer* GetObjectContainerPtr() { return m_pObjectContainer; }
private:
	// GameObjectを作成
	void AddGameObject(const std::wstring& name,const uint32_t& type);
	// TransformComponentを追加
	void AddTransformComponent(const uint32_t& entity,const uint32_t& mapID);
	// MeshComponentを追加
	void AddMeshComponent(const uint32_t& entity,ModelManager* modelManager);
	// RenderComponentを追加
	void AddRenderComponent(const uint32_t& entity);
	// CameraComponentを追加
	void AddCameraComponent(const uint32_t& entity,ResourceManager* resourceManager);
	// SceneのMainCameraを設定
	std::optional<uint32_t> SetMainCamera(const uint32_t& setCameraID);

	SceneManager* m_pSceneManager = nullptr;
	ECSManager* m_pECSManager = nullptr;
	SystemManager* m_pSystemManager = nullptr;
	ObjectContainer* m_pObjectContainer = nullptr;
};

