#pragma once
#include "GameCore/ECS/ECSManager.h"
struct ScriptContext
{
	// Constructor
	ScriptContext(Entity entity, ECSManager* ecs) :
		entity(entity), ecs(ecs)
	{
	}

	TransformComponent* transform = nullptr;		// Transformコンポーネント
	CameraComponent* camera = nullptr;				// Cameraコンポーネント
	MeshFilterComponent* meshFilter = nullptr;		// MeshFilterコンポーネント
	MeshRendererComponent* meshRenderer = nullptr;	// MeshRendererコンポーネント
	ScriptComponent* script = nullptr;				// スクリプトコンポーネント

	// Componentの追加
private:
	std::optional<Entity> entity = std::nullopt;	// スクリプトのエンティティ
	ECSManager* ecs = nullptr;						// ECSマネージャ
};

//struct ScriptContext
//{
//	std::optional<std::reference_wrapper<TransformComponent>> transform=std::nullopt;			// Transformコンポーネント
//	std::optional<std::reference_wrapper<CameraComponent>> camera=std::nullopt;					// Cameraコンポーネント
//	std::optional<std::reference_wrapper<MeshFilterComponent>> meshFilter = std::nullopt;		// MeshFilterコンポーネント
//	std::optional<std::reference_wrapper<MeshRendererComponent>> meshRenderer = std::nullopt;	// MeshRendererコンポーネント
//	std::optional<std::reference_wrapper<ScriptComponent>> script = std::nullopt;				// スクリプトコンポーネント
//};