#include "pch.h"
#include "SceneManager.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "GameCore/GameCore.h"
#include "EngineCommand/EngineCommands.h"

void ScenePrefab::Start()
{
	// シーンの初期化処理
	// シーンに必要なオブジェクトを追加する処理
	ObjectContainer* objectContainer = m_SceneManager->m_pGameCore->GetObjectContainer();
	ECSManager* ecs = m_SceneManager->m_pGameCore->GetECSManager();
	for (const auto& objData : m_GameObjectData)
	{
		// オブジェクト名とタイプを取得
		std::wstring name = objData.m_Name;
		ObjectType type = objData.m_Type;
		// Entityを生成
		Entity entity = ecs->GenerateEntity();
		// GameObjectを追加
		ObjectID objectID = objectContainer->AddGameObject(entity, name, type);
		GameObject& gameObj = objectContainer->GetGameObjectByName(name);
		gameObj.SetCurrentSceneName(m_SceneName);
		// SceneUseListに登録
		AddUseObject(gameObj.GetID().value());
		// コンポーネントの追加
		// TransformComponentの追加
		if (objData.m_Transform.has_value())
		{
			TransformComponent* transform = ecs->AddComponent<TransformComponent>(entity);
			*transform = objData.m_Transform.value();
			transform->mapID = m_SceneManager->m_pResourceManager->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
		}
		// CameraComponentの追加
		if (objData.m_Camera.has_value())
		{
			CameraComponent* camera = ecs->AddComponent<CameraComponent>(entity);
			*camera = objData.m_Camera.value();
			camera->bufferIndex = m_SceneManager->m_pResourceManager->CreateConstantBuffer<BUFFER_DATA_VIEWPROJECTION>();
		}
		// MeshFilterComponentの追加
		if (objData.m_MeshFilter.has_value())
		{
			MeshFilterComponent* mesh = ecs->AddComponent<MeshFilterComponent>(entity);
			*mesh = objData.m_MeshFilter.value();
			mesh->modelID = m_SceneManager->m_pResourceManager->GetModelManager()->GetModelDataIndex(mesh->modelName);
			// モデルのUseListに登録
			TransformComponent* transform = ecs->GetComponent<TransformComponent>(entity);
			m_SceneManager->m_pResourceManager->GetModelManager()->RegisterModelUseList(mesh->modelID.value(), transform->mapID.value());

		}
		// MeshRendererComponentの追加
		if (objData.m_MeshRenderer.has_value())
		{
			MeshRendererComponent* renderer = ecs->AddComponent<MeshRendererComponent>(entity);
			*renderer = objData.m_MeshRenderer.value();
		}
		// MaterialComponentの追加
		if (objData.m_Material.has_value())
		{
			MaterialComponent* material = ecs->AddComponent<MaterialComponent>(entity);
			*material = objData.m_Material.value();
			material->mapID = m_SceneManager->m_pResourceManager->GetIntegrationData(IntegrationDataType::Material)->GetMapID();
			TransformComponent* transform = ecs->GetComponent<TransformComponent>(entity);
			transform->materialID = material->mapID;
		}
		// ScriptComponentの追加
		if (objData.m_Script.has_value())
		{
			ScriptComponent* script = ecs->AddComponent<ScriptComponent>(entity);
			*script = objData.m_Script.value();
			script->objectID = objectID;
		}
		// LineRendererComponentの追加
		for (const auto& line : objData.m_LineRenderer)
		{
			LineRendererComponent* lineRenderer = ecs->AddComponent<LineRendererComponent>(entity);
			*lineRenderer = line;
			lineRenderer->mapID = m_SceneManager->m_pResourceManager->GetIntegrationData(IntegrationDataType::Line)->GetMapID();
		}
		// Rigidbody2DComponentの追加
		if (objData.m_Rigidbody2D.has_value())
		{
			Rigidbody2DComponent* rb = ecs->AddComponent<Rigidbody2DComponent>(entity);
			*rb = objData.m_Rigidbody2D.value();
			rb->selfObjectID = objectID;
		}
		// BoxCollider2DComponentの追加
		if (objData.m_BoxCollider2D.has_value())
		{
			BoxCollider2DComponent* box = ecs->AddComponent<BoxCollider2DComponent>(entity);
			*box = objData.m_BoxCollider2D.value();
		}
		// EmitterComponentの追加
		if (objData.m_Emitter.has_value())
		{
			EmitterComponent* emitter = ecs->AddComponent<EmitterComponent>(entity);
			*emitter = objData.m_Emitter.value();
			emitter->bufferIndex = m_SceneManager->m_pResourceManager->CreateStructuredBuffer<BUFFER_DATA_EMITTER>(1);
		}
		// ParticleComponentの追加
		if (objData.m_Particle.has_value())
		{
			ParticleComponent* particle = ecs->AddComponent<ParticleComponent>(entity);
			*particle = objData.m_Particle.value();
			// パーティクル
			particle->bufferIndex = m_SceneManager->m_pResourceManager->CreateRWStructuredBuffer<BUFFER_DATA_PARTICLE>(particle->count);
			// PerFrame
			particle->perFrameBufferIndex = m_SceneManager->m_pResourceManager->CreateConstantBuffer<BUFFER_DATA_PARTICLE_PERFRAME>();
			// FreeListIndex
			particle->freeListIndexBufferIndex = m_SceneManager->m_pResourceManager->CreateRWStructuredBuffer<int32_t>(1);
			// FreeList
			particle->freeListBufferIndex = m_SceneManager->m_pResourceManager->CreateRWStructuredBuffer<uint32_t>(particle->count, true);
		}
		// UISpriteComponentの追加
		if (objData.m_UISprite.has_value())
		{
			UISpriteComponent* uiSprite = ecs->AddComponent<UISpriteComponent>(entity);
			*uiSprite = objData.m_UISprite.value();
			uiSprite->mapID = m_SceneManager->m_pResourceManager->GetIntegrationData(IntegrationDataType::UISprite)->GetMapID();
			m_SceneManager->m_pResourceManager->GetUIContainer()->AddUI(uiSprite->mapID.value());
		}
		// LightComponentの追加
		if (objData.m_Light.has_value())
		{
			LightComponent* light = ecs->AddComponent<LightComponent>(entity);
			*light = objData.m_Light.value();
			light->mapID = m_SceneManager->m_pResourceManager->GetLightIndex();
		}
		// AudioComponentの追加
		if (objData.m_Audio.has_value())
		{
			AudioComponent* audio = ecs->AddComponent<AudioComponent>(entity);
			*audio = objData.m_Audio.value();
			/*if (!audio->audioName.empty())
			{
				audio->audioID = m_SceneManager->m_pResourceManager->GetAudioManager()->GetSoundDataIndex(audio->audioName);
			}*/
		}
		// AnimationComponentの追加
		if(objData.m_Animation.has_value())
		{
			AnimationComponent* animation = ecs->AddComponent<AnimationComponent>(entity);
			*animation = objData.m_Animation.value();
			ModelData* modelData = m_SceneManager->m_pResourceManager->GetModelManager()->GetModelData(animation->modelName);
			if (modelData)
			{
				animation->skeleton = modelData->skeleton;
				animation->skinCluster = modelData->skinCluster;
				animation->boneOffsetID = modelData->nextBoneOffsetIndex;
				modelData->nextBoneOffsetIndex++;
			}
		}
	}
	// MainCameraの設定
	GameObject& mainCamera = objectContainer->GetGameObjectByName(m_StartCameraName);
	if (mainCamera.GetType() == ObjectType::Camera)
	{
		SetMainCameraID(mainCamera.GetID().value());
	}
	// GameObjectDataをクリア
	m_GameObjectData.clear();
}

void ScenePrefab::Update()
{
}

void ScenePrefab::Finalize()
{
	// シーンの終了処理
	// シーンに追加したオブジェクトを全て削除する
	std::vector<ObjectID> useObjects = m_UseObjects;
	for (const auto& objectID : useObjects)
	{
		// GameObjectを取得
		GameObject& object = m_SceneManager->m_pGameCore->GetObjectContainer()->GetGameObject(objectID);
		GameObjectData objectCopy = object;
		std::unique_ptr<DeleteObjectCommand> deleteCommand = std::make_unique<DeleteObjectCommand>(objectID);
		if (!deleteCommand->Execute(m_SceneManager->m_pGameCore->GetEngineCommand()))
		{
			Cho::Log::Write(Cho::LogLevel::Assert, "DeleteObjectCommand failed");
		}
		AddGameObjectData(objectCopy);
	}
	m_UseObjects.clear();
	std::vector<ObjectID> clonedObjects = m_ClonedObjects;
	for (const auto& objectID : clonedObjects)
	{
		// GameObjectを取得
		//GameObject& object = m_SceneManager->m_pGameCore->GetObjectContainer()->GetGameObject(objectID);
		std::unique_ptr<DeleteObjectCommand> deleteCommand = std::make_unique<DeleteObjectCommand>(objectID);
		deleteCommand->Execute(m_SceneManager->m_pGameCore->GetEngineCommand());
	}
	m_ClonedObjects.clear();
}

void SceneManager::Update()
{
	//// 次のシーンがある場合
	//if (m_pNextScene) { ChangeScene(); }
	//// 現在のシーンがない場合
	//if (!m_pCurrentScene){ return; }
	//m_pCurrentScene->Update();
	// メインシーンが存在する場合は終了処理を行う
	if (!m_NextSceneName.empty())
	{
		if (!m_SceneNameToID.contains(m_NextSceneName))
		{
			return;
		}
		// 既に有効ならなにもしない
		if (m_SceneNameToScene.contains(m_NextSceneName))
		{
			return;
		}
		if (m_MainSceneID.has_value())
		{
			ScenePrefab* mainScene = m_pScenes[m_MainSceneID.value()].get();
			const std::wstring mainSceneName = mainScene->GetSceneName();
			// シーンの終了処理を行う
			m_pGameCore->SceneFinelize(mainScene);
			mainScene->Finalize();
			// 有効リストから削除
			m_SceneNameToScene.erase(mainSceneName);
		}
		// メインシーンのIDを更新
		m_MainSceneID = m_SceneNameToID[m_NextSceneName];
		// メインシーンを有効リストに追加
		ScenePrefab* newMainScene = m_pScenes[m_MainSceneID.value()].get();
		if (!newMainScene) { return; }
		m_SceneNameToScene[m_NextSceneName] = newMainScene;
		// メインシーンの初期化
		newMainScene->Start();
		m_pGameCore->AddGameLoadSceneID(newMainScene->GetSceneID());
		m_NextSceneName.clear();
	}

	for (const auto& loadSceneID : m_LoadedScenesIDs)
	{
		//ScenePrefab* scene = m_pScenes[loadSceneID].get();
		// 有効リストに追加
		SceneID id = loadSceneID;
		ScenePrefab* newScene = m_pScenes[id].get();
		if (!newScene) { return; }
		m_SceneNameToScene[newScene->GetSceneName()] = newScene;
		// 初期化
		newScene->Start();
		m_pGameCore->AddGameLoadSceneID(newScene->GetSceneID());
	}

	m_LoadedScenesIDs.clear();

	for (const auto& unLoadSceneID : m_UnloadedScenesIDs)
	{
		ScenePrefab* scene = m_pScenes[unLoadSceneID].get();
		// 有効リストに存在しない場合は何もしない
		if (!m_SceneNameToScene.contains(scene->GetSceneName()))
		{
			continue;
		}
		if (!scene) { return; }
		// シーンの終了処理を行う
		m_pGameCore->SceneFinelize(scene);
		scene->Finalize();
		// 有効リストから削除
		m_SceneNameToScene.erase(scene->GetSceneName());
	}

	m_UnloadedScenesIDs.clear();
}

// シーンを追加
void SceneManager::AddScene(const ScenePrefab& newScene)
{
	// 同じ名前のシーンがある場合は追加しない
	std::wstring sceneName = newScene.GetSceneName();
	if (m_SceneNameToID.contains(sceneName)) { return; }
	// シーンの名前とIDを紐づけて追加
	SceneID sceneID = static_cast<SceneID>(m_pScenes.push_back(std::make_unique<ScenePrefab>(std::move(newScene))));
	m_pScenes[sceneID]->SetSceneID(sceneID);
	// 補助コンテナに追加
	m_SceneNameToID[sceneName] = sceneID;
}

void SceneManager::LoadScene(const std::wstring& sceneName)
{
	//　シーン名が存在しない場合は何もしない
	if (!m_SceneNameToID.contains(sceneName))
	{
		return;
	}
	// 既に有効ならなにもしない
	if (m_SceneNameToScene.contains(sceneName))
	{
		return;
	}
	m_LoadedScenesIDs.push_back(m_SceneNameToID[sceneName]);
}

void SceneManager::UnLoadScene(const std::wstring& sceneName)
{
	//　シーン名が存在しない場合は何もしない
	if (!m_SceneNameToID.contains(sceneName))
	{
		return;
	}
	// 有効リストに存在しない場合は何もしない
	if (!m_SceneNameToScene.contains(sceneName))
	{
		return;
	}
	m_UnloadedScenesIDs.push_back(m_SceneNameToID[sceneName]);
}

void SceneManager::ChangeMainScene(const std::wstring& sceneName)
{
	// メインシーンの終了
	m_NextSceneName = sceneName;
}

//void SceneManager::ChangeScene()
//{
//	if (!m_pCurrentScene)// 現在のシーンがない場合
//	{
//		// 次のシーンを現在のシーンにセット
//		m_pCurrentScene = m_pNextScene;
//		m_pNextScene = nullptr;
//		m_pCurrentScene->Start();
//	} else
//	{
//		// 現在のシーンを終了
//		m_pCurrentScene->Finalize();
//		m_pCurrentScene = nullptr;
//		m_pCurrentScene = m_pNextScene;
//		m_pNextScene = nullptr;
//		m_pCurrentScene->Start();
//	}
//}
