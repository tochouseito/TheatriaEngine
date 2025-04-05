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
	Entity GetEntity() const noexcept { return m_Entity; }
private:
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
	Entity GetEntity() const noexcept { return m_Entity; }
private:
	Entity m_Entity;// エンティティ
	std::wstring m_Name = L"";// ゲームオブジェクト名
	ObjectType m_Type;// ゲームオブジェクトのタイプ
};

