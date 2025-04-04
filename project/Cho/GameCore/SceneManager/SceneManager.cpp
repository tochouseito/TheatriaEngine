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

void SceneManager::AddMeshComponent(const uint32_t& entity)
{
	MeshComponent* mesh = m_pECSManager->AddComponent<MeshComponent>(entity);
	mesh->modelID = 0;// 一旦0にしておく
	// Transformがあるとき、連携する
	TransformComponent* transform = m_pECSManager->GetComponent<TransformComponent>(entity);
	if (transform)
	{
		// ModelのUseListに登録
		m_pIntegrationBuffer->AddUseIndex(mesh->modelID, transform->mapID);
	} else
	{
		ChoAssertLog("TransformComponent is nullptr", false, __FILE__, __LINE__);
	}
}

void SceneManager::AddRenderComponent(const uint32_t& entity)
{
	RenderComponent* render = m_pECSManager->AddComponent<RenderComponent>(entity);
	render->visible = true;
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
	desc.sizeInBytes = sizeof(BUFFER_DATA_VIEWPROJECTION);
	desc.state = D3D12_RESOURCE_STATE_GENERIC_READ;
	camera->bufferIndex= m_pResourceManager->CreateConstantBuffer(desc);
	camera->mappedIndex = m_pIntegrationBuffer->GetMappedVPID(camera->bufferIndex);
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
	TransformComponent* transform = m_pECSManager->AddComponent<TransformComponent>(entity);
	// mapIDを取得
	transform->mapID = m_pIntegrationBuffer->GetNextMapID();
}
