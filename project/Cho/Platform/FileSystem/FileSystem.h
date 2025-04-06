#pragma once
#include <string>
#include <optional>
#include <filesystem>
using namespace std::filesystem;
class FileSystem
{
public:
	// Constructor
	FileSystem()
	{
	}
	// Destructor
	~FileSystem()
	{
	}
    static std::optional<std::filesystem::path> FindOrCreateGameProjects()
    {
        namespace fs = std::filesystem;
        fs::path execPath = fs::current_path();
        fs::path gameProjectsPath = execPath / "GameProjects";

        if (!fs::exists(gameProjectsPath))
        {
            if (!fs::create_directory(gameProjectsPath))
            {
                return std::nullopt;
            }
        }

        return gameProjectsPath;
    }

    static std::vector<std::wstring> GetProjectFolders()
    {
        std::vector<std::wstring> result;

        auto rootOpt = FindOrCreateGameProjects();
        if (!rootOpt) return result;

        for (const auto& entry : std::filesystem::directory_iterator(*rootOpt))
        {
            if (entry.is_directory())
            {
                result.push_back(entry.path().filename().wstring());
            }
        }

        return result;
    }
    // 新しいプロジェクトを作成
    static bool CreateNewProjectFolder(const std::wstring& projectName)
    {
        auto rootOpt = FindOrCreateGameProjects();
        if (!rootOpt) return false;

        std::filesystem::path newProjectPath = *rootOpt / projectName;

        if (std::filesystem::exists(newProjectPath))
        {
            // すでに同名のプロジェクトがある
            return false;
        }

        return std::filesystem::create_directory(newProjectPath);
    }
};

