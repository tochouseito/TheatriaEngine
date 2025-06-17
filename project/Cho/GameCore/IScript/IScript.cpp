#include "IScript.h"
#include "Platform/FileSystem/FileSystem.h"
#include "GameCore/ObjectContainer/ObjectContainer.h"

GameObject* IScript::FindGameObjectByName(std::wstring_view name)
{
	GameObject* result = gameObject.m_ObjectContainer->GetGameObjectByName(name.data());
	if (!result)
	{
		return nullptr;
	}
	result->Initialize(false);
	return result;
}
