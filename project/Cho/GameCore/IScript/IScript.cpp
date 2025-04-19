#include "IScript.h"
#include "GameCore/ObjectContainer/ObjectContainer.h"

GameObject& IScript::FindGameObjectByName(const std::wstring& name)
{
	GameObject& result = gameObject.m_ObjectContainer->GetGameObjectByName(name);
	if (!result.IsActive())
	{
		return result.m_ObjectContainer->GetDummyGameObject();
	}
	result.Initialize();
	return result;
}
