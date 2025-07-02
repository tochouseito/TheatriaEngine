#include "pch.h"
#include "SceneManager.h"
#include "GameCore/GameWorld/GameWorld.h"

// 読み込まれているシーンを破棄して指定したシーンをロード
GameSceneInstance* SceneManager::LoadScene(const std::wstring& sceneName)
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
	// シーンインスタンスを返す
	return m_pSceneInstances.back().get();
}
