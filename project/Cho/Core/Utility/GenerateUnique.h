#pragma once
#include <string>
#include <regex>
#include <unordered_map>
#include <set>

namespace Cho
{
    // ユーティリティ：`container` に `key` が含まれているかチェック（ADLで見つかる `find` を使う）
    template<typename Container, typename Key>
    bool ContainsKey(const Container& container, const Key& key)
    {
        return container.find(key) != container.end();
    }

    template<typename Container>
    inline std::wstring GenerateUniqueName(
        const std::wstring& baseName,
        const Container& nameList)
    {
        std::wstring newName = baseName;
        int counter = 1;

        std::wregex numberRegex(LR"(^(.+?) \((\d+)\)$)");
        std::wsmatch match;

        if (std::regex_match(baseName, match, numberRegex))
        {
            newName = match[1];
            counter = std::stoi(match[2]) + 1;
        }
        else
        {
            if (!ContainsKey(nameList, baseName))
                return baseName;
        }

        while (ContainsKey(nameList, newName + L"(" + std::to_wstring(counter) + L")"))
        {
            counter++;
        }

        return newName + L"(" + std::to_wstring(counter) + L")";
    }
}