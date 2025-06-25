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
		GameObject* gameObj = objectContainer->GetGameObjectByName(name);
		gameObj->SetCurrentSceneName(m_SceneName);
		// SceneUseListに登録
		AddUseObject(gameObj->GetID().value());
		// コンポーネントの追加
		// TransformComponentの追加
		
	}
	// MainCameraの設定
	GameObject* mainCamera = objectContainer->GetGameObjectByName(m_StartCameraName);
	if (mainCamera && mainCamera->GetType() == ObjectType::Camera)
	{
		SetMainCameraID(mainCamera->GetID().value());
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
		GameObject* object = m_SceneManager->m_pGameCore->GetObjectContainer()->GetGameObject(objectID);
		GameObjectData objectCopy = *object;
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
