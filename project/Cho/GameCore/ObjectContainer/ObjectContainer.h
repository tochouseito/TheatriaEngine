#pragma once
#include "GameCore/GameObject/GameObject.h"
#include "Core/Utility/FVector.h"
#include <unordered_map>

class ECSManager;
class ResourceManager;
class InputManager;
class ObjectContainer
{
public:
	// Constructor
	ObjectContainer(ECSManager* ecs, ResourceManager* resourceManager, InputManager* input) :
		m_ECS(ecs), m_ResourceManager(resourceManager), m_InputManager(input)
	{
		m_DummyGameObject.SetInactive();
	}
	// Destructor
	~ObjectContainer()
	{
	}
	// ゲームオブジェクトを追加
	ObjectID AddGameObject(const Entity& entity,const std::wstring& name,const ObjectType& type)
	{
		ObjectID id = static_cast<ObjectID>(m_GameObjects.push_back(std::make_unique<GameObject>(this, m_InputManager, m_ResourceManager, m_ECS, entity, name, type)));
		//m_GameObjects[id].Initialize();
		m_NameToObjectID[name] = id;
		m_TypeToObjectIDs[type].push_back(id);
		m_GameObjects[id]->SetID(id);
		return id;
	}
	// ゲームオブジェクトを削除
	void DeleteGameObject(const ObjectID& id)
	{
		if (!m_GameObjects.isValid(id))
		{
			return;
		}
		m_NameToObjectID.erase(m_GameObjects[id]->GetName());
		ObjectType type = m_GameObjects[id]->GetType();
		if (m_TypeToObjectIDs.contains(type))
		{
			m_TypeToObjectIDs[type].erase(std::remove(m_TypeToObjectIDs[type].begin(), m_TypeToObjectIDs[type].end(), id), m_TypeToObjectIDs[type].end());
		}
		m_GameObjects[id]->SetInactive();
		m_GameObjects.erase(id);
	}
	// プレハブを追加
	PrefabID AddPrefab(const Entity& entity, const std::wstring& name, const ObjectType& type)
	{ 
		PrefabID id = static_cast<PrefabID>(m_Prefabs.push_back(Prefab(entity, name, type)));
		m_NameToPrefabID[name] = id;
		m_TypeToPrefabIDs[type].push_back(id);
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
		ObjectType type = m_Prefabs[id].GetType();
		if (m_TypeToPrefabIDs.contains(type))
		{
			m_TypeToPrefabIDs[type].erase(std::remove(m_TypeToPrefabIDs[type].begin(), m_TypeToPrefabIDs[type].end(), id), m_TypeToPrefabIDs[type].end());
		}
		m_Prefabs.erase(id);
	}
	// ゲームオブジェクトを取得
	/*GameObject* GetGameObject(const ObjectID& index) {
		if (!m_GameObjects.isValid(index))
		{
			return nullptr;
		}
		return &m_GameObjects[index];
	}*/
	GameObject& GetGameObject(const ObjectID& index)
	{
		if (!m_GameObjects.isValid(index))
		{
			return m_DummyGameObject;
		}
		return *m_GameObjects[index];
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
	FVector<std::unique_ptr<GameObject>>& GetGameObjects() { return m_GameObjects; }
	// プレハブコンテナを取得
	FVector<Prefab>& GetPrefabs() { return m_Prefabs; }
	// 名前検索用補助コンテナを取得
	std::unordered_map<std::wstring, ObjectID>& GetNameToObjectID() { return m_NameToObjectID; }
	std::unordered_map<std::wstring, PrefabID>& GetNameToPrefabID() { return m_NameToPrefabID; }
	// 名前でゲームオブジェクトを取得
	/*GameObject* GetGameObjectByName(const std::wstring& name)
	{
		if (m_NameToObjectID.contains(name))
		{
			return &m_GameObjects[m_NameToObjectID[name]];
		}
		return nullptr;
	}*/
	// 名前でゲームオブジェクトを取得
	GameObject& GetGameObjectByName(const std::wstring& name)
	{
		if (m_NameToObjectID.contains(name))
		{
			return *m_GameObjects[m_NameToObjectID[name]];
		}
		return m_DummyGameObject;
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
	// ダミーGameObjectを取得
	GameObject& GetDummyGameObject() { return m_DummyGameObject; }
	// GameObject全て初期化
	void InitializeAllGameObjects()
	{
		for (auto& obj : m_GameObjects.GetVector())
		{
			if (!obj->IsActive()) continue;
			obj->Initialize();
		}
	}
private:
	ECSManager* m_ECS = nullptr;	// ECSManager
	ResourceManager* m_ResourceManager = nullptr;	// ResourceManager
	InputManager* m_InputManager = nullptr;	// InputManager

	// ゲームオブジェクトコンテナ
	FVector<std::unique_ptr<GameObject>> m_GameObjects;
	// プレファブコンテナ
	FVector<Prefab> m_Prefabs;
	// 名前検索用補助コンテナ
	std::unordered_map<std::wstring, ObjectID> m_NameToObjectID;
	std::unordered_map<std::wstring, PrefabID> m_NameToPrefabID;
	// タイプ検索用補助コンテナ
	std::unordered_map<ObjectType, std::vector<ObjectID>> m_TypeToObjectIDs;
	std::unordered_map<ObjectType, std::vector<PrefabID>> m_TypeToPrefabIDs;

	// ダミーGameObject
	GameObject m_DummyGameObject;
};

