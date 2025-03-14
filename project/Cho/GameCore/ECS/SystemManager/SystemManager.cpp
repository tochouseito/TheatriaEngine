#include "pch.h"
#include "SystemManager.h"

void SystemManager::Update(const EntityManager& entityManager, const ComponentManager& componentManager)
{
	TransformUpdate(entityManager, componentManager);
}

void SystemManager::TransformUpdate(const EntityManager& entityManager, const ComponentManager& componentManager)
{
}
