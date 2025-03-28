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
	ObjectID AddGameObject(const Entity& entity) { return static_cast<ObjectID>(m_GameObjects.push_back(GameObject(entity))); }
	// プレハブを追加
	PrefabID AddPrefab(const Entity& entity) { return static_cast<PrefabID>(m_Prefabs.push_back(Prefab(entity))); }
	// ゲームオブジェクトを取得
	GameObject* GetGameObject(const uint32_t& index) { return &m_GameObjects[index]; }
	// プレハブを取得
	Prefab* GetPrefab(const uint32_t& index) { return &m_Prefabs[index]; }
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

