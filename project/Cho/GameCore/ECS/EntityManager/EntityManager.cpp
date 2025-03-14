#include "pch.h"
#include "EntityManager.h"

Entity EntityManager::CreateEntity()
{
	return m_Entities.push_back(Entity());
}

void EntityManager::DestroyEntity(const Entity& entity)
{
	m_Entities.erase(entity);
}
