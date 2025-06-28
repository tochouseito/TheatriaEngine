#pragma once
#include "GameCore/GameObject/GameObject.h"
#include "GameCore/ECS/ECSManager.h"
class CPrefab : public IPrefab
{
public:
	CPrefab() = default;
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

