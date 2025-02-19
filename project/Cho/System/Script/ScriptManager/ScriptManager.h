#pragma once

#include <unordered_map>
#include <string>
#include <regex>
#include"ObjectType/ObjectType.h"
#include"Script/ScriptStatus.h"

class ScriptManager
{
public:
	void Initialize();

	void Load();

	std::string AddScript(ObjectType type,std::string& NewName);

	const std::unordered_map < ObjectType, std::unordered_map < std::string, ScriptStatus>>& GetScripts() const{
		return scripts;
	}
private:

	// ユニークな名前を生成する関数
	std::string GenerateUniqueName(
		const std::unordered_map < std::string, ScriptStatus > & subMapScripts,
		const std::string& baseName);

	std::unordered_map < ObjectType, std::unordered_map < std::string, ScriptStatus>> scripts;
};

