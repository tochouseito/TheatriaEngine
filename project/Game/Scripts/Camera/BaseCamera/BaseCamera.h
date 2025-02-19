#pragma once
#include<string>
#include"ECS/ComponentManager/Components/Components.h"
class SceneManager;
class ComponentManager;
#include<Input/InputManager.h>
class BaseCamera
{
public:
	BaseCamera();
	virtual ~BaseCamera();
	virtual void Create(const std::string& cameraName, SceneManager* sceneManager, ComponentManager* compManager, InputManager* inputManager);
	virtual void Destroy();
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual std::string Name()const { return name; }
protected:
	std::string name;
	bool createCount = false;
	SceneManager* sceneManager_ = nullptr;
	ComponentManager* compManager_ = nullptr;
	InputManager* input = nullptr;

	// Components
	CameraComponent* camera = nullptr;
};

