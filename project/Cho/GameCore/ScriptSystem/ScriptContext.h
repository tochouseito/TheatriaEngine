#pragma once
#include "Core/Utility/Components.h"
struct ScriptContext
{
	TransformComponent* transform = nullptr;	// Transformコンポーネント
	CameraComponent* camera = nullptr;		// Cameraコンポーネント
	MeshFilterComponent* meshFilter = nullptr;	// MeshFilterコンポーネント
	MeshRendererComponent* meshRenderer = nullptr;	// MeshRendererコンポーネント
};