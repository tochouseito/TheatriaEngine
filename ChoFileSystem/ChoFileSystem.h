#pragma once

#define WIN32_LEAN_AND_MEAN             // Windows �w�b�_�[����قƂ�ǎg�p����Ă��Ȃ����������O����
#define NOMINMAX

#include <string>
#include <vector>
#include <optional>
#include <filesystem>

//namespace Cho
//{
//    namespace FileSystem
//    {
//        // �e�L�X�g�t�@�C���ǂݍ���
//        std::optional<std::string> ReadText(const std::filesystem::path& path);
//
//        // �e�L�X�g�t�@�C���ۑ�
//        bool WriteText(const std::filesystem::path& path, const std::string& text);
//
//        // �o�C�i���ǂݍ���
//        std::optional<std::vector<uint8_t>> ReadBinary(const std::filesystem::path& path);
//
//        // �o�C�i���ۑ�
//        bool WriteBinary(const std::filesystem::path& path, const std::vector<uint8_t>& data);
//
//        // �t�@�C�����݊m�F
//        bool Exists(const std::filesystem::path& path);
//
//        // �t�H���_�쐬�i�e���܂߂č��j
//        bool CreateDirectory(const std::filesystem::path& path);
//
//        // �f�B���N�g�����̃t�@�C����
//        std::vector<std::filesystem::path> ListFiles(const std::filesystem::path& directory, bool recursive = false);
//    }
//}

