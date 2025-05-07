#include "pch.h"
#include "FileSystem.h"
#include "GameCore/GameCore.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "EngineCommand/EngineCommand.h"
#include <cstdlib>
#include "Core/ChoLog/ChoLog.h"
#include <windows.h>
#include <winsock.h>
#include <wingdi.h>
#include <shellapi.h>
#include <ole2.h>
#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp.lib")
using namespace Cho;

std::wstring Cho::FileSystem::m_sProjectName = L"";
// GUID 生成
std::string Cho::FileSystem::ScriptProject::m_SlnGUID = GenerateGUID();
std::string Cho::FileSystem::ScriptProject::m_ProjGUID = GenerateGUID();
std::string Cho::FileSystem::ScriptProject::m_SlnPath = "";
std::string Cho::FileSystem::ScriptProject::m_ProjPath = "";
HMODULE Cho::FileSystem::ScriptProject::m_DllHandle = nullptr;
DWORD64 Cho::FileSystem::ScriptProject::m_PDBBaseAddress = 0;

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
        const GameObject& obj = container->GetGameObject(id);
        if (!obj.IsActive()) continue;

        nlohmann::ordered_json objJson;
        objJson["name"] = std::filesystem::path(obj.GetName()).string();
        objJson["type"] = ObjectTypeToWString(obj.GetType());

        // components
        nlohmann::ordered_json comps;

        Entity entity = obj.GetEntity();
        // マルチコンポーネント存在確認用
		std::vector<LineRendererComponent>* lineRenderers;
        switch (obj.GetType())
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
			if (const auto* m = ecs->GetComponent<MaterialComponent>(entity))
			{
				comps["Material"] = Cho::Serialization::ToJson(*m);
			}
			if (const auto* s = ecs->GetComponent<ScriptComponent>(entity))
			{
				comps["Script"] = Cho::Serialization::ToJson(*s);
			}
			lineRenderers = ecs->GetAllComponents<LineRendererComponent>(entity);
            if (lineRenderers)
            {
				comps["LineRenderer"] = Cho::Serialization::ToJson(*lineRenderers);
            }
			if (const auto* rb = ecs->GetComponent<Rigidbody2DComponent>(entity))
			{
				comps["Rigidbody2D"] = Cho::Serialization::ToJson(*rb);
			}
			if (const auto* bc = ecs->GetComponent<BoxCollider2DComponent>(entity))
			{
				comps["BoxCollider2D"] = Cho::Serialization::ToJson(*bc);
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
			if (const auto* s = ecs->GetComponent<ScriptComponent>(entity))
			{
				comps["Script"] = Cho::Serialization::ToJson(*s);
			}
            lineRenderers = ecs->GetAllComponents<LineRendererComponent>(entity);
			if (lineRenderers)
			{
				comps["LineRenderer"] = Cho::Serialization::ToJson(*lineRenderers);
			}
            break;
		case ObjectType::ParticleSystem:
			if (const auto* t = ecs->GetComponent<TransformComponent>(entity))
			{
				comps["Transform"] = Cho::Serialization::ToJson(*t);
			}
			if (const auto* p = ecs->GetComponent<ParticleComponent>(entity))
			{
				comps["Particle"] = Cho::Serialization::ToJson(*p);
			}
			if (const auto* e = ecs->GetComponent<EmitterComponent>(entity))
			{
				comps["Emitter"] = Cho::Serialization::ToJson(*e);
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
        const GameObject& cam = container->GetGameObject(*camID);
        if (cam.IsActive())
        {
            j["mainCamera"] = std::filesystem::path(cam.GetName()).string();
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

bool Cho::FileSystem::LoadSceneFile(const std::wstring& filePath, EngineCommand* engineCommand)
{
    if (!engineCommand) { Log::Write(LogLevel::Assert, "EngineCommand is nullptr"); }
	SceneManager* sceneManager = engineCommand->GetGameCore()->GetSceneManager();
    ObjectContainer* container = engineCommand->GetGameCore()->GetObjectContainer();
	ECSManager* ecs = engineCommand->GetGameCore()->GetECSManager();
	ResourceManager* resourceManager = engineCommand->GetResourceManager();
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
                ObjectID id = container->AddGameObject(entity, name, type);
                GameObject& gameObj = container->GetGameObjectByName(name);
                if (!gameObj.IsActive()) continue;

                scene->AddUseObject(gameObj.GetID().value());

                if (!obj.contains("components")) continue;
                const auto& comps = obj["components"];

                if (comps.contains("Transform"))
                {
                    TransformComponent t{};
                    auto& jt = comps["Transform"];
					// TransformComponentの読み込み
					Deserialization::FromJson(jt,t);
                    /*t.translation = Vector3{ jt["translation"][0], jt["translation"][1], jt["translation"][2] };
                    t.rotation = Quaternion{ jt["rotation"][0], jt["rotation"][1], jt["rotation"][2], jt["rotation"][3] };
                    t.scale = Scale{ jt["scale"][0], jt["scale"][1], jt["scale"][2] };
					t.degrees = Vector3{ jt["degrees"][0], jt["degrees"][1], jt["degrees"][2] };*/
                    TransformComponent* transform = ecs->AddComponent<TransformComponent>(entity);
					/*transform->translation = t.translation;
					transform->rotation = t.rotation;
					transform->scale = t.scale;
					transform->degrees = t.degrees;*/
                    *transform = t;
                    transform->mapID = resourceManager->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
                }

                if (comps.contains("Camera"))
                {
                    CameraComponent c{};
                    auto& jc = comps["Camera"];
					// CameraComponentの読み込み
					Deserialization::FromJson(jc, c);
                    /*c.fovAngleY = jc["fovAngleY"];
                    c.aspectRatio = jc["aspectRatio"];
                    c.nearZ = jc["nearZ"];
                    c.farZ = jc["farZ"];*/
                    CameraComponent* camera = ecs->AddComponent<CameraComponent>(entity);
					/*camera->fovAngleY = c.fovAngleY;
					camera->aspectRatio = c.aspectRatio;
					camera->nearZ = c.nearZ;
					camera->farZ = c.farZ;*/
					*camera = c;
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
					// モデル名からIDを取得
                    filter->modelID = resourceManager->GetModelManager()->GetModelDataIndex(filter->modelName);
					// モデルのUseListに登録
                    resourceManager->GetModelManager()->RegisterModelUseList(filter->modelID.value(), transform->mapID.value());
                }

                if (comps.contains("MeshRenderer"))
                {
                    MeshRendererComponent r{};
                    r.visible = comps["MeshRenderer"].value("visible", true);
                    MeshRendererComponent* renderer = ecs->AddComponent<MeshRendererComponent>(entity);
					renderer->visible = r.visible;
                }

				if (comps.contains("Material"))
				{
					MaterialComponent m{};
					auto& jm = comps["Material"];
					// MaterialComponentの読み込み
					Deserialization::FromJson(jm, m);
					MaterialComponent* material = ecs->AddComponent<MaterialComponent>(entity);
					*material = m;
					TransformComponent* transform = ecs->GetComponent<TransformComponent>(entity);
					transform->materialID = resourceManager->GetIntegrationData(IntegrationDataType::Material)->GetMapID();
					material->mapID = transform->materialID;
				}

                if (comps.contains("Script"))
                {
                    ScriptComponent s{};
                    if (comps["Script"].contains("scriptName"))
                    {
                        std::string scriptNameStr = comps["Script"]["scriptName"].get<std::string>();
                        s.scriptName = scriptNameStr;
						//s.objectID = comps["Script"]["objectID"];
                        //s.entity = entity;
                        ScriptComponent* script = ecs->AddComponent<ScriptComponent>(entity);
                        script->scriptName = s.scriptName;
                        script->objectID = id;
                    }
                }

                // LineRenderer（マルチコンポーネント）
                if (comps.contains("LineRenderer"))
                {
                    for (const auto& lineJson : comps["LineRenderer"])
                    {
                        LineRendererComponent l{};
                        auto& start = lineJson["start"];
                        auto& end = lineJson["end"];
                        auto& color = lineJson["color"];

                        l.line.start = Vector3{ start[0], start[1], start[2] };
                        l.line.end = Vector3{ end[0], end[1], end[2] };
                        l.line.color = Color{ color[0], color[1], color[2], color[3] };

						// LineRendererComponentを追加
						uint32 mapID = resourceManager->GetIntegrationData(IntegrationDataType::Line)->GetMapID();
                        LineRendererComponent* comp = ecs->AddComponent<LineRendererComponent>(entity);
						comp->line.start = l.line.start;
						comp->line.end = l.line.end;
						comp->line.color = l.line.color;
						comp->mapID = mapID;
                    }
                }

                // Rigidbody2D
                if (comps.contains("Rigidbody2D"))
                {
                    Rigidbody2DComponent r{};
                    auto& jr = comps["Rigidbody2D"];
                    r.isKinematic = jr.value("isKinematic", false);
                    r.gravityScale = jr.value("gravityScale", 1.0f);
                    r.mass = jr.value("mass", 1.0f);
                    r.bodyType = static_cast<b2BodyType>(jr.value("bodyType", 2)); // default: b2_dynamicBody
                    r.fixedRotation = jr.value("fixedRotation", false);
					r.isCollisionStay = false;
					r.runtimeBody = nullptr;
					r.world = nullptr;
					r.otherObjectID = std::nullopt;
                    r.selfObjectID = id;

                    Rigidbody2DComponent* rb = ecs->AddComponent<Rigidbody2DComponent>(entity);
					rb->isKinematic = r.isKinematic;
					rb->gravityScale = r.gravityScale;
					rb->mass = r.mass;
					rb->bodyType = r.bodyType;
					rb->fixedRotation = r.fixedRotation;
					rb->isCollisionStay = r.isCollisionStay;
					rb->runtimeBody = r.runtimeBody;
					rb->world = r.world;
					rb->otherObjectID = r.otherObjectID;
					rb->selfObjectID = r.selfObjectID;
                }

                // BoxCollider2D
                if (comps.contains("BoxCollider2D"))
                {
                    BoxCollider2DComponent b{};
                    auto& jb = comps["BoxCollider2D"];
                    b.offsetX = jb.value("offsetX", 0.0f);
                    b.offsetY = jb.value("offsetY", 0.0f);
                    b.width = jb.value("width", 1.0f);
                    b.height = jb.value("height", 1.0f);
                    b.density = jb.value("density", 1.0f);
                    b.friction = jb.value("friction", 0.5f);
                    b.restitution = jb.value("restitution", 0.0f);

                    BoxCollider2DComponent* box = ecs->AddComponent<BoxCollider2DComponent>(entity);
					box->offsetX = b.offsetX;
					box->offsetY = b.offsetY;
					box->width = b.width;
					box->height = b.height;
					box->density = b.density;
					box->friction = b.friction;
					box->restitution = b.restitution;
                }

                // Emitter
                if (comps.contains("Emitter"))
                {
                    EmitterComponent e{};
                    auto& je = comps["Emitter"];
					Deserialization::FromJson(je, e);
					EmitterComponent* emitter = ecs->AddComponent<EmitterComponent>(entity);
					*emitter = e;
					emitter->bufferIndex = resourceManager->CreateConstantBuffer<BUFFER_DATA_EMITTER>();
                }

				// Particle
                if (comps.contains("Particle"))
                {
                    ParticleComponent p{};
                    auto& jp = comps["Particle"];
                    Deserialization::FromJson(jp, p);
                    ParticleComponent* particle = ecs->AddComponent<ParticleComponent>(entity);
                    *particle = p;
                    // パーティクル
                    particle->bufferIndex = resourceManager->CreateRWStructuredBuffer<BUFFER_DATA_PARTICLE>(particle->count);
                    // PerFrame
                    particle->perFrameBufferIndex = resourceManager->CreateConstantBuffer<BUFFER_DATA_PARTICLE_PERFRAME>();
                    // FreeListIndex
                    //particle->freeListIndexBufferIndex = resourceManager->CreateRWStructuredBuffer<int32_t>(1);
                    // FreeList
                    particle->freeListBufferIndex = resourceManager->CreateRWStructuredBuffer<uint32_t>(particle->count, true);
                }
            }
        }

        // メインカメラ
        if (j.contains("mainCamera"))
        {
            std::wstring camName = std::filesystem::path(j["mainCamera"].get<std::string>()).wstring();
            GameObject& camObj = container->GetGameObjectByName(camName);
            if (camObj.IsActive())
            {
                scene->SetMainCameraID(camObj.GetID().value());
            }
        }

        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool Cho::FileSystem::SaveScriptFile(const std::wstring& directory, ResourceManager* resourceManager)
{
    ScriptContainer* scriptContainer = resourceManager->GetScriptContainer();
    if (!scriptContainer) return false;

    nlohmann::ordered_json j;
    j["fileType"] = "ScriptFile";

    std::vector<std::string> scriptList;
    for (size_t i = 0; i < scriptContainer->GetScriptCount(); ++i)
    {
        std::optional<std::string> data = scriptContainer->GetScriptDataByID(static_cast<uint32_t>(i));
        if (data)
        {
            scriptList.push_back(data.value());
        }
    }
    j["scripts"] = scriptList;

    std::filesystem::path path = std::filesystem::path(directory) / "ScriptData.json";
    try
    {
        std::ofstream file(path);
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

bool Cho::FileSystem::LoadScriptFile(const std::wstring& filePath, EngineCommand* engineCommand)
{
    if (!engineCommand) { Log::Write(LogLevel::Assert, "EngineCommand is nullptr"); }
	ScriptContainer* scriptContainer = engineCommand->GetResourceManager()->GetScriptContainer();
    if (!scriptContainer) { return false; }

    try
    {
        std::ifstream file(filePath);
        if (!file.is_open()) { return false; }

        nlohmann::json j;
        file >> j;

        if (j.contains("fileType") && j["fileType"] != "ScriptFile")
        {
            return false;
        }

        std::vector<std::string> availableScriptFiles = ScriptProject::GetScriptFiles();

        if (j.contains("scripts"))
        {
            for (const auto& scriptNameJson : j["scripts"])
            {
                std::string scriptName = scriptNameJson.get<std::string>();

                // スクリプトファイル群の中に存在しているかをチェック
                auto found = std::find_if(availableScriptFiles.begin(), availableScriptFiles.end(),
                    [&](const std::string& file) {
                        return std::filesystem::path(file).stem().string() == scriptName;
                    });

                if (found != availableScriptFiles.end())
                {
                    scriptContainer->AddScriptData(scriptName);
                }
            }
        }

        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool Cho::FileSystem::SaveGameParameter(const std::wstring& filePath, const std::string& group, const std::string& item, const std::string& dataName, const GameParameterVariant& value)
{
    json root;

    // 既存のファイルを開く（存在しなければ新規）
    std::wstring path = L"GameProjects/" + m_sProjectName + L"/" + filePath + L".json";
    std::ifstream ifs(path);
    if (ifs.is_open())
    {
        try
        {
            ifs >> root;
        }
        catch (...)
        {
            return false;
        }
        ifs.close();
    }
    // ファイルタイプの確認 or 設定
    root["fileType"] = "GameParameter";
    // グループ → アイテム → データ名 に設定
    json& target = root[group][item][dataName];
    if (std::holds_alternative<int>(value))
    {
        target = std::get<int>(value);
    } else if (std::holds_alternative<float>(value))
    {
        target = std::get<float>(value);
    } else if (std::holds_alternative<bool>(value))
    {
        target = std::get<bool>(value);
    } else if (std::holds_alternative<Vector3>(value))
    {
        const Vector3& v = std::get<Vector3>(value);
        target = { v.x, v.y, v.z };
    }
    // 書き込み
    std::ofstream ofs(path);
    if (!ofs.is_open()) return false;
    ofs << std::setw(4) << root << std::endl;
    return true;
}

bool Cho::FileSystem::LoadGameParameter(const std::wstring& filePath, const std::string& group, const std::string& item, const std::string& dataName, GameParameterVariant& outValue)
{
    json root;
	// ファイルを開く
    std::wstring path = L"GameProjects/" + m_sProjectName + L"/" + filePath + L".json";
    std::ifstream ifs(path);
    if (!ifs.is_open()) { return false; }
    try
    {
        ifs >> root;
    }
    catch (...)
    {
        return false;
    }
    if (root.value("fileType", "") != "GameParameter") { return false; }
    if (!root.contains(group) || !root[group].contains(item) || !root[group][item].contains(dataName))
    {
        return false;
    }
    const json& val = root[group][item][dataName];
    if (val.is_number_integer())
    {
        outValue = val.get<int>();
    } else if (val.is_number_float())
    {
        outValue = val.get<float>();
    } else if (val.is_boolean())
    {
        outValue = val.get<bool>();
    } else if (val.is_array() && val.size() == 3 && val[0].is_number()) 
    {
        outValue = Vector3{ val[0].get<float>(), val[1].get<float>(), val[2].get<float>() };
    } else{
        return false;
    }
    return true;
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

json Cho::Serialization::ToJson(const MaterialComponent& m)
{
    json j;
	j["Color"] = { m.color.r, m.color.g, m.color.b, m.color.a };
	j["enableLighting"] = m.enableLighting;
	j["enableTexture"] = m.enableTexture;
	j["shininess"] = m.shininess;
	std::string textureName = ConvertString(m.textureName);
	j["textureName"] = textureName;
	return j;
}

json Cho::Serialization::ToJson(const ScriptComponent& s)
{
	json j;
	j["scriptName"] = s.scriptName;
	/*if (s.entity.has_value())
	{
		j["entity"] = s.entity.value();
	}*/
	if (s.objectID.has_value())
	{
		j["objectID"] = s.objectID.value();
	}
	return j;
}

json Cho::Serialization::ToJson(const std::vector<LineRendererComponent>& ls)
{
    json jArray = json::array();
    for (const auto& l : ls)
    {
        json j;
        j["start"] = { l.line.start.x, l.line.start.y, l.line.start.z };
        j["end"] = { l.line.end.x, l.line.end.y, l.line.end.z };
        j["color"] = { l.line.color.r, l.line.color.g, l.line.color.b, l.line.color.a };
        jArray.push_back(j);
    }
    return jArray;
}

json Cho::Serialization::ToJson(const Rigidbody2DComponent& rb)
{
	json j;
	j["isKinematic"] = rb.isKinematic;
	j["gravityScale"] = rb.gravityScale;
	j["mass"] = rb.mass;
	j["bodyType"] = static_cast<int>(rb.bodyType);
	j["fixedRotation"] = rb.fixedRotation;
    if (rb.selfObjectID.has_value())
    {
        j["selfObjectID"] = rb.selfObjectID.value();
    }
	return j;
}

json Cho::Serialization::ToJson(const BoxCollider2DComponent& bc)
{
	json j;
	j["offsetX"] = bc.offsetX;
	j["offsetY"] = bc.offsetY;
	j["width"] = bc.width;
	j["height"] = bc.height;
	j["density"] = bc.density;
	j["friction"] = bc.friction;   
	j["restitution"] = bc.restitution;
	return j;
}

json Cho::Serialization::ToJson(const EmitterComponent& e)
{
	json j;
	j["position"] = { e.position.x, e.position.y, e.position.z };
	j["radius"] = e.radius;
	j["count"] = e.count;
	j["frequency"] = e.frequency;
	j["frequencyTime"] = e.frequencyTime;
	return j;
}

json Cho::Serialization::ToJson(const ParticleComponent& p)
{
    json j;
	j["count"] = p.count;
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

void Cho::FileSystem::SaveProject(SceneManager* sceneManager, ObjectContainer* container, ECSManager* ecs, ResourceManager* resourceManager)
{
    if (m_sProjectName.empty()) { return; }
    // セーブ
    for (auto& scene : sceneManager->GetScenes().GetVector())
    {
        // シーンファイルを保存
        Cho::FileSystem::SaveSceneFile(
            L"GameProjects/" + m_sProjectName,
            scene.get(),
            container,
            ecs
        );
    }
	Cho::FileSystem::SaveScriptFile(
		L"GameProjects/" + m_sProjectName,
		resourceManager
	);
}

// プロジェクトフォルダを読み込む
bool Cho::FileSystem::LoadProjectFolder(const std::wstring& projectName, EngineCommand* engineCommand)
{
	m_sProjectName = projectName;
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
    ScanFolder(projectPath,engineCommand);
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
    m_SlnPath = outputPath + "/" + projectNameStr + ".sln";
	// vcxprojファイルパス
	m_ProjPath = outputPath + "/" + projectNameStr + ".vcxproj";
	// フィルターファイルパス
	std::string filterName = outputPath + "/" + projectNameStr + ".vcxproj.filters";
    // プロジェクトファイルの生成
    UpdateVcxproj();
    // フィルターファイルの生成
    UpdateFilters(filterName);
	// ソリューションファイルの生成
    if (!fs::exists(m_SlnPath))
    {
        std::ofstream slnFile(m_SlnPath, std::ios::trunc);
        slnFile << "Microsoft Visual Studio Solution File, Format Version 12.00\n";
        slnFile << "# Visual Studio Version 17\n";
        slnFile << "VisualStudioVersion = 17.0.31903.59\n";
        slnFile << "MinimumVisualStudioVersion = 10.0.40219.1\n";

        slnFile << "Project(\"" << m_SlnGUID << "\") = \"" << projectNameStr << "\", \"" << projectNameStr << ".vcxproj\", \"" << m_ProjGUID << "\"\n";
        slnFile << "EndProject\n";

        // ソリューション構成の追加
        slnFile << "Global\n";
        slnFile << "    GlobalSection(SolutionConfigurationPlatforms) = preSolution\n";
        slnFile << "        Debug|x64 = Debug|x64\n";
        slnFile << "        Release|x64 = Release|x64\n";
        slnFile << "    EndGlobalSection\n";
        slnFile << "    GlobalSection(ProjectConfigurationPlatforms) = postSolution\n";
        slnFile << "        " << m_ProjGUID << ".Debug|x64.ActiveCfg = Debug|x64\n";
        slnFile << "        " << m_ProjGUID << ".Debug|x64.Build.0 = Debug|x64\n";
        slnFile << "        " << m_ProjGUID << ".Release|x64.ActiveCfg = Release|x64\n";
        slnFile << "        " << m_ProjGUID << ".Release|x64.Build.0 = Release|x64\n";
        slnFile << "    EndGlobalSection\n";
        slnFile << "    GlobalSection(SolutionProperties) = preSolution\n";
        slnFile << "        HideSolutionNode = FALSE\n";
        slnFile << "    EndGlobalSection\n";
        slnFile << "EndGlobal\n";

        slnFile.close();
        std::cout << "Generated solution file: " << m_SlnPath << "\n";
    } else
    {
        std::cout << "Solution file already exists: " << m_SlnPath << "\n";
    }
}

void Cho::FileSystem::ScriptProject::UpdateVcxproj()
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
            fs::path relativePath = fs::relative(path, projectDir); // 相対パス化
            relativePath.make_preferred(); // Windowsの \ 区切りに変換
            scriptFiles.push_back(relativePath.string()); // 相対パスを保存
        }
    }

    // パス設定
    fs::path currentPath = fs::current_path();

    // インクルードディレクトリ
	// スクリプトファイルのパス
    fs::path includeBase = fs::relative(currentPath, projectDir);
    fs::path systemPath = includeBase / "Cho";
    fs::path mathLibPath = includeBase / "Cho/Externals/ChoMath";
    fs::path scriptPath = includeBase / "Cho/GameCore/IScript";
	fs::path contextPath = includeBase / "Cho/GameCore/ScriptAPI";

    // ライブラリディレクトリ
    //fs::path libraryPath = currentPath / "../generated/outputs/$(Configuration)/";
    //fs::path libraryPath2 = includeBase / "../../";
    fs::path libraryPath = "$(ProjectDir)../../../../generated/outputs/$(Configuration)/";
    fs::path libraryPath2 = "$(ProjectDir)../../";

    // パスの正規化
    systemPath.make_preferred();
    mathLibPath.make_preferred();
	scriptPath.make_preferred();
	contextPath.make_preferred();

    libraryPath.make_preferred();
	libraryPath2.make_preferred();

    std::ofstream vcxFile(m_ProjPath, std::ios::trunc);
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
    vcxFile << "    <ProjectGuid>" << m_ProjGUID << "</ProjectGuid>\n";
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
    vcxFile << "    <OutDir>$(SolutionDir)bin\\</OutDir>\n";
    vcxFile << "    <IntDir>$(SolutionDir)obj\\</IntDir>\n";
    vcxFile << "    <TargetName>" << ConvertString(m_sProjectName) << "</TargetName>\n";
    vcxFile << "  </PropertyGroup>\n";

    // Debug 用設定
    vcxFile << "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">\n";
    vcxFile << "    <ClCompile>\n";
    vcxFile << "      <WarningLevel>Level3</WarningLevel>\n";
    vcxFile << "      <Optimization>Disabled</Optimization>\n";
    vcxFile << "      <PreprocessorDefinitions>_DEBUG;EXPORT_SCRIPT_API;%(PreprocessorDefinitions)</PreprocessorDefinitions>\n";
    vcxFile << "      <AdditionalIncludeDirectories>" << contextPath.string() << ";" << scriptPath.string() << ";" << mathLibPath.string() << ";" << systemPath.string() << ";" << ";%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\n";
    vcxFile << "      <LanguageStandard>stdcpp20</LanguageStandard>\n";
    vcxFile << "      <AdditionalOptions>/utf-8 %(AdditionalOptions)</AdditionalOptions>\n";
    vcxFile << "      <RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>\n"; // MTd
    vcxFile << "    </ClCompile>\n";
    vcxFile << "    <Link>\n";
    vcxFile << "      <SubSystem>Windows</SubSystem>\n";
    vcxFile << "      <GenerateDebugInformation>true</GenerateDebugInformation>\n";
    vcxFile << "      <AdditionalDependencies>"<<"ChoMath.lib"<<";"<< "ChoEngine.lib" << ";" << "%(AdditionalDependencies)</AdditionalDependencies>\n";
    vcxFile << "      <AdditionalLibraryDirectories>" << libraryPath.string() << ";" << libraryPath2.string() << ";%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>\n";
    vcxFile << "    </Link>\n";
    vcxFile << "  </ItemDefinitionGroup>\n";

    // Release 用設定
    vcxFile << "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">\n";
    vcxFile << "    <ClCompile>\n";
    vcxFile << "      <WarningLevel>Level3</WarningLevel>\n";
    //vcxFile << "      <Optimization>MaxSpeed</Optimization>\n";
    vcxFile << "      <Optimization>Disabled</Optimization>\n";
    vcxFile << "      <PreprocessorDefinitions>NDEBUG;EXPORT_SCRIPT_API;%(PreprocessorDefinitions)</PreprocessorDefinitions>\n";
    vcxFile << "      <AdditionalIncludeDirectories>" << contextPath.string() << ";" << scriptPath.string() << ";" << mathLibPath.string() << ";" << systemPath.string() << ";" << ";%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\n";
    vcxFile << "      <LanguageStandard>stdcpp20</LanguageStandard>\n";
    vcxFile << "      <AdditionalOptions>/utf-8 %(AdditionalOptions)</AdditionalOptions>\n";
    vcxFile << "      <RuntimeLibrary>MultiThreaded</RuntimeLibrary>\n"; // MT
    vcxFile << "    </ClCompile>\n";
    vcxFile << "    <Link>\n";
    vcxFile << "      <SubSystem>Windows</SubSystem>\n";
    vcxFile << "      <EnableCOMDATFolding>true</EnableCOMDATFolding>\n";
    vcxFile << "      <OptimizeReferences>true</OptimizeReferences>\n";
    vcxFile << "      <GenerateDebugInformation>true</GenerateDebugInformation>\n";
    vcxFile << "      <AdditionalDependencies>" << "ChoMath.lib" << ";" << "ChoEngine.lib" << ";" << "%(AdditionalDependencies)</AdditionalDependencies>\n";
    vcxFile << "      <AdditionalLibraryDirectories>" << libraryPath.string() << ";" << libraryPath2.string() << ";%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>\n";
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

    // scriptFiles には相対パスを格納するように修正
    for (const auto& entry : fs::recursive_directory_iterator(projectDir))
    {
        if (!entry.is_regular_file()) continue;

        const auto& path = entry.path();
        std::string ext = path.extension().string();

        if (ext == ".cpp" || ext == ".h")
        {
            // .vcxproj があるディレクトリからの相対パスを取得
            std::string relativePath = fs::relative(path, projectDir).make_preferred().string();
            scriptFiles.push_back(relativePath);
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

void Cho::FileSystem::ScriptProject::GenerateScriptFiles(const std::string& scriptName)
{
    std::filesystem::path outputDir = "GameProjects/" + ConvertString(m_sProjectName);

    // テンプレートファイルのパス
    //std::filesystem::path templateHeader = "Cho/Resources/EngineAssets/TemplateScript/TemplateScript.h";
    std::filesystem::path templateCpp = "Cho/Resources/EngineAssets/TemplateScript/TemplateScript.cpp";

    // 出力ファイル名
    //std::string headerFileName = scriptName + ".h";
    std::string cppFileName = scriptName + ".cpp";

    // ヘルパー関数：テンプレートの置換
    auto ReplaceScriptName = [&](const std::string& content) -> std::string {
        std::string result = content;
        std::regex pattern("\\{\\s*SCRIPT_NAME\\s*\\}");
        return std::regex_replace(result, pattern, scriptName);
        };

    // ヘッダーファイル生成
    /*{
        std::ifstream in(templateHeader);
        if (!in)
        {
            throw std::runtime_error("TemplateScript.h not found");
        }
        std::stringstream buffer;
        buffer << in.rdbuf();
        std::string replaced = ReplaceScriptName(buffer.str());

        std::ofstream out(outputDir / headerFileName);
        out << replaced;
    }*/

    // CPPファイル生成
    {
        std::ifstream in(templateCpp);
        if (!in)
        {
            throw std::runtime_error("TemplateScript.cpp not found");
        }
        std::stringstream buffer;
        buffer << in.rdbuf();
        std::string replaced = ReplaceScriptName(buffer.str());

        std::ofstream out(outputDir / cppFileName);
        out << replaced;
    }
}

void Cho::FileSystem::ScriptProject::LoadProjectPath(const std::wstring& projectName)
{
    std::string projectNameStr = ConvertString(projectName);
    // 出力先
    std::string outputPath = "GameProjects/" + projectNameStr;
    // ソリューションファイルパス
    m_SlnPath = outputPath + "/" + projectNameStr + ".sln";
    // vcxprojファイルパス
    m_ProjPath = outputPath + "/" + projectNameStr + ".vcxproj";
}

void Cho::FileSystem::ScriptProject::LoadScriptDLL()
{
	// DLLのパス
	std::string dllPath = "GameProjects/" + ConvertString(m_sProjectName) + "/bin/" + ConvertString(m_sProjectName) + ".dll";
	// PDBのロード
	LoadPDB(dllPath);
    // ロード
	m_DllHandle = LoadLibraryA(dllPath.c_str());
    if (!m_DllHandle)
    {
		Log::Write(LogLevel::Info, "Failed to load DLL: " + dllPath);
        return;
    }
}

void Cho::FileSystem::ScriptProject::UnloadScriptDLL()
{
	// PDBのアンロード
    UnloadPDB();
	// DLLのアンロード
	FreeLibrary(m_DllHandle);
    m_DllHandle = nullptr;
}

//bool Cho::FileSystem::ScriptProject::BuildScriptDLL()
//{
//	std::string projectPath = "GameProjects/" + ConvertString(m_sProjectName) + "/" + ConvertString(m_sProjectName) + ".vcxproj";
//    std::string command = "msbuild \"" + projectPath + "\" /p:Configuration=Debug /p:Platform=x64";
//    int result = std::system(command.c_str());
//
//    if (result != 0)
//    {
//		Log::Write(LogLevel::Error, "Script project build failed with code " + std::to_string(result));
//		return false;
//    } else
//    {
//		Log::Write(LogLevel::Info, "Script project built successfully.");
//		return true;
//    }
//}

bool Cho::FileSystem::ScriptProject::BuildScriptDLL()
{
    std::string projectPath = "GameProjects/" + ConvertString(m_sProjectName) + "/" + ConvertString(m_sProjectName) + ".vcxproj";
    std::string command = "msbuild \"" + projectPath + "\" /p:Configuration=Debug /p:Platform=x64";

    std::ostringstream logStream;
    logStream << "実行したコマンド: " << command << "\n";

    // _popen を利用してコマンド出力を取得
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe)
    {
        Log::Write(LogLevel::Error, "コマンド実行に失敗しました: " + command);
        return false;
    }

    char buffer[128] = { 0 };
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        logStream << buffer;
    }

    int result = _pclose(pipe);
    Log::Write(LogLevel::Info, logStream.str());

    if (result != 0)
    {
        Log::Write(LogLevel::Error, "Script project build failed with code " + std::to_string(result));
        return false;
    } else
    {
        Log::Write(LogLevel::Info, "Script project built successfully.");
        return true;
    }
}

std::vector<std::string> Cho::FileSystem::ScriptProject::GetScriptFiles()
{
    std::vector<std::string> scriptNames;
    fs::path exePath = fs::current_path(); // 実行ファイルがある場所
    // プロジェクトディレクトリ全体を再帰的に探索
    fs::path projectDir = exePath / "GameProjects" / m_sProjectName;
    for (const auto& entry : fs::recursive_directory_iterator(projectDir))
    {
        if (!entry.is_regular_file()) continue;

        const fs::path& path = entry.path();
        std::string ext = path.extension().string();

        // .cpp または .h のみに絞る
        if (ext == ".cpp" || ext == ".h")
        {
            // ファイル名（拡張子なし）
            std::string name = path.stem().string(); // 例: PlayerController.cpp → "PlayerController"

            // 重複チェック（同名 .cpp / .h がある場合）
            if (std::find(scriptNames.begin(), scriptNames.end(), name) == scriptNames.end())
            {
                scriptNames.push_back(name);
            }
        }
    }
	return scriptNames;
}

bool Cho::FileSystem::ScriptProject::LoadPDB(const std::string& dllPath)
{
    HANDLE process = GetCurrentProcess();

    // 初期化
    static bool initialized = false;
    if (!initialized)
    {
        if (!SymInitialize(process, nullptr, TRUE))
        {
            DWORD error = GetLastError();
            std::cerr << "SymInitialize failed (Error Code: " << error << ")\n";
            return false;
        }
        initialized = true;
    }

    // DLLを読み込んで、PDBも自動で探してロード
    DWORD64 baseAddress = SymLoadModuleEx(
        process,
        nullptr,
        dllPath.c_str(),
        nullptr,
        0,  // 0 = 自動計算
        0,
        nullptr,
        0
    );

    if (baseAddress == 0)
    {
        DWORD error = GetLastError();
        std::cerr << "SymLoadModuleEx failed (Error Code: " << error << ")\n";
        return false;
    }

    std::cout << "DLL and PDB loaded successfully.\n";

    // 後でUnloadに使う
    m_PDBBaseAddress = baseAddress;

    return true;
}

void Cho::FileSystem::ScriptProject::UnloadPDB()
{
    SymUnloadModule64(GetCurrentProcess(), m_PDBBaseAddress);
	SymCleanup(GetCurrentProcess());
}

void Cho::Deserialization::FromJson(const json& j, TransformComponent& t)
{
	t.translation = { j["translation"][0], j["translation"][1], j["translation"][2] };
	t.rotation = { j["rotation"][0], j["rotation"][1], j["rotation"][2], j["rotation"][3] };
	t.scale = { j["scale"][0], j["scale"][1], j["scale"][2] };
	t.degrees = { j["degrees"][0], j["degrees"][1], j["degrees"][2] };
}

void Cho::Deserialization::FromJson(const json& j, CameraComponent& c)
{
	c.fovAngleY = j.value("fovAngleY", 45.0f);
	c.aspectRatio = j.value("aspectRatio", 1.777f);
	c.nearZ = j.value("nearZ", 0.1f);
	c.farZ = j.value("farZ", 1000.0f);
}

void Cho::Deserialization::FromJson(const json& j, MeshFilterComponent& m)
{
    j;m;
	/*if (j.contains("modelName"))
	{
		m.modelName = j["modelName"].get<std::string>();
		m.modelID = j["modelID"].get<std::string>();
	} else
	{
		m.modelName = "";
		m.modelID.reset();
	}*/
}

void Cho::Deserialization::FromJson(const json& j, MeshRendererComponent& r)
{
	r.visible = j.value("visible", true);
}

void Cho::Deserialization::FromJson(const json& j, MaterialComponent& m)
{
	m.color = { j["Color"][0], j["Color"][1], j["Color"][2], j["Color"][3] };
	m.textureName = ConvertString(j.value("textureName",""));
	m.enableLighting = j.value("enableLighting", true);
	m.enableTexture = j.value("enableTexture",false);
	m.shininess = j.value("shininess", 32.0f);
}

void Cho::Deserialization::FromJson(const json& j, ScriptComponent& s)
{
    j;s;
	/*s.scriptName = j.value("scriptName", "");
	if (j.contains("scriptID"))
	{
		s.scriptID = j["scriptID"].get<std::string>();
	} else
	{
		s.scriptID.reset();
	}
	if (j.contains("entity"))
	{
		s.entity = j["entity"].get<std::string>();
	} else
	{
		s.entity.reset();
	}*/
}

void Cho::Deserialization::FromJson(const json& j, std::vector<LineRendererComponent>& ls)
{
    j;ls;
	/*if (j.is_array())
	{
		for (const auto& item : j)
		{
			LineRendererComponent l;
			l.line.start = { item["start"][0], item["start"][1], item["start"][2] };
			l.line.end = { item["end"][0], item["end"][1], item["end"][2] };
			l.line.color = { item["color"][0], item["color"][1], item["color"][2], item["color"][3] };
			ls.push_back(l);
		}
	}*/
}

void Cho::Deserialization::FromJson(const json& j, Rigidbody2DComponent& rb)
{
    j;rb;
	/*rb.isKinematic = j.value("isKinematic", false);
	rb.gravityScale = j.value("gravityScale", 1.0f);
	rb.mass = j.value("mass", 1.0f);
	rb.bodyType = static_cast<Rigidbody2DComponent::BodyType>(j.value("bodyType", 0));
	rb.fixedRotation = j.value("fixedRotation", false);*/
}

void Cho::Deserialization::FromJson(const json& j, BoxCollider2DComponent& bc)
{
	bc.offsetX = j.value("offsetX", 0.0f);
	bc.offsetY = j.value("offsetY", 0.0f);
	bc.width = j.value("width", 1.0f);
	bc.height = j.value("height", 1.0f);
	bc.density = j.value("density", 1.0f);
	bc.friction = j.value("friction", 0.5f);
	bc.restitution = j.value("restitution", 0.0f);
}

void Cho::Deserialization::FromJson(const json& j, EmitterComponent& e)
{
	e.position = { j["position"][0], j["position"][1], j["position"][2] };
	e.radius = j.value("radius", 1.0f);
	e.count = j.value("count", 100);
	e.frequency = j.value("frequency", 0.1f);
	e.frequencyTime = j.value("frequencyTime", 0.1f);
}

void Cho::Deserialization::FromJson(const json& j, ParticleComponent& p)
{
	p.count = j.value("count", 1024);
}

void Cho::FileSystem::ScanFolder(const path& rootPath, EngineCommand* engineCommand)
{
    g_ProjectFiles = ScanRecursive(rootPath,engineCommand);
}

FolderNode Cho::FileSystem::ScanRecursive(const path& path, EngineCommand* engineCommand)
{
    FolderNode node;        // 一番上のノード
	node.folderPath = path; // フォルダパス

    // フォルダの中を走査
    for (const auto& entry : directory_iterator(path))
    {
		// フォルダなら再帰処理
        if (entry.is_directory())
        {
			// フォルダ名がbin,obj,.vs,.gitの場合はスキップ
            if (entry.path().filename() == "bin" ||
                entry.path().filename() == "obj" ||
                entry.path().filename() == ".vs" ||
                entry.path().filename() == ".git")
            {
                continue;
            }
			node.children.push_back(ScanRecursive(entry.path(),engineCommand));
        }
		// ファイルなら処理
		else if (entry.is_regular_file())
		{
            if (ProcessFile(entry.path(),engineCommand))// 処理に成功したらノードに追加
            {
                node.files.push_back(entry.path());
            }
		}
    }
    return node;
}

// フォルダノードをパスで検索
FolderNode* Cho::FileSystem::FindFolderNodeByPath(FolderNode& node, const std::filesystem::path& target)
{
    if (node.folderPath == target)
        return &node;

    for (auto& child : node.children)
    {
        if (FolderNode* found = FindFolderNodeByPath(child, target))
            return found;
    }
    return nullptr;
}

bool Cho::FileSystem::ProcessFile(const path& filePath, EngineCommand* engineCommand)
{
	std::wstring wFileName = filePath.filename().wstring();

    // テクスチャファイル
    if (wFileName.ends_with(L".dds")|| wFileName.ends_with(L".png") || wFileName.ends_with(L".jpg"))
    {
		// テクスチャの処理
        engineCommand->GetResourceManager()->GetTextureManager()->LoadTextureFile(filePath);
        return true;
    }
	// モデルファイル
    if (wFileName.ends_with(L".fbx") || wFileName.ends_with(L".gltf") || wFileName.ends_with(L".obj"))
    {
		// モデルの処理
        return engineCommand->GetResourceManager()->GetModelManager()->LoadModelFile(filePath);
    }
	// 音声ファイル
	if (wFileName.ends_with(L".wav") || wFileName.ends_with(L".mp3"))
	{
		// 音声の処理
        return false;
	}
	// スクリプトファイル
	if (wFileName.ends_with(L".cpp") || wFileName.ends_with(L".h"))
	{
		// スクリプトの処理
        return false;
	}
	// jsonファイル
	if (wFileName.ends_with(L".json"))
	{
		// jsonの処理
		FileType type = FileSystem::GetJsonFileType(filePath);

        switch (type)
        {
        case Cho::ChoProject:
            break;
        case Cho::EngineConfig:
            break;
        case Cho::GameSettings:
            break;
        case Cho::SceneFile:// シーンファイル
        {
			return LoadSceneFile(filePath,engineCommand);
        }
            break;
        case Cho::ModelFile:
            break;
        case Cho::ImageFile:
            break;
        case Cho::SoundFile:
            break;
        case Cho::EffectFile:
            break;
		case Cho::ScriptFile:// スクリプトファイル
        {
			return LoadScriptFile(filePath, engineCommand);
        }
            break;
        case Cho::GameParameter:
            break;
        case Cho::PrefabFile:
            break;
        case Cho::Unknown:
            break;
        default:
            break;
        }
	}
	// その他のファイルは無視
	return false;
}

bool Cho::FileSystem::AddFile(const path& filePath, FolderNode& folderNode, EngineCommand* engineCommand)
{
    namespace fs = std::filesystem;
    if (filePath.empty() || !fs::exists(filePath)) return false;

    try
    {
        if (fs::is_regular_file(filePath))
        {
            path dstPath = folderNode.folderPath / filePath.filename();

            // コピー（上書き許可）
            if (!fs::copy_file(filePath, dstPath, fs::copy_options::overwrite_existing))
            {
				// コピー失敗
				return false;
            }

            // コピー後のパスで処理
            if (ProcessFile(dstPath, engineCommand))
            {
                folderNode.files.push_back(dstPath);
                return true;
            } else
            {
                fs::remove(dstPath); // 処理失敗なら削除
            }
        } else if (fs::is_directory(filePath))
        {
            path dstDir = folderNode.folderPath / filePath.filename();

            // ディレクトリ作成
            fs::create_directories(dstDir);

            FolderNode childNode;
            childNode.folderPath = dstDir;

            bool hasValid = false;

            for (const auto& entry : fs::directory_iterator(filePath))
            {
                if (AddFile(entry.path(), childNode, engineCommand))
                {
                    hasValid = true;
                }
            }

            if (hasValid)
            {
                folderNode.children.push_back(std::move(childNode));
                return true;
            } else
            {
                fs::remove_all(dstDir); // 有効なファイルがなければ削除
            }
        }
    }
    catch (const fs::filesystem_error& e)
    {
        // ログ出力などがあれば追加
        Cho::Log::Write(LogLevel::Assert,"AddFile failed: " + filePath.string() + " (" + e.what() + ")");
    }

    return false;
}
