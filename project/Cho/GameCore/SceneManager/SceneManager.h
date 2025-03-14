#pragma once
#include "Cho/GameCore/ECS/EntityManager/EntityManager.h"
#include "Cho/GameCore/ECS/ComponentManager/ComponentManager.h"
#include "Cho/GameCore/ECS/SystemManager/SystemManager.h"
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
	EntityManager m_EntityManager;
	ComponentManager m_ComponentManager;
	SystemManager m_SystemManager;
	ObjectContainer m_ObjectContainer;
};

