#pragma once

using Entity = uint32_t;

class Prefab 
{
public:
	// Constructor
	Prefab(const Entity& entity) :
		m_Entity(entity)
	{
	}
	// Destructor
	~Prefab()
	{
	}
private:
	Entity m_Entity;
};
class GameObject
{
public:
	// Constructor
	GameObject(const Entity& entity):
		m_Entity(entity)
	{
	}
	// Destructor
	~GameObject()
	{
	}
private:
	Entity m_Entity;
};

