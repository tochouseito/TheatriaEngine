#pragma once
#include "GameCore/SystemManager/SystemManager.h"
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
	virtual inline void SetMainCameraID(const ObjectID& cameraID) { m_MainCameraID = cameraID; }
	virtual inline std::optional<ObjectID> GetMainCameraID() const noexcept { return m_MainCameraID; }
	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void Finalize() = 0;
	inline void SetSceneID(const SceneID sceneID) { m_SceneID = sceneID; }
protected:
	SceneID m_SceneID = 0;
	std::wstring m_SceneName = L"";
	SceneManager* m_SceneManager = nullptr;
	std::vector<ObjectID> useObjects;
	std::optional<ObjectID> m_MainCameraID = std::nullopt;
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

class ResourceManager;
class SceneManager
{
public:
	// Constructor
	SceneManager(ResourceManager* resourceManager):
		m_pResourceManager(resourceManager)
	{
		AddScene(L"MainScene");
		ChangeSceneRequest(m_SceneNameToID[L"MainScene"]);
		CreateSystem();
	}
	// Destructor
	~SceneManager()
	{
	}

	// シーンを更新
	void Update();
	// シーンを追加
	void AddScene(const std::wstring& sceneName);
	// シーンを変更リクエスト
	void ChangeSceneRequest(const SceneID& sceneID) noexcept { m_pNextScene = m_pScenes[sceneID].get(); }

	// System
	void CreateSystem() noexcept;

	FVector<std::unique_ptr<ScenePrefab>>& GetScenes() noexcept { return m_pScenes; }
	ScenePrefab* GetCurrentScene() const noexcept { return m_pCurrentScene; }
private:
	// シーンを変更
	void ChangeScene();

	// ResourceManager
	ResourceManager* m_pResourceManager = nullptr;
	// 現在のシーン
	ScenePrefab* m_pCurrentScene = nullptr;
	// 次のシーン
	ScenePrefab* m_pNextScene = nullptr;
	// シーンコンテナ（フリーリスト付き）
	FVector<std::unique_ptr<ScenePrefab>> m_pScenes;
	// 名前検索用補助コンテナ
	std::unordered_map<std::wstring, SceneID> m_SceneNameToID;
};

