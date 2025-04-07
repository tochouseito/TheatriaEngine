#include "pch.h"
#include "FileSystem.h"
#include "GameCore/GameCore.h"
#include "Resources/ResourceManager/ResourceManager.h"

std::wstring Cho::FileSystem::m_sProjectName = L"";

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
                    if (comps["MeshFilter"].contains("modelID"))
                    {
                        m.modelID = comps["MeshFilter"]["modelID"];
                    }
                    MeshFilterComponent* filter = ecs->AddComponent<MeshFilterComponent>(entity);
					filter->modelID = m.modelID;
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
