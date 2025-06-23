#pragma once
#include "GameCore/GameObject/GameObject.h"
#include "GameCore/Prefab/Prefab.h"
#include "Core/Utility/FVector.h"
class GameWorld
{
	friend class SceneManager;
public:
	// Constructor
	GameWorld()
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
private:
	// オブジェクトを追加
	void AddGameObject(GameObject obj);
	// クローンを追加
	void AddGameObjectClone(const ObjectID& srcID, GameObject clone);
	// オブジェクトを削除
	void RemoveGameObject(const ObjectID& id);

	// GameObjectコンテナ
	// [SceneID][EntityID][CloneID] = GameObject
	FVector<FVector<FVector<std::unique_ptr<GameObject>>>> m_pGameObjects;
	// 名前検索
	std::unordered_map<std::wstring, ObjectID> m_ObjectNameMap;
};

