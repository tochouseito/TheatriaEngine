#include "IScript.h"
#include "Platform/FileSystem/FileSystem.h"
#include "GameCore/ObjectContainer/ObjectContainer.h"

GameObject& IScript::FindGameObjectByName(const std::wstring& name)
{
	Cho::Log::Write(Cho::LogLevel::Info, "IScript::FindGameObjectByName: " + ConvertString(name));
	GameObject& result = gameObject.m_ObjectContainer->GetGameObjectByName(name);
	if (!result.IsActive())
	{
		return result.m_ObjectContainer->GetDummyGameObject();
	}
	result.Initialize();
	Cho::Log::Write(Cho::LogLevel::Info, "IScript::FindGameObjectByName: " + ConvertString(name) + " found");
	return result;
}
