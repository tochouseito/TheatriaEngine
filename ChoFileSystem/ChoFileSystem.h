#pragma once

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
#define NOMINMAX

#include <string>
#include <vector>
#include <optional>
#include <filesystem>

//namespace Cho
//{
//    namespace FileSystem
//    {
//        // テキストファイル読み込み
//        std::optional<std::string> ReadText(const std::filesystem::path& path);
//
//        // テキストファイル保存
//        bool WriteText(const std::filesystem::path& path, const std::string& text);
//
//        // バイナリ読み込み
//        std::optional<std::vector<uint8_t>> ReadBinary(const std::filesystem::path& path);
//
//        // バイナリ保存
//        bool WriteBinary(const std::filesystem::path& path, const std::vector<uint8_t>& data);
//
//        // ファイル存在確認
//        bool Exists(const std::filesystem::path& path);
//
//        // フォルダ作成（親も含めて作る）
//        bool CreateDirectory(const std::filesystem::path& path);
//
//        // ディレクトリ内のファイル列挙
//        std::vector<std::filesystem::path> ListFiles(const std::filesystem::path& directory, bool recursive = false);
//    }
//}

