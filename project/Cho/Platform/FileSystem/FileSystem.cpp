#include "pch.h"
#include "FileSystem.h"
#include "GameCore/GameCore.h"
#include "Resources/ResourceManager/ResourceManager.h"

std::wstring Cho::FileSystem::m_sProjectName = L"";
// GUID 生成
std::string Cho::FileSystem::ScriptProject::slnGUID = GenerateGUID();
std::string Cho::FileSystem::ScriptProject::projGUID = GenerateGUID();

// プロジェクトフォルダを探す
std::optional<std::filesystem::path> Cho::FileSystem::FindOrCreateGameProjects()
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

// プロジェクトフォルダを取得
std::vector<std::wstring> Cho::FileSystem::GetProjectFolders()
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
bool Cho::FileSystem::CreateNewProjectFolder(const std::wstring& projectName)
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

// プロジェクトファイルを保存
bool Cho::FileSystem::SaveProjectFile(const std::wstring& projectName, const std::vector<std::wstring>& sceneFiles)
{
    std::filesystem::path path = std::filesystem::path(projectName) / "ChoProject.json";

    nlohmann::ordered_json j;
    j["fileType"] = "ChoProject";
    j["name"] = std::filesystem::path(projectName).filename().string(); // プロジェクト名
    j["version"] = "1.0.0";

    // wstring → string 変換（UTF-8）
    std::vector<std::string> sceneNames;
    for (const auto& ws : sceneFiles)
    {
        sceneNames.emplace_back(std::filesystem::path(ws).string()); // or std::wstring_convert
    }

    j["scenes"] = sceneNames;

    try
    {
        std::ofstream file(path.string());
        if (!file.is_open())
        {
            return false;
        }
        file << j.dump(4); // 整形
        file.close();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

// プロジェクトファイルを読み込む
std::optional<Cho::ProjectInfo> Cho::FileSystem::LoadProjectFile(const std::wstring& projectName)
{
    std::filesystem::path path = std::filesystem::path(projectName) / "ChoProject.json";

    try
    {
        std::ifstream file(path.string());
        if (!file.is_open())
        {
            return std::nullopt;
        }

        nlohmann::json j;
        file >> j;

        if (j.contains("fileType") && j["fileType"] != "ChoProject")
        {
            return std::nullopt; // ファイル種別違い
        }

        Cho::ProjectInfo info;
        info.name = j.value("name", "");
        info.version = j.value("version", "");

        for (const auto& sceneName : j["scenes"])
        {
            std::string s = sceneName.get<std::string>();
            info.scenes.push_back(std::filesystem::path(s).wstring());
        }

        return info;
    }
    catch (...)
    {
        return std::nullopt;
    }
}

// エンジン設定ファイルを保存
bool Cho::FileSystem::SaveEngineConfig(const std::wstring& projectName, const EngineConfigInfo& config)
{
    std::filesystem::path path = std::filesystem::path(projectName) / "EngineConfig.json";

    nlohmann::ordered_json j;
    j["fileType"] = "EngineConfig";
    j["renderer"] = config.renderer;
    j["window"] = {
        { "width", config.window.width },
        { "height", config.window.height },
        { "fullscreen", config.window.fullscreen }
    };
    j["vsync"] = config.vsync;

    try
    {
        std::ofstream file(path.string());
        if (!file.is_open()) return false;

        file << j.dump(4);
        file.close();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

// エンジン設定ファイルを読み込む
std::optional<Cho::EngineConfigInfo> Cho::FileSystem::LoadEngineConfig(const std::wstring& projectName)
{
    std::filesystem::path path = std::filesystem::path(projectName) / "EngineConfig.json";

    try
    {
        std::ifstream file(path.string());
        if (!file.is_open()) return std::nullopt;

        nlohmann::json j;
        file >> j;

        if (j.contains("fileType") && j["fileType"] != "EngineConfig")
        {
            return std::nullopt;
        }

        EngineConfigInfo config;
        config.renderer = j.value("renderer", "DirectX12");

        if (j.contains("window"))
        {
            const auto& win = j["window"];
            config.window.width = win.value("width", 1280);
            config.window.height = win.value("height", 720);
            config.window.fullscreen = win.value("fullscreen", false);
        }

        config.vsync = j.value("vsync", true);
        return config;
    }
    catch (...)
    {
        return std::nullopt;
    }
}

// ゲーム設定ファイルを保存
bool Cho::FileSystem::SaveGameSettings(const std::wstring& projectName, const Cho::GameSettingsInfo& settings)
{
    std::filesystem::path path = std::filesystem::path(projectName) / "GameSettings.json";

    nlohmann::ordered_json j;
    j["fileType"] = "GameSettings";
    j["startScene"] = std::filesystem::path(settings.startScene).string();
    j["frameRate"] = settings.frameRate;
    j["fixedDeltaTime"] = settings.fixedDeltaTime;
    j["debugMode"] = settings.debugMode;

    try
    {
        std::ofstream file(path.string());
        if (!file.is_open()) return false;

        file << j.dump(4);
        file.close();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

// ゲーム設定ファイルを読み込む
std::optional<Cho::GameSettingsInfo> Cho::FileSystem::LoadGameSettings(const std::wstring& projectName)
{
    std::filesystem::path path = std::filesystem::path(projectName) / "GameSettings.json";

    try
    {
        std::ifstream file(path.string());
        if (!file.is_open()) return std::nullopt;

        nlohmann::json j;
        file >> j;

        if (j.contains("fileType") && j["fileType"] != "GameSettings")
        {
            return std::nullopt;
        }

        Cho::GameSettingsInfo settings;
        settings.startScene = std::filesystem::path(j.value("startScene", "MainScene.json")).wstring();
        settings.frameRate = j.value("frameRate", 60);
        settings.fixedDeltaTime = j.value("fixedDeltaTime", 1.0f / 60.0f);
        settings.debugMode = j.value("debugMode", false);

        return settings;
    }
    catch (...)
    {
        return std::nullopt;
    }
}

bool Cho::FileSystem::SaveSceneFile(const std::wstring& directory,BaseScene* scene, ObjectContainer* container, ECSManager* ecs)
{
    std::filesystem::path path = std::filesystem::path(directory) / (scene->GetSceneName() + L".json");

    nlohmann::ordered_json j;
    j["fileType"] = "SceneFile";
    j["sceneName"] = std::filesystem::path(scene->GetSceneName()).string();

    // 使用中のオブジェクトを保存
    nlohmann::ordered_json objArray = nlohmann::json::array();
    for (ObjectID& id : scene->GetUseObjects())
    {
        const GameObject* obj = container->GetGameObject(id);
        if (!obj) continue;

        nlohmann::ordered_json objJson;
        objJson["name"] = std::filesystem::path(obj->GetName()).string();
        objJson["type"] = ObjectTypeToWString(obj->GetType());

        // components
        nlohmann::ordered_json comps;

        Entity entity = obj->GetEntity();
        switch (obj->GetType())
        {
        case ObjectType::MeshObject:
            if (const auto* t = ecs->GetComponent<TransformComponent>(entity))
            {
                comps["Transform"] = Cho::Serialization::ToJson(*t);
            }
            if (const auto* m = ecs->GetComponent<MeshFilterComponent>(entity))
            {
                comps["MeshFilter"] = Cho::Serialization::ToJson(*m);
            }
            if (const auto* r = ecs->GetComponent<MeshRendererComponent>(entity))
            {
                comps["MeshRenderer"] = Cho::Serialization::ToJson(*r);
            }
            break;

        case ObjectType::Camera:
            if (const auto* t = ecs->GetComponent<TransformComponent>(entity))
            {
                comps["Transform"] = Cho::Serialization::ToJson(*t);
            }
            if (const auto* c = ecs->GetComponent<CameraComponent>(entity))
            {
                comps["Camera"] = Cho::Serialization::ToJson(*c);
            }
            break;

        default:
            break;
        }

        objJson["components"] = comps;
        objArray.push_back(objJson);
    }
    j["objects"] = objArray;

    // メインカメラ
    if (auto camID = scene->GetMainCameraID())
    {
        if (const GameObject* cam = container->GetGameObject(*camID))
        {
            j["mainCamera"] = std::filesystem::path(cam->GetName()).string();
        }
    }

    try
    {
        std::ofstream file(path.string());
        if (!file.is_open()) return false;
        file << j.dump(4);
        file.close();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool Cho::FileSystem::LoadSceneFile(const std::wstring& filePath, SceneManager* sceneManager, ObjectContainer* container, ECSManager* ecs, ResourceManager* resourceManager)
{
    try
    {
        std::ifstream file(filePath);
        if (!file.is_open()) return false;

        nlohmann::json j;
        file >> j;

        if (j.contains("fileType") && j["fileType"] != "SceneFile")
        {
            return false;
        }
        BaseScene* scene = nullptr;
        // シーン名設定
        if (j.contains("sceneName"))
        {
			std::wstring sceneName = std::filesystem::path(j["sceneName"].get<std::string>()).wstring();
            sceneManager->AddScene(sceneName);
            sceneManager->ChangeSceneRequest(sceneManager->GetSceneID(sceneName));
			scene = sceneManager->GetSceneToName(sceneName);
		}

        // オブジェクト読み込み
        if (j.contains("objects"))
        {
            for (const auto& obj : j["objects"])
            {
                std::wstring name = std::filesystem::path(obj["name"].get<std::string>()).wstring();
                ObjectType type = ObjectTypeFromString(obj["type"].get<std::string>());
                Entity entity = ecs->GenerateEntity();
                container->AddGameObject(entity, name, type);
                GameObject* gameObj = container->GetGameObjectByName(name);
                if (!gameObj) continue;

                scene->AddUseObject(gameObj->GetID());

                if (!obj.contains("components")) continue;
                const auto& comps = obj["components"];

                if (comps.contains("Transform"))
                {
                    TransformComponent t{};
                    auto& jt = comps["Transform"];
                    t.translation = Vector3{ jt["translation"][0], jt["translation"][1], jt["translation"][2] };
                    t.rotation = Quaternion{ jt["rotation"][0], jt["rotation"][1], jt["rotation"][2], jt["rotation"][3] };
                    t.scale = Scale{ jt["scale"][0], jt["scale"][1], jt["scale"][2] };
					t.degrees = Vector3{ jt["degrees"][0], jt["degrees"][1], jt["degrees"][2] };
                    TransformComponent* transform = ecs->AddComponent<TransformComponent>(entity);
					transform->translation = t.translation;
					transform->rotation = t.rotation;
					transform->scale = t.scale;
					transform->degrees = t.degrees;
                    transform->mapID = resourceManager->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
                }

                if (comps.contains("Camera"))
                {
                    CameraComponent c{};
                    auto& jc = comps["Camera"];
                    c.fovAngleY = jc["fovAngleY"];
                    c.aspectRatio = jc["aspectRatio"];
                    c.nearZ = jc["nearZ"];
                    c.farZ = jc["farZ"];
                    CameraComponent* camera = ecs->AddComponent<CameraComponent>(entity);
					camera->fovAngleY = c.fovAngleY;
					camera->aspectRatio = c.aspectRatio;
					camera->nearZ = c.nearZ;
					camera->farZ = c.farZ;
					camera->bufferIndex = resourceManager->CreateConstantBuffer<BUFFER_DATA_VIEWPROJECTION>();
                }

                if (comps.contains("MeshFilter"))
                {
                    MeshFilterComponent m{};
                    if (comps["MeshFilter"].contains("modelName"))
                    {
                        std::string modelNameStr = comps["MeshFilter"]["modelName"].get<std::string>();
                        m.modelName = std::filesystem::path(modelNameStr).wstring(); // UTF-8 → wstring
                    }
                    if (comps["MeshFilter"].contains("modelID"))
                    {
                        m.modelID = comps["MeshFilter"]["modelID"];
                    }
                    MeshFilterComponent* filter = ecs->AddComponent<MeshFilterComponent>(entity);
					TransformComponent* transform = ecs->GetComponent<TransformComponent>(entity);
                    filter->modelName = m.modelName;
                    filter->modelID = resourceManager->GetModelManager()->GetModelDataIndex(filter->modelName,transform->mapID);
                }

                if (comps.contains("MeshRenderer"))
                {
                    MeshRendererComponent r{};
                    r.visible = comps["MeshRenderer"].value("visible", true);
                    MeshRendererComponent* renderer = ecs->AddComponent<MeshRendererComponent>(entity);
					renderer->visible = r.visible;
                }
            }
        }

        // メインカメラ
        if (j.contains("mainCamera"))
        {
            std::wstring camName = std::filesystem::path(j["mainCamera"].get<std::string>()).wstring();
            GameObject* camObj = container->GetGameObjectByName(camName);
            if (camObj)
            {
                scene->SetMainCameraID(camObj->GetID());
            }
        }

        return true;
    }
    catch (...)
    {
        return false;
    }
}

// コンポーネントを保存
json Cho::Serialization::ToJson(const TransformComponent& t)
{
    json j;
    j["translation"] = { t.translation.x, t.translation.y, t.translation.z };
    j["rotation"] = { t.rotation.x, t.rotation.y, t.rotation.z, t.rotation.w };
    j["scale"] = { t.scale.x, t.scale.y, t.scale.z };
	j["degrees"] = { t.degrees.x, t.degrees.y, t.degrees.z };
    return j;
}

json Cho::Serialization::ToJson(const CameraComponent& c)
{
    json j;
    j["fovAngleY"] = c.fovAngleY;
    j["aspectRatio"] = c.aspectRatio;
    j["nearZ"] = c.nearZ;
    j["farZ"] = c.farZ;
    return j;
}

json Cho::Serialization::ToJson(const MeshFilterComponent& m)
{
    json j;
    if (m.modelID.has_value())
    {
        j["modelName"] = std::filesystem::path(m.modelName).string();
        j["modelID"] = m.modelID.value();
    }
    return j;
}

json Cho::Serialization::ToJson(const MeshRendererComponent& r)
{
    json j;
    j["visible"] = r.visible;
    return j;
}

FileType Cho::FileSystem::GetJsonFileType(const std::filesystem::path& path)
{
    try
    {
        std::ifstream file(path);
        if (!file.is_open()) return FileType::Unknown;

        nlohmann::json j;
        file >> j;

        std::string type = j.value("fileType", "");
        if (type == "ChoProject")   return FileType::ChoProject;
        if (type == "EngineConfig") return FileType::EngineConfig;
        if (type == "GameSettings") return FileType::GameSettings;
        if (type == "SceneFile")    return FileType::SceneFile;
        if (type == "ModelFile")    return FileType::ModelFile;
        if (type == "ImageFile")    return FileType::ImageFile;
        if (type == "SoundFile")    return FileType::SoundFile;
        if (type == "EffectFile")   return FileType::EffectFile;
        if (type == "ScriptFile")   return FileType::ScriptFile;

        return FileType::Unknown;
    }
    catch (...)
    {
        return FileType::Unknown;
    }
}

// プロジェクトフォルダを読み込む
bool Cho::FileSystem::LoadProjectFolder(const std::wstring& projectName, SceneManager* sceneManager, ObjectContainer* container, ECSManager* ecs, ResourceManager* resourceManager)
{
    std::filesystem::path projectPath = std::filesystem::path(L"GameProjects") / projectName;

    // プロジェクトファイル類を読み込み
    std::optional<ProjectInfo> projectInfo = LoadProjectFile(projectPath);
    std::optional<EngineConfigInfo> engineConfig = LoadEngineConfig(projectPath);
    std::optional<GameSettingsInfo> gameSettings = LoadGameSettings(projectPath);

    /*if (!projectInfo || !engineConfig || !gameSettings)
    {
    return false;
    }*/

    // 全ファイル走査（サブディレクトリ含む）
    for (const auto& entry : std::filesystem::recursive_directory_iterator(projectPath))
    {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".json") continue;

        FileType type = GetJsonFileType(entry.path());

        switch (type)
        {
        case FileType::SceneFile:
        {
            LoadSceneFile(entry.path(), sceneManager, container, ecs, resourceManager);
        }
        break;
        // ここに ModelFile, EffectFile, ScriptFile など追加
        default:
            break;
        }
    }
    return true;
}

std::string Cho::FileSystem::GenerateGUID()
{
    GUID guid;
    HRESULT hr;
    hr = CoCreateGuid(&guid);
    if (FAILED(hr))
    {
        std::cerr << "Failed to generate GUID. Error code: " << hr << "\n";
        return "";
    }

    std::ostringstream oss;
    oss << std::hex << std::uppercase;
    oss << "{"
        << std::setw(8) << std::setfill('0') << guid.Data1 << "-"
        << std::setw(4) << std::setfill('0') << guid.Data2 << "-"
        << std::setw(4) << std::setfill('0') << guid.Data3 << "-";

    for (int i = 0; i < 2; ++i)
    {
        oss << std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[i]);
    }
    oss << "-";
    for (int i = 2; i < 8; ++i)
    {
        oss << std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[i]);
    }
    oss << "}";

    return oss.str();
}

void Cho::FileSystem::ScriptProject::GenerateSolutionAndProject()
{
    std::string projectNameStr = ConvertString(m_sProjectName);
    // 出力先
    std::string outputPath = "GameProjects/" + projectNameStr;
	// ソリューションファイルパス
    std::string solutionName = outputPath + "/" + projectNameStr + ".sln";
	// vcxprojファイルパス
	std::string vcxprojName = outputPath + "/" + projectNameStr + ".vcxproj";
	// フィルターファイルパス
	std::string filterName = outputPath + "/" + projectNameStr + ".vcxproj.filters";
	// ソリューションファイルの生成
    if (!fs::exists(solutionName))
    {
        std::ofstream slnFile(solutionName, std::ios::trunc);
        slnFile << "Microsoft Visual Studio Solution File, Format Version 12.00\n";
        slnFile << "# Visual Studio Version 17\n";
        slnFile << "VisualStudioVersion = 17.0.31903.59\n";
        slnFile << "MinimumVisualStudioVersion = 10.0.40219.1\n";

        slnFile << "Project(\"" << slnGUID << "\") = \"" << projectNameStr << "\", \"" << projectNameStr << ".vcxproj\", \"" << projGUID << "\"\n";
        slnFile << "EndProject\n";

        // ソリューション構成の追加
        slnFile << "Global\n";
        slnFile << "    GlobalSection(SolutionConfigurationPlatforms) = preSolution\n";
        slnFile << "        Debug|x64 = Debug|x64\n";
        slnFile << "        Release|x64 = Release|x64\n";
        slnFile << "    EndGlobalSection\n";
        slnFile << "    GlobalSection(ProjectConfigurationPlatforms) = postSolution\n";
        slnFile << "        " << projGUID << ".Debug|x64.ActiveCfg = Debug|x64\n";
        slnFile << "        " << projGUID << ".Debug|x64.Build.0 = Debug|x64\n";
        slnFile << "        " << projGUID << ".Release|x64.ActiveCfg = Release|x64\n";
        slnFile << "        " << projGUID << ".Release|x64.Build.0 = Release|x64\n";
        slnFile << "    EndGlobalSection\n";
        slnFile << "    GlobalSection(SolutionProperties) = preSolution\n";
        slnFile << "        HideSolutionNode = FALSE\n";
        slnFile << "    EndGlobalSection\n";
        slnFile << "EndGlobal\n";

        slnFile.close();
        std::cout << "Generated solution file: " << solutionName << "\n";
    } else
    {
        std::cout << "Solution file already exists: " << solutionName << "\n";
    }
	// プロジェクトファイルの生成
	UpdateVcxproj(vcxprojName);
	// フィルターファイルの生成
	UpdateFilters(filterName);
}

void Cho::FileSystem::ScriptProject::UpdateVcxproj(const std::string& vcxprojPath)
{
    std::vector<std::string> scriptFiles;

    fs::path exePath = fs::current_path(); // 実行ファイルがある場所
    // プロジェクトディレクトリ全体を再帰的に探索
    fs::path projectDir = exePath / "GameProjects" / m_sProjectName;

    for (const auto& entry : fs::recursive_directory_iterator(projectDir))
    {
        if (!entry.is_regular_file()) continue;

        const auto& path = entry.path();
        std::string ext = path.extension().string();

        if (ext == ".cpp" || ext == ".h")
        {
            scriptFiles.push_back(path.string());
        }
    }

    // パス設定
    fs::path currentPath = fs::current_path();

    // インクルードディレクトリ
    fs::path systemPath = currentPath / "Cho";
    fs::path mathLibPath = currentPath / "Cho/Externals/ChoMath";
	// スクリプトファイルのパス
	fs::path scriptPath = currentPath / "Cho/GameCore/IScript";

    // ライブラリディレクトリ
    fs::path libraryPath = currentPath / "../generated/outputs/$(Configuration)/";

    // パスの正規化
    systemPath.make_preferred();
    mathLibPath.make_preferred();

    libraryPath.make_preferred();

    std::ofstream vcxFile(vcxprojPath, std::ios::trunc);
    vcxFile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    vcxFile << "<Project DefaultTargets=\"Build\" ToolsVersion=\"17.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";

    // プロジェクト構成の定義
    vcxFile << "  <ItemGroup Label=\"ProjectConfigurations\">\n";
    vcxFile << "    <ProjectConfiguration Include=\"Debug|x64\">\n";
    vcxFile << "      <Configuration>Debug</Configuration>\n";
    vcxFile << "      <Platform>x64</Platform>\n";
    vcxFile << "    </ProjectConfiguration>\n";
    vcxFile << "    <ProjectConfiguration Include=\"Release|x64\">\n";
    vcxFile << "      <Configuration>Release</Configuration>\n";
    vcxFile << "      <Platform>x64</Platform>\n";
    vcxFile << "    </ProjectConfiguration>\n";
    vcxFile << "  </ItemGroup>\n";

    // グローバルプロパティ
    vcxFile << "  <PropertyGroup Label=\"Globals\">\n";
    vcxFile << "    <ProjectGuid>" << projGUID << "</ProjectGuid>\n";
    vcxFile << "    <Keyword>Win32Proj</Keyword>\n";
    vcxFile << "    <RootNamespace>" << ConvertString(m_sProjectName) << "</RootNamespace>\n";
    vcxFile << "  </PropertyGroup>\n";

    vcxFile << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />\n";

    // Debug 構成
    vcxFile << "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\" Label=\"Configuration\">\n";
    vcxFile << "    <ConfigurationType>DynamicLibrary</ConfigurationType>\n";
    vcxFile << "    <UseDebugLibraries>true</UseDebugLibraries>\n";
    vcxFile << "    <PlatformToolset>v143</PlatformToolset>\n";
    vcxFile << "    <CharacterSet>Unicode</CharacterSet>\n";
    vcxFile << "  </PropertyGroup>\n";

    // Release 構成
    vcxFile << "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\" Label=\"Configuration\">\n";
    vcxFile << "    <ConfigurationType>DynamicLibrary</ConfigurationType>\n";
    vcxFile << "    <UseDebugLibraries>false</UseDebugLibraries>\n";
    vcxFile << "    <PlatformToolset>v143</PlatformToolset>\n";
    vcxFile << "    <WholeProgramOptimization>true</WholeProgramOptimization>\n";
    vcxFile << "    <CharacterSet>Unicode</CharacterSet>\n";
    vcxFile << "  </PropertyGroup>\n";

    vcxFile << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />\n";

    // 共通プロパティ
    vcxFile << "  <PropertyGroup>\n";
    vcxFile << "    <OutDir>$(SolutionDir)bin\\$(Configuration)\\</OutDir>\n";
    vcxFile << "    <IntDir>$(SolutionDir)bin\\Intermediate\\$(Configuration)\\</IntDir>\n";
    vcxFile << "    <TargetName>" << ConvertString(m_sProjectName) << "</TargetName>\n";
    vcxFile << "  </PropertyGroup>\n";

    // Debug 用設定
    vcxFile << "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">\n";
    vcxFile << "    <ClCompile>\n";
    vcxFile << "      <WarningLevel>Level3</WarningLevel>\n";
    vcxFile << "      <Optimization>Disabled</Optimization>\n";
    vcxFile << "      <PreprocessorDefinitions>_DEBUG;EXPORT_SCRIPT_API;%(PreprocessorDefinitions)</PreprocessorDefinitions>\n";
    vcxFile << "      <AdditionalIncludeDirectories>" << scriptPath.string() << ";" << mathLibPath.string() << ";" << systemPath.string() << ";" << ";%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\n";
    vcxFile << "      <LanguageStandard>stdcpp20</LanguageStandard>\n";
    vcxFile << "      <AdditionalOptions>/utf-8 %(AdditionalOptions)</AdditionalOptions>\n";
    vcxFile << "      <RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>\n"; // MTd
    vcxFile << "    </ClCompile>\n";
    vcxFile << "    <Link>\n";
    vcxFile << "      <SubSystem>Windows</SubSystem>\n";
    vcxFile << "      <GenerateDebugInformation>false</GenerateDebugInformation>\n";
    vcxFile << "      <AdditionalDependencies>ChoMath.lib;%(AdditionalDependencies)</AdditionalDependencies>\n";
    vcxFile << "      <AdditionalLibraryDirectories>" << libraryPath.string() << ";%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>\n";
    vcxFile << "    </Link>\n";
    vcxFile << "  </ItemDefinitionGroup>\n";

    // Release 用設定
    vcxFile << "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">\n";
    vcxFile << "    <ClCompile>\n";
    vcxFile << "      <WarningLevel>Level3</WarningLevel>\n";
    vcxFile << "      <Optimization>MaxSpeed</Optimization>\n";
    vcxFile << "      <PreprocessorDefinitions>NDEBUG;EXPORT_SCRIPT_API;%(PreprocessorDefinitions)</PreprocessorDefinitions>\n";
    vcxFile << "      <AdditionalIncludeDirectories>" << scriptPath.string() << ";" << mathLibPath.string() << ";" << systemPath.string() << ";" << ";%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\n";
    vcxFile << "      <LanguageStandard>stdcpp20</LanguageStandard>\n";
    vcxFile << "      <AdditionalOptions>/utf-8 %(AdditionalOptions)</AdditionalOptions>\n";
    vcxFile << "      <RuntimeLibrary>MultiThreaded</RuntimeLibrary>\n"; // MT
    vcxFile << "    </ClCompile>\n";
    vcxFile << "    <Link>\n";
    vcxFile << "      <SubSystem>Windows</SubSystem>\n";
    vcxFile << "      <EnableCOMDATFolding>true</EnableCOMDATFolding>\n";
    vcxFile << "      <OptimizeReferences>true</OptimizeReferences>\n";
    vcxFile << "      <GenerateDebugInformation>false</GenerateDebugInformation>\n";
    vcxFile << "      <AdditionalDependencies>ChoMath.lib;%(AdditionalDependencies)</AdditionalDependencies>\n";
    vcxFile << "      <AdditionalLibraryDirectories>" << libraryPath.string() << ";%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>\n";
    vcxFile << "    </Link>\n";
    vcxFile << "  </ItemDefinitionGroup>\n";

    // ソースファイルの登録
    vcxFile << "  <ItemGroup>\n";
    for (const auto& file : scriptFiles)
    {
        if (file.ends_with(".cpp"))
        {
            vcxFile << "    <ClCompile Include=\"" << file << "\" />\n";
        }
    }
    vcxFile << "  </ItemGroup>\n";

    // ヘッダーファイルの登録
    vcxFile << "  <ItemGroup>\n";
    for (const auto& file : scriptFiles)
    {
        if (file.ends_with(".h"))
        {
            vcxFile << "    <ClInclude Include=\"" << file << "\" />\n";
        }
    }
    vcxFile << "  </ItemGroup>\n";

    vcxFile << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />\n";
    vcxFile << "</Project>\n";
    vcxFile.close();
}

void Cho::FileSystem::ScriptProject::UpdateFilters(const std::string& filterPath)
{
    std::vector<std::string> scriptFiles;

    fs::path exePath = fs::current_path(); // 実行ファイルがある場所
    // プロジェクトディレクトリ全体を再帰的に探索
    fs::path projectDir = exePath / "GameProjects" / m_sProjectName;
    for (const auto& entry : fs::recursive_directory_iterator(projectDir))
    {
        if (!entry.is_regular_file()) continue;

        const auto& path = entry.path();
        std::string ext = path.extension().string();
        if (ext == ".cpp" || ext == ".h")
        {
            scriptFiles.push_back(path.string());
        }
    }

    std::ofstream filtersFile(filterPath, std::ios::trunc);
    filtersFile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    filtersFile << "<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";
    filtersFile << "  <ItemGroup>\n";

    for (const auto& file : scriptFiles)
    {
        if (file.ends_with(".cpp"))
        {
            filtersFile << "    <ClCompile Include=\"" << file << "\">\n";
            filtersFile << "      <Filter>Source Files</Filter>\n";
            filtersFile << "    </ClCompile>\n";
        } else if (file.ends_with(".h"))
        {
            filtersFile << "    <ClInclude Include=\"" << file << "\">\n";
            filtersFile << "      <Filter>Header Files</Filter>\n";
            filtersFile << "    </ClInclude>\n";
        }
    }

    filtersFile << "  </ItemGroup>\n";
    filtersFile << "  <ItemGroup>\n";
    filtersFile << "    <Filter Include=\"Source Files\">\n";
    filtersFile << "      <UniqueIdentifier>{E2E5D8EC-87BD-45C3-8B23-4E6F9D5E8DB9}</UniqueIdentifier>\n";
    filtersFile << "    </Filter>\n";
    filtersFile << "    <Filter Include=\"Header Files\">\n";
    filtersFile << "      <UniqueIdentifier>{B6B78A4F-BEAF-40F2-927B-73A40EC5C5B4}</UniqueIdentifier>\n";
    filtersFile << "    </Filter>\n";
    filtersFile << "  </ItemGroup>\n";
    filtersFile << "</Project>\n";

    filtersFile.close();
}
