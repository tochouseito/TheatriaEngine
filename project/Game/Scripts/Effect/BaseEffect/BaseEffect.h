#pragma once
#include "ECS/ComponentManager/Components/Components.h"
#include "Input/InputManager.h"
class SceneManager;
class ComponentManager;
class BaseEffect
{
public:
	BaseEffect() = default;
	virtual ~BaseEffect();
	virtual void Create(const std::string& objectName, SceneManager* sceneManager, ComponentManager* compManager, InputManager* inputManager);
	virtual void Destroy();
	virtual void Initialize() = 0;
	virtual void Update() = 0;

	virtual void CreateEffectResource();
	virtual void Start();
	virtual void Reset();
	virtual bool IsEnd() { return effect->isEnd; }
	virtual void Stop() { effect->isRun = false; }
	virtual void SetPosition(const Vector3& pos);

protected:
	std::string name;
	bool createCount = false;
	SceneManager* sceneManager_ = nullptr;
	ComponentManager* compManager_ = nullptr;
	InputManager* input = nullptr;

	// コンポーネント
	EffectComponent* effect = nullptr;
};

