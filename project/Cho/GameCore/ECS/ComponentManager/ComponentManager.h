#pragma once
#include "Cho/GameCore/ECS/EntityManager/EntityManager.h"
class Component
{

};
class TransformComponent : public Component
{
public:
	void Initialize()
	{
	}
};

class ComponentManager
{
public:
	// Constructor
	ComponentManager()
	{
	}
	// Destructor
	~ComponentManager()
	{
	}
	void AddNewComponent(const Entity& entity) { m_TransformComponents[entity].Initialize(); }
private:
	FVector<TransformComponent> m_TransformComponents;
};

