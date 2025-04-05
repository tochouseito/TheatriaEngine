#include "pch.h"
#include "SceneManager.h"
#include "Cho/Resources/ResourceManager/ResourceManager.h"

void ScenePrefab::Start()
{
}

void ScenePrefab::Update()
{
}

void ScenePrefab::Finalize()
{
}

void SceneManager::Update()
{
	if (m_pNextScene)// 次のシーンがある場合
	{
		ChangeScene();
	}
	if (!m_pCurrentScene)// 現在のシーンがない場合
	{
		return;
	}
	m_pCurrentScene->Update();
}

// シーンを追加
void SceneManager::AddScene(const std::wstring& sceneName)
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

void SceneManager::CreateSystem() noexcept
{
	
}

void SceneManager::ChangeScene()
{
	if (!m_pCurrentScene)// 現在のシーンがない場合
	{
		// 次のシーンを現在のシーンにセット
		m_pCurrentScene = m_pNextScene;
		m_pNextScene = nullptr;
		m_pCurrentScene->Start();
	} else
	{
		// 現在のシーンを終了
		m_pCurrentScene->Finalize();
		m_pCurrentScene = nullptr;
		m_pCurrentScene = m_pNextScene;
		m_pNextScene = nullptr;
		m_pCurrentScene->Start();
	}
}
