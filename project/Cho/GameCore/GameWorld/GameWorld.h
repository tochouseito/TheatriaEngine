#pragma once
#include "GameCore/GameObject/GameObject.h"
#include "Core/Utility/FVector.h"

class ECSManager;
class GameWorld
{
public:
	// Constructor
	GameWorld(ECSManager* ecs):m_pECSManager(ecs)
	{
	}
	// Destructor
	~GameWorld()
	{
	}
	void Initialize()
	{
		
	}
	void Update()
	{

	}
	// オブジェクトを取得
	GameObject* GetGameObject(const std::wstring& name)
	{
		if (!m_ObjectHandleMap.contains(name))
		{
			return nullptr;
		}
		ObjectHandle handle = m_ObjectHandleMap[name];
		return m_pGameObjects[handle.sceneID][handle.objectID][handle.cloneID].get();
	}
	GameObject* GetGameObject(const Entity& e)
	{
		if (!m_ObjectHandleMapFromEntity.contains(e))
		{
			return nullptr;
		}
		ObjectHandle handle = m_ObjectHandleMapFromEntity[e];
		return m_pGameObjects[handle.sceneID][handle.objectID][handle.cloneID].get();
	}
	GameObject* GetGameObject(const ObjectHandle& handle)
	{
		if (!m_pGameObjects.isValid(handle.sceneID) ||
			!m_pGameObjects[handle.sceneID].isValid(handle.objectID)||
			!m_pGameObjects[handle.sceneID][handle.objectID].isValid(handle.cloneID))
		{
			return nullptr;
		}
		return m_pGameObjects[handle.sceneID][handle.objectID][handle.cloneID].get();
	}
	// メインカメラを取得
	GameObject* GetMainCamera()
	{
		return m_pMainCamera;
	}
	// メインカメラを空にする
	void ClearMainCamera()
	{
		m_pMainCamera = nullptr;
	}
private:
	// オブジェクトを作成
	ObjectHandle CreateGameObject(const std::wstring& name, ObjectType type)
	{
		// handleを作成
		ObjectHandle handle;
		handle.sceneID = 0; // 0番のSceneに追加
		// Entityを取得
		Entity entity = m_pECSManager->GenerateEntity();
		handle.entity = entity;
		// オブジェクトIDを取得
		uint32_t objectID = static_cast<uint32_t>(m_pGameObjects[0].push_back(FVector<std::unique_ptr<GameObject>>()));
		handle.objectID = objectID;
		// GameObjectを作成
		m_pGameObjects[0][objectID].push_back(std::make_unique<GameObject>(handle, name, type));
		// 辞書に登録
		m_ObjectHandleMap[name] = handle;
		m_ObjectHandleMapFromEntity[entity] = handle;
		return handle;
	}
	// クローンを追加
	void AddGameObjectClone(const GameObject& src)
	{

	}
	// オブジェクトを削除
	void RemoveGameObject(const Entity& e)
	{

	}
	// メインカメラを設定
	void SetMainCamera(GameObject* pCamera)
	{
		m_pMainCamera = pCamera;
	}

	ECSManager* m_pECSManager = nullptr;	
	// GameObjectコンテナ
	// [SceneID][ObjectID][CloneID] = GameObject
	FVector<FVector<FVector<std::unique_ptr<GameObject>>>> m_pGameObjects;
	// 名前検索辞書
	std::unordered_map<std::wstring, ObjectHandle> m_ObjectHandleMap;
	// Entityから逆引きする辞書
	std::unordered_map<Entity, ObjectHandle> m_ObjectHandleMapFromEntity;
	// メインカメラ
	GameObject* m_pMainCamera = nullptr;
};

