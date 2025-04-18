#pragma once
#include "GameCore/ObjectContainer/ObjectContainer.h"
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
	virtual inline std::vector<ObjectID>& GetUseObjects() noexcept { return useObjects; }
	virtual inline void AddUseObject(const ObjectID& objectID) { useObjects.push_back(objectID); }
	virtual inline void RemoveUseObject(const ObjectID& objectID) { useObjects.erase(std::remove(useObjects.begin(), useObjects.end(), objectID), useObjects.end()); }
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
		CreateSystem();
	}
	// Destructor
	~SceneManager()
	{
	}

	// デフォルトのシーンを作成
	void CreateDefaultScene()
	{
		AddScene(L"MainScene");
		ChangeSceneRequest(m_SceneNameToID[L"MainScene"]);
	}

	// シーンを更新
	void Update();
	// シーンを追加
	void AddScene(const std::wstring& sceneName);
	// シーンを変更リクエスト
	void ChangeSceneRequest(const SceneID& sceneID) noexcept { m_pNextScene = m_pScenes[sceneID].get(); }

	// System
	void CreateSystem() noexcept;

	ScenePrefab* GetScene(const SceneID& index) const noexcept
	{
		if (m_pScenes.isValid(index))
		{
			return m_pScenes[index].get();
		}
		return nullptr;
	}
	FVector<std::unique_ptr<ScenePrefab>>& GetScenes() noexcept { return m_pScenes; }
	// シーン名からシーンIDを取得
	SceneID GetSceneID(const std::wstring& sceneName) const noexcept
	{
		if (m_SceneNameToID.contains(sceneName))
		{
			return m_SceneNameToID.at(sceneName);
		}
		return 0;
	}
	// シーン名からシーンを取得
	ScenePrefab* GetSceneToName(const std::wstring& sceneName) const noexcept
	{
		if (m_SceneNameToID.contains(sceneName))
		{
			return m_pScenes[m_SceneNameToID.at(sceneName)].get();
		}
		return nullptr;
	}
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

