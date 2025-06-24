#pragma once
#include "GameCore/GameScene/GameScene.h"
class GameCore;

class ResourceManager;
class SceneManager
{
public:
	// Constructor
	SceneManager(GameCore* gameCore,ResourceManager* resourceManager):
		m_pGameCore(gameCore),m_pResourceManager(resourceManager)
	{
	}
	// Destructor
	~SceneManager()
	{
	}

	// デフォルトのシーンを作成
	void CreateDefaultScene()
	{
		GameScene scene(L"MainScene");
		AddScene(scene);
		ChangeMainScene(L"MainScene");
	}

	// シーンを更新
	void Update();
	// シーンを追加
	void AddScene(const GameScene& newScene);
	// シーンを変更リクエスト
	//void ChangeSceneRequest(const SceneID& sceneID) noexcept { m_pNextScene = m_pScenes[sceneID].get(); }
	// シーンを追加ロード
	void LoadScene(const std::wstring& sceneName);
	// シーンの解除
	void UnLoadScene(const std::wstring& sceneName);
	// メインシーンの変更
	void ChangeMainScene(const std::wstring& sceneName);
	// シーンを取得
	GameScene* GetGameScene(const SceneID& index) noexcept
	{
		if (m_Scenes.isValid(index))
		{
			return &m_Scenes[index];
		}
		return nullptr;
	}
	FVector<GameScene>& GetScenes() noexcept { return m_Scenes; }
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
	GameScene* GetSceneToName(const std::wstring& sceneName) noexcept
	{
		if (m_SceneNameToID.contains(sceneName))
		{
			return &m_Scenes[m_SceneNameToID.at(sceneName)];
		}
		return nullptr;
	}
	// メインシーンを取得
	GameScene* GetMainScene() noexcept
	{
		if (!m_MainSceneID.has_value())
		{
			return nullptr;
		}
		return &m_Scenes[m_MainSceneID.value()];
	}
	// シーンをクリア
	void ClearScenes() noexcept
	{
		for (auto& scene : m_SceneNameToScene)
		{
			scene.second->Finalize();
		}
		m_SceneNameToScene.clear();
		m_MainSceneID = std::nullopt;
	}
	void EditorReLoad(const SceneID& sceneID)
	{
		// 最初のシーンに戻す
		ClearScenes();
		std::wstring sceneName = m_pScenes[sceneID]->GetSceneName();
		ChangeMainScene(sceneName);
	}
private:
	// GameCore
	GameCore* m_pGameCore = nullptr;
	// ResourceManager
	ResourceManager* m_pResourceManager = nullptr;
	// メインシーンID
	std::optional<SceneID> m_MainSceneID = std::nullopt;
	// メインシーン名
	std::wstring m_MainSceneName = L"";
	// シーンコンテナ（フリーリスト付き）
	FVector<GameScene> m_Scenes;
	// 名前検索用補助コンテナ
	std::unordered_map<std::wstring, SceneID> m_SceneNameToID;
	// 読み込まれたScene
	std::vector<SceneID> m_LoadedScenesIDs;
	// 解除されたScene
	std::vector<SceneID> m_UnloadedScenesIDs;
};

