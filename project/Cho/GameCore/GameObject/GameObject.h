#pragma once
#include <string>
using Entity = uint32_t;
using ObjectID = uint32_t;
using PrefabID = uint32_t;

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
	Entity GetEntity() const noexcept { return m_Entity; }
private:
	Entity m_Entity;// エンティティ
	std::wstring m_Name = L"";// プレハブ名
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
	Entity GetEntity() const noexcept { return m_Entity; }
private:
	Entity m_Entity;// エンティティ
	std::wstring m_Name = L"";// ゲームオブジェクト名
};

