#include "pch.h"
#include "GameWorld.h"
#include "GameCore/GameScene/GameScene.h"
#include "Core/ChoLog/ChoLog.h"

inline void GameWorld::Initialize()
{

}

inline void GameWorld::Update()
{

}

// オブジェクトを取得
inline GameObject* GameWorld::GetGameObject(const std::wstring& name)
{
	if (!m_ObjectHandleMap.contains(name))
	{
		return nullptr;
	}
	ObjectHandle handle = m_ObjectHandleMap[name];
	return m_pGameObjects[handle.sceneID][handle.objectID][handle.cloneID].get();
}

inline GameObject* GameWorld::GetGameObject(const Entity& e)
{
	if (!m_ObjectHandleMapFromEntity.contains(e))
	{
		return nullptr;
	}
	ObjectHandle handle = m_ObjectHandleMapFromEntity[e];
	return m_pGameObjects[handle.sceneID][handle.objectID][handle.cloneID].get();
}

inline GameObject* GameWorld::GetGameObject(const ObjectHandle& handle)
{
	if (!m_pGameObjects.isValid(handle.sceneID) ||
		!m_pGameObjects[handle.sceneID].isValid(handle.objectID) ||
		!m_pGameObjects[handle.sceneID][handle.objectID].isValid(handle.cloneID))
	{
		return nullptr;
	}
	return m_pGameObjects[handle.sceneID][handle.objectID][handle.cloneID].get();
}

// オブジェクトを作成
inline ObjectHandle GameWorld::CreateGameObject(const std::wstring& name, ObjectType type)
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

// シーンデータからオブジェクトを作成
inline SceneID GameWorld::AddGameObjectFromScene(const GameScene& scene)
{
	// シーンIDを取得
	SceneID sceneID = static_cast<SceneID>(m_pGameObjects.push_back(FVector<FVector<std::unique_ptr<GameObject>>>()));
	// シーンのオブジェクトを作成
	FVector<CPrefab> prefabs = scene.GetPrefabs();
	for (auto& prefab : prefabs)
	{
		// handleを作成
		ObjectHandle handle;
		handle.sceneID = sceneID; // シーンIDを設定
		// PrefabからEntityを生成
		Entity entity = prefab.Instantiate(*m_pECSManager);
		handle.entity = entity;
		// オブジェクトIDを取得
		uint32_t objectID = static_cast<uint32_t>(m_pGameObjects[sceneID].push_back(FVector<std::unique_ptr<GameObject>>()));
		handle.objectID = objectID;
		// GameObjectを作成
		if (prefab.GetName().empty())
		{
			Cho::Log::Write(Cho::LogLevel::Assert, "Prefab name is empty. Please set a valid name for the prefab.");
			return sceneID;
		}
		if(prefab.GetType() == ObjectType::None)
		{
			Cho::Log::Write(Cho::LogLevel::Assert, "Prefab type is None. Please set a valid type for the prefab.");
			return sceneID;
		}
		m_pGameObjects[sceneID][objectID].push_back(std::make_unique<GameObject>(handle, prefab.GetName(), prefab.GetType()));
		// 辞書に登録
		m_ObjectHandleMap[prefab.GetName()] = handle;
		m_ObjectHandleMapFromEntity[entity] = handle;
	}
	return sceneID;
}
