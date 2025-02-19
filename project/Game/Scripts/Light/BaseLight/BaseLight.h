#pragma once
#include <string>
#include "ECS/ComponentManager/Components/Components.h"
#include <Input/InputManager.h>

enum LightType
{
	Directional,
	Point,
	Spot
};

class SceneManager;
class ComponentManager;

class BaseLight
{
public:
	BaseLight();
	virtual ~BaseLight();
	virtual void Create(const std::string& objectName,LightType type, SceneManager* sceneManager, ComponentManager* compManager, InputManager* inputManager);
	virtual void Destroy();
	virtual void GetGameObject(const std::string& objectName,LightType type, SceneManager* sceneManager, ComponentManager* compManager, InputManager* inputManager);
	virtual void Initialize() = 0;
	virtual void Update() = 0;

	virtual void SetDirection(const Vector3& direction);

protected:
	std::string name;
	bool createCount = false;
	SceneManager* sceneManager_ = nullptr;
	ComponentManager* compManager_ = nullptr;
	InputManager* input = nullptr;

	// Components
	DirectionalLightComponent* dirLight = nullptr;
	PointLightComponent* pointLight = nullptr;
	SpotLightComponent* spotLight = nullptr;
};

