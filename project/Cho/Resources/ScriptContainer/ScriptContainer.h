#pragma once
#include <string>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <memory>
#include <functional>
#include "Core/Utility/FVector.h"
#include "GameCore/IScript/IScript.h"
using ScriptFactoryFunc = std::function<std::unique_ptr<IScript>()>;

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
		// すでに同じ名前のスクリプトが存在する場合は何もしない
		if (m_ScriptNameToIndex.contains(scriptName))
		{
			return; // 既に存在する場合は何もしない
		}
		std::string name = scriptName;
		uint32_t index = static_cast<uint32_t>(m_ScriptNameContainer.push_back(std::move(name))); // スクリプト名をコンテナに追加
		m_ScriptNameToIndex[scriptName] = index; // スクリプト名とインデックスをマップに追加
	}
	// スクリプトデータをIDで取得する関数
	std::optional<std::string> GetScriptDataByID(const uint32_t & index)
	{
		if (m_ScriptNameContainer.isValid(index))
		{
			return m_ScriptNameContainer[index];
		} else
		{
			return std::nullopt; // 無効なIDの場合は空のデータを返す
		}
	}
	// スクリプトデータを名前で取得する関数
	std::string GetScriptDataByName(const std::string & scriptName)
	{
		if (m_ScriptNameToIndex.contains(scriptName))
		{
			uint32_t id = m_ScriptNameToIndex[scriptName];
			return GetScriptDataByID(id).value();
		}
		return std::string(); // 無効な名前の場合は空のデータを返す
	}
	size_t GetScriptCount() { return m_ScriptNameContainer.GetVector().size(); }
	FVector<std::string>& GetScriptNameContainer() { return m_ScriptNameContainer; } // スクリプト名コンテナを取得

	void RegisterScript(const std::string& scriptName, ScriptFactoryFunc func);
	std::unique_ptr<IScript> CreateScript(const std::string& scriptName);
private:
	FVector<std::string> m_ScriptNameContainer; // スクリプトデータを格納するベクター
	// 名前で検索する用のコンテナ
	std::unordered_map<std::string, uint32_t> m_ScriptNameToIndex; // スクリプト名からスクリプトIDを取得するためのマップ

	// 登録されたスクリプト名と生成関数のマップ
	std::unordered_map<std::string, ScriptFactoryFunc> m_ScriptRegistry;
};

