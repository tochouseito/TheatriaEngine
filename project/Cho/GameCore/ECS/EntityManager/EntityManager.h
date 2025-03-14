#pragma once
#include <cstdint>
#include "Cho/Core/Utility/FVector.h"

using Entity = uint32_t;
class EntityManager
{
public:
	// Constructor
	EntityManager()
	{
	}
	// Destructor
	~EntityManager()
	{
	}
	Entity CreateEntity();
	void DestroyEntity(const Entity& entity);
	std::vector<Entity>& GetEntities() { return m_Entities.GetVector(); }
private:
	FVector<Entity> m_Entities;
};

