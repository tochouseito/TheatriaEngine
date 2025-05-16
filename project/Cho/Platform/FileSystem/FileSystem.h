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
using GameParameterVariant = std::variant<int, float, bool, Vector3>;
class BaseScene;
class SceneManager;
class ECSManager;
class ObjectContainer;
class ResourceManager;
class EngineCommand;
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
		GameParameter,  // ゲームパラメータファイル
		PrefabFile,     // プレハブファイル
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
        std::wstring startScene = L"MainScene";
        int frameRate = 60;
        float fixedDeltaTime = 1.0f / 60.0f;
        bool debugMode = false;
    };
    struct FolderNode
    {
		std::filesystem::path folderPath;
		std::vector<std::filesystem::path> files;
		std::vector<FolderNode> children;
    };

    // ComponentsSerializer
    namespace Serialization
    {
        // コンポーネントを保存
        json ToJson(const TransformComponent& t);
        json ToJson(const CameraComponent& c);
        json ToJson(const MeshFilterComponent& m);
        json ToJson(const MeshRendererComponent& r);
		json ToJson(const MaterialComponent& m);
		json ToJson(const ScriptComponent& s);
		json ToJson(const std::vector<LineRendererComponent>& ls);
		json ToJson(const Rigidbody2DComponent& rb);
		json ToJson(const BoxCollider2DComponent& bc);
		json ToJson(const EmitterComponent& e);
		json ToJson(const ParticleComponent& p);
		json ToJson(const UISpriteComponent& ui);
    }
	// ComponentsDeserializer
	namespace Deserialization
	{
		// コンポーネントを読み込み
		void FromJson(const json& j, TransformComponent& t);
		void FromJson(const json& j, CameraComponent& c);
		void FromJson(const json& j, MeshFilterComponent& m);
		void FromJson(const json& j, MeshRendererComponent& r);
		void FromJson(const json& j, MaterialComponent& m);
		void FromJson(const json& j, ScriptComponent& s);
		void FromJson(const json& j, std::vector<LineRendererComponent>& ls);
		void FromJson(const json& j, Rigidbody2DComponent& rb);
		void FromJson(const json& j, BoxCollider2DComponent& bc);
		void FromJson(const json& j, EmitterComponent& e);
		void FromJson(const json& j, ParticleComponent& p);
		void FromJson(const json& j, UISpriteComponent& ui);
	}

    class FileSystem
    {
    public:
        // フォルダ走査
        static void ScanFolder(const path& rootPath,EngineCommand* engineCommand);
        static FolderNode ScanRecursive(const path& path, EngineCommand* engineCommand);
		// フォルダノードをパスで検索
        static FolderNode* FindFolderNodeByPath(FolderNode& node, const std::filesystem::path& target);
        // 拡張子ごとに処理を分ける関数
        static bool ProcessFile(const path& filePath, EngineCommand* engineCommand);
        // ファイル、フォルダを追加、処理
		static bool AddFile(const path& filePath, FolderNode& folderNode,EngineCommand* engineCommand);
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
        static bool LoadGameSettings(const std::wstring& filePath);
		// シーンファイルを保存
        static bool SaveSceneFile(const std::wstring& directory,SceneManager* sceneManager,BaseScene* scene,ObjectContainer* container, ECSManager* ecs );
		// シーンファイルを読み込む
        static bool LoadSceneFile(const std::wstring& filePath,EngineCommand* engineCommand);
        // スクリプトのファイルを保存
		static bool SaveScriptFile(const std::wstring& directory,ResourceManager* resourceManager);
		// スクリプトのファイルを読み込む
		static bool LoadScriptFile(const std::wstring& filePath, EngineCommand* engineCommand);
		// ゲームパラメーターファイルを保存
        static bool SaveGameParameter(const std::wstring& filePath,
            const std::string& group,
            const std::string& item,
            const std::string& dataName,
            const GameParameterVariant& value);
		// ゲームパラメーターファイルを読み込む
        static bool LoadGameParameter(const std::wstring& filePath,
            const std::string& group,
            const std::string& item,
            const std::string& dataName,
            GameParameterVariant& outValue);

        static FileType GetJsonFileType(const std::filesystem::path& path);

        // プロジェクトを保存
		static void SaveProject(SceneManager* sceneManager, ObjectContainer* container, ECSManager* ecs, ResourceManager* resourceManager);
        // プロジェクトフォルダを読み込む
        static bool LoadProjectFolder(const std::wstring& projectName, EngineCommand* engineCommand);

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
			if (type == "GameParameter") return FileType::GameParameter;
			if (type == "PrefabFile") return FileType::PrefabFile;
            return FileType::Unknown;
        }
        // GUID 生成
        static std::string GenerateGUID();
        static std::wstring m_sProjectName;
        static inline FolderNode g_ProjectFiles;
		static inline GameSettingsInfo g_GameSettings;

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
			static std::vector<std::string> GetScriptFiles();
			static HMODULE GetScriptDLLHandle() { return m_DllHandle; }
			static bool LoadPDB(const std::string& dllPath);
			static void UnloadPDB();

            static std::string m_SlnGUID;
			static std::string m_ProjGUID;
			static std::string m_SlnPath;
			static std::string m_ProjPath;
            static HMODULE m_DllHandle;
			static DWORD64 m_PDBBaseAddress;
        };

        class GameBuilder
        {
        public:
			static std::wstring SelectFolderDialog();
			static void CopyFilesToBuildFolder(const std::wstring& folderPath);
            static std::wstring GetEnvVar(const wchar_t* name);

        };
    };
}

