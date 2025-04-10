#pragma once
#include "Components.h"
struct ScriptContext
{
	TransformComponent* transform = nullptr;		// Transformコンポーネント
	CameraComponent* camera = nullptr;				// Cameraコンポーネント
	MeshFilterComponent* meshFilter = nullptr;		// MeshFilterコンポーネント
	MeshRendererComponent* meshRenderer = nullptr;	// MeshRendererコンポーネント
	ScriptComponent* script = nullptr;				// スクリプトコンポーネント
};

//struct ScriptContext
//{
//	std::optional<std::reference_wrapper<TransformComponent>> transform=std::nullopt;			// Transformコンポーネント
//	std::optional<std::reference_wrapper<CameraComponent>> camera=std::nullopt;					// Cameraコンポーネント
//	std::optional<std::reference_wrapper<MeshFilterComponent>> meshFilter = std::nullopt;		// MeshFilterコンポーネント
//	std::optional<std::reference_wrapper<MeshRendererComponent>> meshRenderer = std::nullopt;	// MeshRendererコンポーネント
//	std::optional<std::reference_wrapper<ScriptComponent>> script = std::nullopt;				// スクリプトコンポーネント
//};