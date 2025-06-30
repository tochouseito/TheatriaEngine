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
	// オブジェクトを作成
	ObjectHandle CreateGameObject(const std::wstring& name, ObjectType type);
private:
	// シーンデータからオブジェクトを作成
	SceneID AddGameObjectFromScene(const GameScene& scene);
	// ワールドからGameSceneを生成
	GameScene CreateGameSceneFromWorld() const
	{

	}
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
	void ClearAllScenes();
	// タイプごとの初期コンポーネントを追加
	void AddDefaultComponentsToGameObject(ObjectHandle handle, ObjectType type)
	{
		// 基本コンポーネントを追加
		m_pECSManager->AddComponent<TransformComponent>(handle.entity);
		switch (type)
		{
		case ObjectType::MeshObject:
			m_pECSManager->AddComponent<MeshFilterComponent>(handle.entity);
			m_pECSManager->AddComponent<MeshRendererComponent>(handle.entity);
			break;
		case ObjectType::Camera:
			m_pECSManager->AddComponent<CameraComponent>(handle.entity);
			break;
		case ObjectType::ParticleSystem:
			m_pECSManager->AddComponent<ParticleComponent>(handle.entity);
			m_pECSManager->AddComponent<EmitterComponent>(handle.entity);
			break;
		case ObjectType::Effect:
			break;
		case ObjectType::Light:
			break;
		case ObjectType::UI:
			break;
		case ObjectType::None:
			break;
		case ObjectType::Count:
			break;
		default:
			break;
		}
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

