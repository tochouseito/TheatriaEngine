#pragma once
#include "GameCore/GameObject/GameObject.h"
#include "GameCore/Prefab/Prefab.h"
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
		//m_DummyGameObject.SetInactive();
	}
	// Destructor
	~ObjectContainer()
	{
	}
	// ゲームオブジェクトを追加
	ObjectID AddGameObject(const Entity& entity,const std::wstring& name,const ObjectType& type)
	{
		ObjectID id = static_cast<ObjectID>(m_GameObjects.push_back(std::make_unique<GameObject>(this, m_InputManager, m_ResourceManager, m_ECS, entity, name, type)));
		if (id != entity) { Log::Write(LogLevel::Assert, "not much ObjectID Entity"); }
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
	// ゲームオブジェクトを取得
	GameObject* GetGameObject(const ObjectID& index) {
		if (!m_GameObjects.isValid(index))
		{
			return nullptr;
		}
		return m_GameObjects[index].get();
	}
	/*GameObject& GetGameObject(const ObjectID& index)
	{
		if (!m_GameObjects.isValid(index))
		{
			return m_DummyGameObject;
		}
		return *m_GameObjects[index];
	}*/
	// ゲームオブジェクトコンテナを取得
	FVector<std::unique_ptr<GameObject>>& GetGameObjects() { return m_GameObjects; }
	// 名前検索用補助コンテナを取得
	std::unordered_map<std::wstring, ObjectID>& GetNameToObjectID() { return m_NameToObjectID; }
	// 名前でゲームオブジェクトを取得
	GameObject* GetGameObjectByName(const std::wstring& name)
	{
		if (m_NameToObjectID.contains(name))
		{
			return m_GameObjects[m_NameToObjectID[name]].get();
		}
		return nullptr;
	}
	// 名前でゲームオブジェクトを取得
	/*GameObject& GetGameObjectByName(const std::wstring& name)
	{
		if (m_NameToObjectID.contains(name))
		{
			return *m_GameObjects[m_NameToObjectID[name]];
		}
		return m_DummyGameObject;
	}*/
	// ダミーGameObjectを取得
	//GameObject& GetDummyGameObject() { return m_DummyGameObject; }
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
	// 名前検索用補助コンテナ
	std::unordered_map<std::wstring, ObjectID> m_NameToObjectID;
	// タイプ検索用補助コンテナ
	std::unordered_map<ObjectType, std::vector<ObjectID>> m_TypeToObjectIDs;

	// ダミーGameObject
	//GameObject m_DummyGameObject;
};