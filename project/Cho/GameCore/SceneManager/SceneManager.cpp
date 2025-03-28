#include "pch.h"
#include "SceneManager.h"
#include "Cho/Core/Log/Log.h"
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

void SceneManager::AddMeshComponent(const uint32_t& entity)
{
	m_pECSManager->AddComponent<MeshComponent>(entity);
}

void SceneManager::AddCameraComponent(const uint32_t& entity)
{
	CameraComponent* camera = m_pECSManager->AddComponent<CameraComponent>(entity);
	if (!camera)
	{
		ChoAssertLog("CameraComponent is nullptr", false, __FILE__, __LINE__);
	}
	// Resourceの生成
	// 生成するResourceの設定
	BUFFER_CONSTANT_DESC desc = {};
}

uint32_t SceneManager::SetMainCamera(const uint32_t& setCameraID)
{
	if (!m_pCurrentScene)
	{
		ChoLog("Current Scene is nullptr");
		return UINT32_MAX;
	}
	uint32_t preCameraID = m_pCurrentScene->GetMainCameraID();
	m_pCurrentScene->SetMainCameraID(setCameraID);
	return preCameraID;
}

void SceneManager::AddGameObject()
{
	if (!m_pCurrentScene)
	{
		ChoLog("Current Scene is nullptr");
		return;
	}
	Entity entity = m_pECSManager->GenerateEntity();
	m_pCurrentScene->AddUseObject(m_pObjectContainer->AddGameObject(entity));
}

void SceneManager::AddTransformComponent(const uint32_t& entity)
{
	m_pECSManager->AddComponent<TransformComponent>(entity);
	// Resourceの生成
	
}
