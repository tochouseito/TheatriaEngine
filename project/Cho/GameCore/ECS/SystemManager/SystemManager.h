#pragma once
#include "Cho/GameCore/ECS/ComponentManager/ComponentManager.h"
class SystemManager
{
public:
	// Constructor
	SystemManager()
	{
	}
	// Destructor
	~SystemManager()
	{
	}
	void Update(const EntityManager& entityManager, const ComponentManager& componentManager);
private:
	void TransformUpdate(const EntityManager& entityManager, const ComponentManager& componentManager);
};

