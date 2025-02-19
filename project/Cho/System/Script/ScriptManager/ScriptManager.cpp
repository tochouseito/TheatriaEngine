#include "PrecompiledHeader.h"
#include "ScriptManager.h"
#include<assert.h>

void ScriptManager::Initialize()
{
	// 全タイプのスクリプトコンテナを用意
	scripts[ObjectType::Object];
	scripts[ObjectType::Camera];
	//scripts[ObjectType::Light];
}

void ScriptManager::Load()
{

}

// スクリプトを新規追加
std::string ScriptManager::AddScript(ObjectType type, std::string& NewName)
{
	std::string& newName = NewName;
	
	if (!scripts.contains(type)) {
		assert(0);
	}
	
	if (scripts[type].contains(newName)) {
		newName = GenerateUniqueName(scripts[type], newName);
	}
	ScriptStatus& script = scripts[type][newName];
	script.name = newName; script.type = type;
	return newName;
}

// ユニークな名前を生成する関数

std::string ScriptManager::GenerateUniqueName(const std::unordered_map<std::string, ScriptStatus>& subMapScripts, const std::string& baseName) {
    std::string newName = baseName;
    int counter = 1;

    // 正規表現で末尾の番号を検出
    std::regex numberRegex(R"(^(.*?)(\d+)$)");
    std::smatch match;

    if (std::regex_match(baseName, match, numberRegex)) {
        // "baseName数字" の場合
        newName = match[1];                 // ベース名（番号前の部分）
        counter = std::stoi(match[2]) + 1; // 現在の番号を取得して次から判定
    } else {
        // 末尾に番号がない場合、すでに重複していないならそのまま返す
        if (!subMapScripts.contains(baseName)) {
            return baseName;
        }
    }

    // 名前が重複している間、新しい名前に番号を追加してチェック
    while (subMapScripts.contains(newName + std::to_string(counter))) {
        counter++;
    }

    return newName + std::to_string(counter);
}
