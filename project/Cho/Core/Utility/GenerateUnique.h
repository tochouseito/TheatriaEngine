#pragma once
#include <string>
#include <unordered_map>
#include <regex>
using ObjectID = uint32_t;

namespace Cho
{
    inline std::wstring GenerateUniqueName(
        const std::wstring& baseName,
        const std::unordered_map<std::wstring, ObjectID>& nameList)
    {
        std::wstring newName = baseName;
        int counter = 1;

        // 正規表現で "(番号)" を検出（ワイド文字版）
        std::wregex numberRegex(LR"(^(.+?) \((\d+)\)$)");
        std::wsmatch match;

        if (std::regex_match(baseName, match, numberRegex))
        {
            // "baseName (番号)" の場合
            newName = match[1];                    // ベース名（括弧前の部分）
            counter = std::stoi(match[2]) + 1;     // 現在の番号を取得して次から判定
        } else
        {
            // 括弧がない場合、すでに重複していないならそのまま返す
            if (!nameList.contains(baseName))
            {
                return baseName;
            }
        }

        // 名前が重複している間、新しい名前に番号を追加してチェック
        while (nameList.contains(newName + L" (" + std::to_wstring(counter) + L")"))
        {
            counter++;
        }

        return newName + L" (" + std::to_wstring(counter) + L")";
    }
}