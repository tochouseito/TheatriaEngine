#pragma once

#include"ECS/ComponentManager/Components/CameraComponent/CameraComponent.h"

class ComponentManager;
class SystemManager;
class InputManager;

class DebugCamera
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ComponentManager* componentManager,SystemManager* systemManager,InputManager* inputManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(bool sceneViewHover);

public:// getter
	const CameraComponent& GetCameraComponent()const { return cameraComponent; }

public:// Setter
	void SetAspect(const float& newAspect) { cameraComponent.aspectRatio = newAspect; }

	void SetMousePos(const Vector2& pos) { mousePos = pos; }

	void SetCurrentMousePos(const Vector2& pos) { deltaMousePos = pos; }

	void AddCurrentMousePos(const Vector2& pos) { deltaMousePos += pos; }

	void AddMousePos(const Vector2& pos) { mousePos += pos; }
private:
	// ポインタ
	ComponentManager* componentManager_ = nullptr;
	SystemManager* systemManager_ = nullptr;
	InputManager* inputManager_ = nullptr;

	// カメラコンポーネント	
	CameraComponent cameraComponent;

	// カーソルを動かすときの感度
	const float mouseSensitivity = 0.1f;
	// カメラの移動速度
	const float moveSpeed = 0.005f;

	Vector2 mousePos;
	Vector2 deltaMousePos;
};

