#pragma once
#include "Cho/GameCore/GameObject/GameObject.h"
#include "Cho/Core/Utility/FVector.h"
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
		return static_cast<ObjectID>(m_GameObjects.push_back(GameObject(entity, name, type)));
	}
	// プレハブを追加
	PrefabID AddPrefab(const Entity& entity, const std::wstring& name, const ObjectType& type)
	{ 
		return static_cast<PrefabID>(m_Prefabs.push_back(Prefab(entity, name, type)));
	}
	// ゲームオブジェクトを取得
	GameObject* GetGameObject(const ObjectID& index) { return &m_GameObjects[index]; }
	// プレハブを取得
	Prefab* GetPrefab(const ObjectID& index) { return &m_Prefabs[index]; }
	// ゲームオブジェクトコンテナを取得
	FVector<GameObject>& GetGameObjects() { return m_GameObjects; }
	// プレハブコンテナを取得
	FVector<Prefab>& GetPrefabs() { return m_Prefabs; }
private:
	// ゲームオブジェクトコンテナ
	FVector<GameObject> m_GameObjects;
	// プレファブコンテナ
	FVector<Prefab> m_Prefabs;

};

