#include "pch.h"
#define ENGINECREATE_FUNCTION
#define USE_CHOENGINE_SCRIPT
#include "ChoEngineAPI.h"
#include "Main/ChoEngine.h"

CHO_API Engine* Cho::CreateEngine(RuntimeMode mode)
{
    return new ChoEngine(mode);
}

CHO_API void Cho::DestroyEngine(Engine* engine)
{
	delete engine;
}

CHO_API bool ChoSystem::SaveGameParameter(const std::wstring& filePath, const std::string& group, const std::string& item, const std::string& dataName, const GameParameterVariant& value)
{
	return Cho::FileSystem::SaveGameParameter(filePath, group, item, dataName, value);
}

CHO_API bool ChoSystem::LoadGameParameter(const std::wstring& filePath, const std::string& group, const std::string& item, const std::string& dataName, GameParameterVariant& outValue)
{
	return Cho::FileSystem::LoadGameParameter(filePath, group, item, dataName, outValue);
}
