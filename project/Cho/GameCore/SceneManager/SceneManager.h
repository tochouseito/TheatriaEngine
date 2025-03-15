#pragma once
#include "Cho/GameCore/ECS/ECSManager.h"
#include "Cho/GameCore/ObjectContainer/ObjectContainer.h"
class BaseScene {
public:

protected:

};

class ScenePrefab : public BaseScene {
public:

private:

};

class SceneManager
{
public:
	// Constructor
	SceneManager()
	{
	}
	// Destructor
	~SceneManager()
	{
	}
private:
	BaseScene* m_pCurrentScene = nullptr;
	BaseScene* m_pNextScene = nullptr;
	ECSManager m_ECSManager;
	ObjectContainer m_ObjectContainer;
};

