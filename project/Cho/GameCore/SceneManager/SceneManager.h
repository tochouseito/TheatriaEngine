#pragma once
#include "Cho/GameCore/ECS/ECSManager.h"
#include "Cho/GameCore/ObjectContainer/ObjectContainer.h"
#include <string>
using SceneID = uint32_t;
class SceneManager;
class BaseScene {
public:
	// Constructor
	BaseScene(SceneManager* sceneManger):
		m_SceneManager(sceneManger)
	{

	}
	// Destructor
	virtual ~BaseScene()
	{

	}

	virtual inline SceneID GetSceneID() const noexcept { return m_SceneID;}
	virtual inline const std::wstring& GetSceneName() const noexcept { return m_SceneName; }
	virtual inline void SetSceneName(const std::wstring& sceneName) { m_SceneName = sceneName; }
	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void Finalize() = 0;

	inline void SetSceneID(const SceneID sceneID) { m_SceneID = sceneID; }
protected:
	SceneID m_SceneID = 0;
	std::wstring m_SceneName = L"";
	SceneManager* m_SceneManager = nullptr;
	std::vector<uint32_t> useObjects;
};

class ScenePrefab : public BaseScene {
public:
	// Constructor
	ScenePrefab(SceneManager* sceneManger) :
		BaseScene(sceneManger)
	{
	}
	// Destructor
	~ScenePrefab() = default;

	void Start() override;
	void Update() override;
	void Finalize() override;
private:
};

class SceneManager
{
	friend class AddGameObjectCommand;
	friend class AddTransformComponent;
	friend class AddMeshComponent;
	friend class AddCameraComponent;
public:
	// Constructor
	SceneManager()
	{
		m_pECSManager = std::make_unique<ECSManager>();
		m_pObjectContainer = std::make_unique<ObjectContainer>();
	}
	// Destructor
	~SceneManager()
	{
	}

	// シーンを更新
	void Update();
	// シーンを追加
	void AddScene(const std::wstring& sceneName)
	{
		std::unique_ptr<ScenePrefab> pScene = std::make_unique<ScenePrefab>(this);
		SceneID sceneID = static_cast<uint32_t>(m_pScenes.push_back(std::move(pScene.get())));
		m_pScenes[sceneID]->SetSceneID(sceneID);
		m_pScenes[sceneID]->SetSceneName(sceneName);
	}
	// シーンを変更リクエスト
	void ChangeSceneRequest(const SceneID& sceneID) noexcept { m_pNextScene = m_pScenes[sceneID]; }

	FVector<ScenePrefab*>& GetScenes() noexcept { return m_pScenes; }
	ECSManager* GetECSManager() const noexcept { return m_pECSManager.get(); }
	ObjectContainer* GetObjectContainer() const noexcept{ return m_pObjectContainer.get(); }
private:
	// シーンを変更
	void ChangeScene();
	// GameObjectを作成
	void AddGameObject();
	// TransformComponentを追加
	void AddTransformComponent(const uint32_t& entity);
	// MeshComponentを追加
	void AddMeshComponent(const uint32_t& entity);
	// CameraComponentを追加
	void AddCameraComponent(const uint32_t& entity);

	// 現在のシーン
	BaseScene* m_pCurrentScene = nullptr;
	// 次のシーン
	BaseScene* m_pNextScene = nullptr;
	// シーンコンテナ（フリーリスト付き）
	FVector<ScenePrefab*> m_pScenes;
	// ECSマネージャ
	std::unique_ptr<ECSManager> m_pECSManager = nullptr;
	// オブジェクトコンテナ
	std::unique_ptr<ObjectContainer> m_pObjectContainer = nullptr;
};

