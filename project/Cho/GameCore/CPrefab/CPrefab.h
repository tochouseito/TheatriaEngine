#pragma once
#include "GameCore/GameObject/GameObject.h"
#include "GameCore/ECS/ECSManager.h"
class CPrefab : public IPrefab
{
public:
	CPrefab(const std::wstring& name, ObjectType type)
		: m_Name(name), m_Type(type)
	{
	}
	~CPrefab() = default;

	std::wstring GetName() const { return m_Name; }
	ObjectType GetType() const { return m_Type; }
private:
	Entity CreateEntity(ECSManager& ecs) const override
	{
		Entity e = IPrefab::CreateEntity(ecs);

		return e;
	}

	std::wstring m_Name = L"";
	ObjectType m_Type = ObjectType::None;
};

