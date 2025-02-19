#include "PrecompiledHeader.h"
#include "JsonFileLoader.h"

#include"imgui.h"

#include"Script/ScriptManager/ScriptManager.h"
#include"ECS/EntityManager/EntityManager.h"
#include"ECS/ComponentManager/ComponentManager.h"
#include"ECS/PrefabManager/PrefabManager.h"
#include"ECS/GameObject/GameObject.h"
#include"Scene/SceneManager/SceneManager.h"

#include"SystemState/SystemState.h"

void JsonFileLoader::Initialize(
    ScriptManager* scriptManager,
    EntityManager* entityManager,
    ComponentManager* componentManager,
    PrefabManager* prefabManager,
    SceneManager* sceneManager
)
{
    scriptManager_ = scriptManager;
    entityManager_ = entityManager;
    componentManager_ = componentManager;
    prefabManager_ = prefabManager;
    sceneManager_ = sceneManager;
}

void JsonFileLoader::SaveStyle(const char* filename)
{
    ImGuiStyle& style = ImGui::GetStyle();
    nlohmann::json j;

    // ImGuiの全ての色をJSONオブジェクトに保存
    for (int i = 0; i < ImGuiCol_COUNT; ++i) {
        ImVec4& color = style.Colors[i];
        j["Colors"][i] = { color.x, color.y, color.z, color.w };
    }
    // ウィンドウの角の丸みの設定も保存
    j["WindowRounding"] = style.WindowRounding;

    // ファイルに書き込む
    std::ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(4); // インデント幅4でJSONを整形して保存
        file.close();
    }
}

void JsonFileLoader::LoadStyle(const char* filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        return; // ファイルが開けない場合は何もしない

    nlohmann::json j;
    file >> j; // ファイルからJSONデータを読み込む
    file.close();

    ImGuiStyle& style = ImGui::GetStyle();
    // Colorsが含まれている場合、各色を設定
    if (j.contains("Colors")) {
        for (int i = 0; i < ImGuiCol_COUNT && i < j["Colors"].size(); ++i) {
            auto& color = j["Colors"][i];
            style.Colors[i] = ImVec4(color[0], color[1], color[2], color[3]);
        }
    }
    // WindowRoundingが含まれている場合、角の丸みを設定
    if (j.contains("WindowRounding")) {
        style.WindowRounding = j["WindowRounding"];
    }
}

void JsonFileLoader::SaveStyleToProject()
{
    std::filesystem::path styleDir = "Cho\\Resources\\ImGuiStyle";
    // ディレクトリが存在しない場合は作成
    if (!std::filesystem::exists(styleDir)) {
        std::filesystem::create_directories(styleDir);
    }
    // スタイルファイルのパスを設定
    std::filesystem::path styleFile = styleDir / "style.json";
    SaveStyle(styleFile.string().c_str()); // スタイルを保存
}

void JsonFileLoader::LoadStyleFromProject()
{
    std::filesystem::path styleFile = "Cho\\Resources\\ImGuiStyle\\style.json";
    // スタイルファイルが存在する場合は読み込む
    if (std::filesystem::exists(styleFile)) {
        LoadStyle(styleFile.string().c_str());
    } else {
        // スタイルファイルが存在しない場合、新規に作成してデフォルトスタイルを保存
        SaveStyle(styleFile.string().c_str());
    }
}

void JsonFileLoader::LoadProject()
{
    // プロジェクト名とルートディレクトリを取得
    std::string projectName = ProjectName();
    std::string projectRoot = ProjectRoot();

    // プロジェクトデータディレクトリを構築
    std::filesystem::path projectDataDir = projectRoot + "\\" + projectName + "\\" + "ProjectData";
    std::filesystem::path dataFile = projectDataDir / "projectData.json";

    // ファイルが存在しない場合はエラー
    if (!std::filesystem::exists(dataFile)) {
        std::cerr << "Project data file not found: " << dataFile << std::endl;
        return;
    }

    // JSON ファイルを読み込む
    nlohmann::ordered_json j;
    std::ifstream file(dataFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open project data file: " << dataFile << std::endl;
        return;
    }

    file >> j;
    file.close();

    // シーンを読み込み
    for (auto& [sceneName, sceneData] : j.items()) {
        std::cout << "Loading scene: " << sceneName << std::endl;

        // ゲームオブジェクトを読み込み
        for (auto& gameObjJson : sceneData) {
            if (gameObjJson.contains("GameObject")) {
                auto& objData = gameObjJson["GameObject"];

                std::string name = objData["name"];
                Entity entity = objData["entityId"];
                ObjectType type = objData["objectType"];

                // ゲームオブジェクトを再構築
                //sceneManager_->AddGameObject(name);
                GameObject* gameObject = sceneManager_->GetGameObject(name);
                gameObject->SetEntity(entity);
                gameObject->SetObjectType(type);

                // コンポーネントを復元
                if (objData.contains("Components")) {
                    auto& compData = objData["Components"];

                    // TransformComponent
                    if (compData.contains("transform")) {
                        auto& tfJ = compData["transform"];

                        TransformComponent tf;

                        tf.translation = { tfJ["position"][0], tfJ["position"][1], tfJ["position"][2] };
                        tf.degrees = { tfJ["rotation"][0], tfJ["rotation"][1], tfJ["rotation"][2] };
                        tf.scale = { tfJ["scale"][0], tfJ["scale"][1], tfJ["scale"][2] };

                        gameObject->AddComponent(tf);
                    }

                    // MaterialComponent
                    if (compData.contains("material")) {
                        auto& mtlJ = compData["material"];
                        MaterialComponent mtl;

                        mtl.textureID = mtlJ["texture"];

                        gameObject->AddComponent(mtl);
                    }

                    // MeshComponent
                    if (compData.contains("mesh")) {
                        auto& meshJ = compData["mesh"];
                        MeshComponent mesh;

                        mesh.meshID = meshJ["meshID"];

                        gameObject->AddComponent(mesh);
                    }

                    // RenderComponent
                    if (compData.contains("render")) {
                        auto& renderJ = compData["render"];
                        RenderComponent render;

                        render.visible = renderJ["visible"];

                        gameObject->AddComponent(render);
                    }

                    // PhysicsComponent
                    if (compData.contains("physics")) {
                        auto& physicsJ = compData["physics"];
                        PhysicsComponent physics;

                        physics.velocity = { physicsJ["velocity"][0], physicsJ["velocity"][1], physicsJ["velocity"][2] };
                        physics.acceleration = { physicsJ["acceleration"][0], physicsJ["acceleration"][1], physicsJ["acceleration"][2] };

                        gameObject->AddComponent(physics);
                    }

                    // ScriptComponent
                    if (compData.contains("script")) {
                        auto& scriptJ = compData["script"];
                        ScriptComponent script;

                        script.status.name = scriptJ["scriptName"];
                        script.status.type = scriptJ["type"];
                        script.id = scriptJ["id"];
                        script.isScript = scriptJ["isScript"];

                        gameObject->AddComponent(script);
                    }
                }
            }

            // カメラオブジェクトを読み込み
            if (gameObjJson.contains("CameraObject")) {
                auto& objData = gameObjJson["CameraObject"];

                std::string name = objData["name"];
                //Entity entity = objData["entityId"];
                //ObjectType type = objData["objectType"];

                // カメラオブジェクトを再構築
                //sceneManager_->AddCameraObject(name);
               // GameObject* cameraObject = sceneManager_->GetCameraObject(name);
                //cameraObject->SetEntity(entity);
                //cameraObject->SetObjectType(type);

                // CameraComponent
                if (objData["Components"].contains("camera")) {
                    auto& cameraJ = objData["Components"]["camera"];
                    CameraComponent camera;

                    camera.translation = { cameraJ["position"][0], cameraJ["position"][1], cameraJ["position"][2] };
                    camera.degrees = { cameraJ["rotation"][0],cameraJ["rotation"][1],cameraJ["rotation"][2] };

                    camera.fovAngleY = cameraJ["fovAngleY"];
                    camera.aspectRatio = cameraJ["AspectRatio"];
                    camera.nearZ = cameraJ["nearZ"];
                    camera.farZ = cameraJ["farZ"];

                    //cameraObject->AddComponent(camera);
                }
            }
        }

        /*for (const auto& script : sceneData) {
            if (script.contains("Object")) {
                auto& scriptData = script["Object"];
                
            }
        }

        for (const auto& [typeStr, scriptMapJson] : sceneData.items()) {
            ObjectType type = StringToObjectType(typeStr);
            for (const auto& [scriptName, scriptJson] : scriptMapJson.items()) {
                ScriptStatus status;
                status.name = scriptJson.at("name").get<std::string>();
                status.type = type;

                scripts[type][scriptName] = status;
            }
        }*/
    }
}
