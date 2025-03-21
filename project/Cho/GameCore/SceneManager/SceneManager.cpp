#include "pch.h"
#include "SceneManager.h"
#include "Cho/Core/Log/Log.h"

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

void SceneManager::ChangeScene()
{
	if (!m_pCurrentScene)// 現在のシーンがない場合
	{
		ChoLog("Current Scene is nullptr");
		return;
	}
	m_pCurrentScene->Finalize();
	delete m_pCurrentScene;
	m_pCurrentScene = m_pNextScene;
	m_pCurrentScene->Start();
}

void SceneManager::AddGameObject()
{
	Entity entity = m_pECSManager->GenerateEntity();
	m_pObjectContainer->AddGameObject(entity);
}
