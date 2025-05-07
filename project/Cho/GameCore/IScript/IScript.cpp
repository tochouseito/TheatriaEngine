#include "IScript.h"
#include "Platform/FileSystem/FileSystem.h"
#include "GameCore/ObjectContainer/ObjectContainer.h"

GameObject& IScript::FindGameObjectByName(std::wstring_view name)
{
	GameObject& result = gameObject.m_ObjectContainer->GetGameObjectByName(name.data());
	if (!result.IsActive())
	{
		return result.m_ObjectContainer->GetDummyGameObject();
	}
	result.Initialize(false);
	return result;
}
