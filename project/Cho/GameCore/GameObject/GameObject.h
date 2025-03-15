#pragma once

using Entity = uint32_t;

class Prefab
{
public:
	// Constructor
	Prefab()
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
	GameObject()
	{
	}
	// Destructor
	~GameObject()
	{
	}
private:
	Entity m_Entity;
};

