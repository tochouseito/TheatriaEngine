#pragma once
#include <string>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include "Core/Utility/FVector.h"
using ScriptID = uint32_t; // スクリプトID
struct ScriptData
{
	std::string scriptName; // スクリプト名
	std::optional<ScriptID> scriptID; // スクリプトID
};
// スクリプトを格納するコンテナクラス
class ScriptContainer
{
public:
	// Constructor
	ScriptContainer() = default;
	// Destructor
	~ScriptContainer() = default;
	// スクリプトデータを追加する関数
	void AddScriptData(const std::string& scriptName)
	{
		ScriptData scriptData;
		scriptData.scriptName = scriptName;
		ScriptID id = static_cast<ScriptID>(m_ScriptContainer.push_back(std::move(scriptData)));
		m_ScriptContainer[id].scriptID = id; // スクリプトIDを設定
		m_ScriptNameToID[scriptName] = id; // スクリプト名とIDをマップに追加
	}
	// スクリプトデータをIDで取得する関数
	ScriptData GetScriptDataByID(ScriptID id)
	{
		if (id < m_ScriptContainer.size())
		{
			return m_ScriptContainer[id];
		} else
		{
			return ScriptData(); // 無効なIDの場合は空のデータを返す
		}
	}
	// スクリプトデータを名前で取得する関数
	ScriptData GetScriptDataByName(const std::string& scriptName)
	{
		if (m_ScriptNameToID.contains(scriptName))
		{
			ScriptID id = m_ScriptNameToID[scriptName];
			return GetScriptDataByID(id);
		}
		return ScriptData(); // 無効な名前の場合は空のデータを返す
	}
	size_t GetScriptCount() const { return m_ScriptContainer.size(); }
private:
	FVector<ScriptData> m_ScriptContainer; // スクリプトデータを格納するベクター
	// 名前で検索する用のコンテナ
	std::unordered_map<std::string, ScriptID> m_ScriptNameToID; // スクリプト名からスクリプトIDを取得するためのマップ
};

