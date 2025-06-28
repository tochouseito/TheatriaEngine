#pragma once
#include "GameCore/GameObject/GameObject.h"
#include "Core/Utility/FVector.h"

// 前方宣言
class ECSManager;

class GameWorld
{
	friend class SceneManager;
public:
	// Constructor
	GameWorld(ECSManager* ecs):m_pECSManager(ecs)
	{
	}
	// Destructor
	~GameWorld()
	{
	}
	void Initialize();
	void Update();
	// オブジェクトを取得
	GameObject* GetGameObject(const std::wstring& name);
	GameObject* GetGameObject(const Entity& e);
	GameObject* GetGameObject(const ObjectHandle& handle);
	// メインカメラを取得
	GameObject* GetMainCamera() { return m_pMainCamera; }
	// メインカメラを空にする
	void ClearMainCamera() { m_pMainCamera = nullptr; }
private:
	// オブジェクトを作成
	ObjectHandle CreateGameObject(const std::wstring& name, ObjectType type);
	// シーンデータからオブジェクトを作成
	SceneID AddGameObjectFromScene(const GameScene& scene);
	// クローンを追加
	void AddGameObjectClone(const GameObject& src)
	{

	}
	// オブジェクトを削除
	void RemoveGameObject(const Entity& e)
	{

	}
	// メインカメラを設定
	void SetMainCamera(GameObject* pCamera)
	{
		m_pMainCamera = pCamera;
	}
	// 全シーン破棄
	void ClearAllScenes()
	{
		for (auto& scene : m_pGameObjects)
		{
			for (auto& object : scene)
			{
				for (auto& clone : object)
				{
					std::wstring name = clone->GetName();
					Entity entity = clone->GetHandle().entity;
					// ECSから削除
					m_pECSManager->RemoveEntity(entity);
				}
			}
		}
		// コンテナと辞書をクリア
		m_pGameObjects.clear();
		m_ObjectHandleMap.clear();
		m_ObjectHandleMapFromEntity.clear();
		// メインカメラをクリア
		m_pMainCamera = nullptr;
	}

	ECSManager* m_pECSManager = nullptr;	
	// GameObjectコンテナ
	// [SceneID][ObjectID][CloneID] = GameObject
	FVector<FVector<FVector<std::unique_ptr<GameObject>>>> m_pGameObjects;
	// 名前検索辞書
	std::unordered_map<std::wstring, ObjectHandle> m_ObjectHandleMap;
	// Entityから逆引きする辞書
	std::unordered_map<Entity, ObjectHandle> m_ObjectHandleMapFromEntity;
	// メインカメラ
	GameObject* m_pMainCamera = nullptr;
};

