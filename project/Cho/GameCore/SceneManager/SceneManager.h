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
	virtual inline void AddUseObject(const ObjectID& objectID) { useObjects.push_back(objectID); }
	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void Finalize() = 0;

	inline void SetSceneID(const SceneID sceneID) { m_SceneID = sceneID; }
protected:
	SceneID m_SceneID = 0;
	std::wstring m_SceneName = L"";
	SceneManager* m_SceneManager = nullptr;
	std::vector<ObjectID> useObjects;
	ObjectID m_MainCameraID = UINT32_MAX;
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
		AddScene(L"MainScene");
		ChangeSceneRequest(m_SceneNameToID[L"MainScene"]);
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
		// 同じ名前のシーンがある場合は追加しない
		if (m_SceneNameToID.contains(sceneName)) { return; }
		// シーンの名前とIDを紐づけて追加
		std::unique_ptr<ScenePrefab> pScene = std::make_unique<ScenePrefab>(this);
		SceneID sceneID = static_cast<SceneID>(m_pScenes.push_back(std::move(pScene)));
		m_pScenes[sceneID]->SetSceneID(sceneID);
		m_pScenes[sceneID]->SetSceneName(sceneName);
		// 補助コンテナに追加
		m_SceneNameToID[sceneName] = sceneID;
	}
	// シーンを変更リクエスト
	void ChangeSceneRequest(const SceneID& sceneID) noexcept { m_pNextScene = m_pScenes[sceneID].get(); }

	FVector<std::unique_ptr<ScenePrefab>>& GetScenes() noexcept { return m_pScenes; }
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
	FVector<std::unique_ptr<ScenePrefab>> m_pScenes;
	// 名前検索用補助コンテナ
	std::unordered_map<std::wstring, SceneID> m_SceneNameToID;
	// ECSマネージャ
	std::unique_ptr<ECSManager> m_pECSManager = nullptr;
	// オブジェクトコンテナ
	std::unique_ptr<ObjectContainer> m_pObjectContainer = nullptr;
};

