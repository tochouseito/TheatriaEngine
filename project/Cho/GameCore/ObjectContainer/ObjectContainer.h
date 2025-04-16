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
		m_GameObjects[id].SetID(id);
		return id;
	}
	// ゲームオブジェクトを削除
	void DeleteGameObject(const ObjectID& id)
	{
		if (!m_GameObjects.isValid(id))
		{
			return;
		}
		m_NameToObjectID.erase(m_GameObjects[id].GetName());
		m_GameObjects.erase(id);
	}
	// プレハブを追加
	PrefabID AddPrefab(const Entity& entity, const std::wstring& name, const ObjectType& type)
	{ 
		PrefabID id = static_cast<PrefabID>(m_Prefabs.push_back(Prefab(entity, name, type)));
		m_NameToPrefabID[name] = id;
		m_Prefabs[id].SetID(id);
		return id;
	}
	// プレハブを削除
	void DeletePrefab(const PrefabID& id)
	{
		if (!m_Prefabs.isValid(id))
		{
			return;
		}
		m_NameToPrefabID.erase(m_Prefabs[id].GetName());
		m_Prefabs.erase(id);
	}
	// ゲームオブジェクトを取得
	GameObject* GetGameObject(const ObjectID& index) {
		if (!m_GameObjects.isValid(index))
		{
			return nullptr;
		}
		return &m_GameObjects[index];
	}
	// プレハブを取得
	Prefab* GetPrefab(const ObjectID& index) {
		if (!m_Prefabs.isValid(index))
		{
			return nullptr;
		}
		return &m_Prefabs[index];
	}
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

