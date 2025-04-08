#pragma once
#include <sstream>
#include <regex>
#include <string>
#include <optional>
#include <fstream>
#include <filesystem>
#include <json.hpp>
#include "Core/Utility/Components.h"
using namespace std::filesystem;
using json = nlohmann::json;
class BaseScene;
class SceneManager;
class ECSManager;
class ObjectContainer;
class ResourceManager;
namespace Cho
{
    enum FileType
    {
        ChoProject,     // プロジェクトファイル
		EngineConfig,   // エンジン設定ファイル
		GameSettings,   // ゲーム設定ファイル
		SceneFile,      // シーンファイル
		ModelFile,      // モデルファイル
		ImageFile,      // 画像ファイル
		SoundFile,      // 音声ファイル
		EffectFile,     // エフェクトファイル
		ScriptFile,     // スクリプトファイル
		Unknown,        // 不明なファイル
    };

    // 戻り値用構造体
    struct ProjectInfo
    {
        std::string name;
        std::string version;
        std::vector<std::wstring> scenes;
    };
    struct WindowConfigInfo
    {
        int width = 1280;
        int height = 720;
        bool fullscreen = false;
    };
    struct EngineConfigInfo
    {
        std::string renderer = "DirectX12";
        WindowConfigInfo window;
        bool vsync = true;
    };
    struct GameSettingsInfo
    {
        std::wstring startScene = L"MainScene.json";
        int frameRate = 60;
        float fixedDeltaTime = 1.0f / 60.0f;
        bool debugMode = false;
    };

    // ComponentsSerializer
    namespace Serialization
    {
        // コンポーネントを保存
        json ToJson(const TransformComponent& t);
        json ToJson(const CameraComponent& c);
        json ToJson(const MeshFilterComponent& m);
        json ToJson(const MeshRendererComponent& r);
		json ToJson(const ScriptComponent& s);
    }

    class FileSystem
    {
    public:
		// プロジェクトフォルダを探す
        static std::optional<std::filesystem::path> FindOrCreateGameProjects();
		// プロジェクトフォルダを取得
        static std::vector<std::wstring> GetProjectFolders();
        // 新しいプロジェクトを作成
        static bool CreateNewProjectFolder(const std::wstring& projectName);
		// プロジェクトファイルを保存
        static bool SaveProjectFile(const std::wstring& projectName, const std::vector<std::wstring>& sceneFiles);
		// プロジェクトファイルを読み込む
        static std::optional<Cho::ProjectInfo> LoadProjectFile(const std::wstring& projectName);
		// エンジン設定ファイルを保存
        static bool SaveEngineConfig(const std::wstring& projectName, const EngineConfigInfo& config);
		// エンジン設定ファイルを読み込む
        static std::optional<EngineConfigInfo> LoadEngineConfig(const std::wstring& projectName);
		// ゲーム設定ファイルを保存
        static bool SaveGameSettings(const std::wstring& projectName, const Cho::GameSettingsInfo& settings);
		// ゲーム設定ファイルを読み込む
        static std::optional<Cho::GameSettingsInfo> LoadGameSettings(const std::wstring& projectName);
		// シーンファイルを保存
        static bool SaveSceneFile(
            const std::wstring& directory,
            BaseScene* scene,
            ObjectContainer* container,
            ECSManager* ecs
        );
		// シーンファイルを読み込む
        static bool LoadSceneFile(
            const std::wstring& filePath,
            SceneManager* sceneManager,
            ObjectContainer* container,
            ECSManager* ecs,
			ResourceManager* resourceManager
        );

        static FileType GetJsonFileType(const std::filesystem::path& path);

        // プロジェクトフォルダを読み込む
        static bool LoadProjectFolder(const std::wstring& projectName, SceneManager* sceneManager, ObjectContainer* container, ECSManager* ecs, ResourceManager* resourceManager);


        static FileType DetectFileType(const nlohmann::json& j)
        {
            std::string type = j.value("fileType", "");
            if (type == "ChoProject") return FileType::ChoProject;
            if (type == "EngineConfig") return FileType::EngineConfig;
            if (type == "GameSettings") return FileType::GameSettings;
            if (type == "SceneFile") return FileType::SceneFile;
			if (type == "ModelFile") return FileType::ModelFile;
			if (type == "ImageFile") return FileType::ImageFile;
			if (type == "SoundFile") return FileType::SoundFile;
			if (type == "EffectFile") return FileType::EffectFile;
			if (type == "ScriptFile") return FileType::ScriptFile;
            return FileType::Unknown;
        }
        // GUID 生成
        static std::string GenerateGUID();
        static std::wstring m_sProjectName;
        class ScriptProject
        {
        public:
            static void GenerateSolutionAndProject();
			static void UpdateVcxproj();
            static void UpdateFilters(const std::string& filterPath);
            static void GenerateScriptFiles(const std::string& scriptName);
            static void LoadProjectPath(const std::wstring& projectName);
            static void LoadScriptDLL();
			static void UnloadScriptDLL();
            static bool BuildScriptDLL();
			static HMODULE GetScriptDLLHandle() { return dllHandle; }

            static std::string slnGUID;
			static std::string projGUID;
			static std::string slnPath;
			static std::string projPath;
            static HMODULE dllHandle;
        };
    };
}

