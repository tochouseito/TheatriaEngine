#pragma once
class ECSManager;
class SystemManager;
class ObjectContainer;
class SceneManager;
class GameCoreCommand
{
public:
	// Constructor
	GameCoreCommand(SceneManager* sceneManager, ECSManager* ecsManager, SystemManager* systemManager, ObjectContainer* objectContainer) :
		m_SceneManager(sceneManager),
		m_ECSManager(ecsManager),
		m_SystemManager(systemManager),
		m_ObjectContainer(objectContainer)
	{
	}
	// Destructor
	~GameCoreCommand()
	{
	}
private:
	// GameObjectを作成
	void AddGameObject();
	// TransformComponentを追加
	void AddTransformComponent(const uint32_t& entity);
	// MeshComponentを追加
	void AddMeshComponent(const uint32_t& entity);
	// RenderComponentを追加
	void AddRenderComponent(const uint32_t& entity);
	// CameraComponentを追加
	void AddCameraComponent(const uint32_t& entity);
	// SceneのMainCameraを設定
	uint32_t SetMainCamera(const uint32_t& setCameraID);

	SceneManager* m_SceneManager = nullptr;
	ECSManager* m_ECSManager = nullptr;
	SystemManager* m_SystemManager = nullptr;
	ObjectContainer* m_ObjectContainer = nullptr;
};

