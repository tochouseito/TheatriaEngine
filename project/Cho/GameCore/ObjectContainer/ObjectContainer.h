#pragma once
#include "Cho/GameCore/GameObject/GameObject.h"
#include "Cho/Core/Utility/FVector.h"
#include <unordered_map>
class ObjectContainer
{
public:
	// Constructor
	ObjectContainer()
	{
	}
	// Destructor
	~ObjectContainer()
	{
	}

	// ゲームオブジェクトを追加
	ObjectID AddGameObject(const Entity& entity,const std::wstring& name,const ObjectType& type)
	{ 
		ObjectID id = static_cast<ObjectID>(m_GameObjects.push_back(GameObject(entity, name, type)));
		m_NameToObjectID[name] = id;
		return id;
	}
	// プレハブを追加
	PrefabID AddPrefab(const Entity& entity, const std::wstring& name, const ObjectType& type)
	{ 
		PrefabID id = static_cast<PrefabID>(m_Prefabs.push_back(Prefab(entity, name, type)));
		m_NameToPrefabID[name] = id;
		return id;
	}
	// ゲームオブジェクトを取得
	GameObject* GetGameObject(const ObjectID& index) { return &m_GameObjects[index]; }
	// プレハブを取得
	Prefab* GetPrefab(const ObjectID& index) { return &m_Prefabs[index]; }
	// ゲームオブジェクトコンテナを取得
	FVector<GameObject>& GetGameObjects() { return m_GameObjects; }
	// プレハブコンテナを取得
	FVector<Prefab>& GetPrefabs() { return m_Prefabs; }
	// 名前検索用補助コンテナを取得
	std::unordered_map<std::wstring, ObjectID>& GetNameToObjectID() { return m_NameToObjectID; }
	std::unordered_map<std::wstring, PrefabID>& GetNameToPrefabID() { return m_NameToPrefabID; }
	// 名前でゲームオブジェクトを取得
	GameObject* GetGameObjectByName(const std::wstring& name)
	{
		if (m_NameToObjectID.contains(name))
		{
			return &m_GameObjects[m_NameToObjectID[name]];
		}
		return nullptr;
	}
	// 名前でプレハブを取得
	Prefab* GetPrefabByName(const std::wstring& name)
	{
		if (m_NameToPrefabID.contains(name))
		{
			return &m_Prefabs[m_NameToPrefabID[name]];
		}
		return nullptr;
	}
private:
	// ゲームオブジェクトコンテナ
	FVector<GameObject> m_GameObjects;
	// プレファブコンテナ
	FVector<Prefab> m_Prefabs;
	// 名前検索用補助コンテナ
	std::unordered_map<std::wstring, ObjectID> m_NameToObjectID;
	std::unordered_map<std::wstring, PrefabID> m_NameToPrefabID;
};

