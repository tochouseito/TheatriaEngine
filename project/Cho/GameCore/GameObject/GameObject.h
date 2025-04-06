#pragma once
#include <string>
using Entity = uint32_t;
using ObjectID = uint32_t;
using PrefabID = uint32_t;

// オブジェクトのタイプ
enum class ObjectType 
{
	MeshObject = 0,// メッシュオブジェクト
	Camera,// カメラオブジェクト
	Count,// カウント
};

inline const char* ObjectTypeToWString(ObjectType type)
{
	switch (type)
	{
	case ObjectType::MeshObject: return "MeshObject";
	case ObjectType::Camera:  return "Camera";
	default:                  return "Unknown";
	}
}

class Prefab 
{
public:
	// Constructor
	Prefab(const Entity& entity,const std::wstring& name,const ObjectType& type) :
		m_Entity(entity),  m_Name(name), m_Type(type)
	{
	}
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
	// Destructor
	~GameObject()
	{
	}
	ObjectID GetID() const noexcept { return m_ID; }
	Entity GetEntity() const noexcept { return m_Entity; }
	std::wstring GetName() const noexcept { return m_Name; }
	ObjectType GetType() const noexcept { return m_Type; }
	void SetID(const ObjectID& id) noexcept { m_ID = id; }
private:
	ObjectID m_ID;// オブジェクトID
	Entity m_Entity;// エンティティ
	std::wstring m_Name = L"";// ゲームオブジェクト名
	ObjectType m_Type;// ゲームオブジェクトのタイプ
};

