#pragma once
#include <string>
#include "Core/Utility/IDType.h"

inline const char* ObjectTypeToWString(ObjectType type)
{
	switch (type)
	{
	case ObjectType::MeshObject: return "MeshObject";
	case ObjectType::Camera:  return "Camera";
	default:                  return "Unknown";
	}
}

inline ObjectType ObjectTypeFromString(const std::string& str)
{
	if (str == "MeshObject") return ObjectType::MeshObject;
	if (str == "Camera") return ObjectType::Camera;
	return ObjectType::Count; // または Unknown があればそちら
}

class Prefab 
{
public:
	// Constructor
	Prefab(const Entity& entity,const std::wstring& name,const ObjectType& type) :
		m_Entity(entity),  m_Name(name), m_Type(type)
	{
	}
	// Constructor
	Prefab() {}
	// Destructor
	~Prefab()
	{
	}
	ObjectID GetID() const noexcept { return m_ID; }
	Entity GetEntity() const noexcept { return m_Entity; }
	std::wstring GetName() const noexcept { return m_Name; }
	ObjectType GetType() const noexcept { return m_Type; }
	void SetID(const ObjectID& id) noexcept { m_ID = id; }
private:
	PrefabID m_ID;// プレハブID
	Entity m_Entity;// エンティティ
	std::wstring m_Name = L"";// プレハブ名
	ObjectType m_Type;// プレハブのタイプ
};
class GameObject
{
public:
	// Constructor
	GameObject(const Entity& entity, const std::wstring& name, const ObjectType& type) :
		m_Entity(entity), m_Name(name), m_Type(type)
	{
	}
	// Constructor
	GameObject(){}
	// Destructor
	~GameObject()
	{
	}
	ObjectID GetID() const noexcept { return m_ID; }
	Entity GetEntity() const noexcept { return m_Entity; }
	std::wstring GetName() const noexcept { return m_Name; }
	ObjectType GetType() const noexcept { return m_Type; }
	void SetID(const ObjectID& id) noexcept { m_ID = id; }
	void SetName(const std::wstring& name) noexcept { m_Name = name; }
private:
	ObjectID m_ID;// オブジェクトID
	Entity m_Entity;// エンティティ
	std::wstring m_Name = L"";// ゲームオブジェクト名
	ObjectType m_Type;// ゲームオブジェクトのタイプ
};

