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
	uint32_t AddGameObject(const Entity& entity) { return static_cast<uint32_t>(m_GameObjects.push_back(GameObject(entity))); }
	// プレハブを追加
	uint32_t AddPrefab(const Entity& entity) { return static_cast<uint32_t>(m_Prefabs.push_back(Prefab(entity))); }
	// ゲームオブジェクトを取得
	GameObject* GetGameObject(const uint32_t index) { return &m_GameObjects[index]; }
	// プレハブを取得
	Prefab* GetPrefab(const uint32_t index) { return &m_Prefabs[index]; }
	// ゲームオブジェクトコンテナを取得
	FVector<GameObject>& GetGameObjects() { return m_GameObjects; }
	// プレハブコンテナを取得
	FVector<Prefab>& GetPrefabs() { return m_Prefabs; }
private:
	FVector<GameObject> m_GameObjects;
	FVector<Prefab> m_Prefabs;
};

