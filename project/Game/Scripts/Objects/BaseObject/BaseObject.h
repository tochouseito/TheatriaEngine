#pragma once
#include<string>
#include"ECS/ComponentManager/Components/Components.h"
#include "Game/Scripts/ColliderID/ColliderType.h"
#include "Audio/AudioManager.h"
class SceneManager;
class ComponentManager;
#include<Input/InputManager.h>

enum Corner {
	kRightBottom, // 右下
	kLeftBottom,  // 左下
	kRightTop,    // 右上
	kLeftTop,     // 左上
	kNumCorner // 要素数
};

struct CollisionMapInfo {
	bool ceilingCollision_ = false;
	bool landingCollision_ = false;
	bool wallCollision_ = false;
	Vector3 movement;
};

class BaseObject
{
public:
	BaseObject();
	virtual ~BaseObject();
	virtual void Create(const std::string& objectName, SceneManager* sceneManager, ComponentManager* compManager, InputManager* inputManager, bool isInstance = false, uint32_t numInstance = 0);
	virtual void Destroy();
	virtual void GetGameObject(const std::string& objectName, SceneManager* sceneManager, ComponentManager* compManager, InputManager* inputManager);
	virtual void Initialize()=0;
	virtual void Update() = 0;
	virtual std::string Name()const { return name; }
	virtual TransformComponent* GetTransform() { return transform; }
	virtual void SetModel(const std::string& modelName);

	// オペレーター

protected:
	std::string name;
	bool createCount = false;
	SceneManager* sceneManager_ = nullptr;
	ComponentManager* compManager_ = nullptr;
	InputManager* input = nullptr;
	AudioManager* audio = nullptr;

	// Components
	TransformComponent* transform = nullptr;
	MeshComponent* mesh = nullptr;
	MaterialComponent* material = nullptr;
	RenderComponent* render = nullptr;
	AnimationComponent* animation = nullptr;
};

