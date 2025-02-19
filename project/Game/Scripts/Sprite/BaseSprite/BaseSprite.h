#pragma once

#include<string>
#include"ECS/ComponentManager/Components/Components.h"
#include"Input/InputManager.h"

class SceneManager;
class ComponentManager;
class BaseSprite
{
public:
	BaseSprite();
	virtual ~BaseSprite();
	virtual void Create(const std::string& objectName, SceneManager* sceneManager, ComponentManager* compManager, InputManager* inputManager);
	virtual void Destroy();
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void SetTexture(const std::string& textureName);
	virtual void SetColor(const Color& color);

protected:
	std::string name;
	bool createCount = false;
	SceneManager* sceneManager_ = nullptr;
	ComponentManager* compManager_ = nullptr;
	InputManager* input = nullptr;
	// Components
	SpriteComponent* sprite = nullptr;
};

