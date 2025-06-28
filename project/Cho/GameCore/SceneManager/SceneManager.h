#pragma once
#include "GameCore/GameScene/GameScene.h"

enum class LoadSceneMode
{
	Single,  // 単一ロード
	Additive,// 追加ロード
};

// 前方宣言
class GameWorld;

class SceneManager
{
public:
	// Constructor
	SceneManager(GameWorld* gameWorld) : m_pGameWorld(gameWorld)
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
		LoadScene(L"MainScene");
	}

	// 読み込まれているシーンを破棄して指定したシーンをロード
	GameSceneInstance* LoadScene(const std::wstring& sceneName)
	{
		// コンテナ
		if (!m_SceneNameToID.contains(sceneName))
		{
			return nullptr; // シーンが存在しない場合はnullptrを返す
		}
		// 読み込まれているシーンをすべて破棄
		m_pGameWorld->ClearAllScenes(); // ゲームワールドのシーンをクリア
		// シーンインスタンスを破棄
		m_pSceneInstances.clear();
		// ロード
		SceneID id = m_pGameWorld->AddGameObjectFromScene(m_Scenes[m_SceneNameToID[sceneName]]);
		// シーンインスタンスを作成
		m_pSceneInstances.push_back(std::make_unique<GameSceneInstance>(this, id));
	}
	// 非同期でシーンをロード
	GameSceneInstance* LoadSceneAsync(const std::wstring& sceneName, const LoadSceneMode& mode)
	{

	}
	// シーンをアンロード
	void UnLoadScene(GameSceneInstance* pSceneInstance)
	{

	}
	// シーンを追加
	void AddScene(GameScene scene)
	{
		if (m_SceneNameToID.contains(scene.GetName()))
		{
			return; // 既に存在する場合は何もしない
		}
		SceneID id = static_cast<SceneID>(m_Scenes.push_back(scene));
		m_SceneNameToID[scene.GetName()] = id; // シーン名とIDを紐付け
	}
	// コンテナからシーンを取得
	GameScene* GetScene(const SceneID& sceneID) noexcept
	{
		if (m_Scenes.isValid(sceneID))
		{
			return &m_Scenes[sceneID];
		}
		return nullptr;
	}
	GameScene* GetScene(const std::wstring& sceneName) noexcept
	{
		if (m_SceneNameToID.contains(sceneName))
		{
			return &m_Scenes[m_SceneNameToID.at(sceneName)];
		}
		return nullptr;
	}
	// シーンコンテナを取得
	FVector<GameScene>& GetScenes() noexcept { return m_Scenes; }
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
private:
	GameWorld* m_pGameWorld = nullptr; // ゲームワールドへのポインタ

	// シーンコンテナ（フリーリスト付き）
	FVector<GameScene> m_Scenes;
	// 名前検索用補助コンテナ
	std::unordered_map<std::wstring, SceneID> m_SceneNameToID;
	// シーンインスタンスコンテナ
	FVector<std::unique_ptr<GameSceneInstance>> m_pSceneInstances;
};

