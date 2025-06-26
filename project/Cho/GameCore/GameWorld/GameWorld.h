#pragma once
#include "GameCore/GameObject/GameObject.h"
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
	// オブジェクト取得
	GameObject* GetGameObject(const SceneID sceneID, const Entity& entity)
	{

	}
	// オブジェクトを追加
	void AddGameObject(GameObject obj);
	// クローンを追加
	void AddGameObjectClone(const Entity& srcEntity, GameObject clone);
	// オブジェクトを削除
	void RemoveGameObject(const Entity& e);
private:
	// GameObjectコンテナ
	// [SceneID][EntityID][CloneID] = GameObject
	FVector<FVector<FVector<std::unique_ptr<GameObject>>>> m_pGameObjects;
	// 名前検索
	// [SceneID][ObjectName] = Entity
	FVector<std::unordered_map<std::wstring, Entity>> m_ObjectNameMap;
};

