#include "PrecompiledHeader.h"
#include "SaveManager.h"

#include"Script/ScriptManager/ScriptManager.h"
#include"ECS/EntityManager/EntityManager.h"
#include"ECS/ComponentManager/ComponentManager.h"
#include"ECS/PrefabManager/PrefabManager.h"
#include"ECS/GameObject/GameObject.h"
#include"Scene/SceneManager/SceneManager.h"

#include"Load/JsonFileLoader/JsonFileLoader.h"

#include"SystemState/SystemState.h"

// Enum を文字列に変換する関数
std::string ObjectTypeToString(ObjectType type) {
    switch (type) {
    case ObjectType::Object: return "Object";
    case ObjectType::Camera: return "Camera";
    //case ObjectType::Light: return "Light";
    default: return "Unknown";
    }
}

void SaveManager::Initialize(
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

void SaveManager::Save(JsonFileLoader* jsonLoad)
{
    jsonLoad;
    std::string projectName = ProjectName();
    std::string projectRoot = ProjectRoot();

    // プロジェクトがなかったら
    if (projectName == "" || projectRoot == "") {
        return;
    }

    std::filesystem::path projectDataDir = projectRoot + "\\" + projectName + "\\" + "ProjectData";

    if (!std::filesystem::exists(projectDataDir)) {
        std::filesystem::create_directories(projectDataDir);
    }
    std::filesystem::path dataFile = projectDataDir / "projectData.json";
    std::string dfn = dataFile.string();
    const char* dataFileName = dfn.c_str();

    // シリアライズ
    nlohmann::ordered_json j;

    // シーンごとのゲームオブジェクトを格納
    //for (auto& scene : sceneManager_->GetScenes()) {
    nlohmann::ordered_json sceneJson;

    // シーン名をキーとして設定
    //std::string sceneName = scene.GetName();
    std::string sceneName = "MainScene";

    // ゲームオブジェクトを追加
    for (auto& gameObject : sceneManager_->GetGameObjects()) {
        nlohmann::ordered_json json;
        nlohmann::ordered_json gameObjJson;

        // ID,type,nameを取得
        Entity entity = gameObject.second->GetEntityID();
        ObjectType type = gameObject.second->GetObjectType();
        std::string name = gameObject.first;

        // 名前とIDを設定
        json["name"] = name;
        json["entityId"] = entity;
        json["objectType"] = type;

        // コンポーネント
        nlohmann::ordered_json compJ;

        // TransformComponent
        if (componentManager_->GetComponent<TransformComponent>(entity)) {
            TransformComponent& tf = componentManager_->GetComponent<TransformComponent>(entity).value();
            nlohmann::ordered_json tfJ;

            tfJ["position"] = { tf.translation.x, tf.translation.y, tf.translation.z };
            tfJ["rotation"] = { tf.degrees.x, tf.degrees.y, tf.degrees.z };
            tfJ["scale"] = { tf.scale.x, tf.scale.y, tf.scale.z };

            compJ["transform"] = tfJ;
        }

        // MaterialComponent
        if (componentManager_->GetComponent<MaterialComponent>(entity)) {
            MaterialComponent& mtl = componentManager_->GetComponent<MaterialComponent>(entity).value();
            nlohmann::ordered_json mtlJ;

            mtlJ["texture"] = mtl.textureID;

            compJ["material"] = mtlJ;
        }

        // MeshComponent
        if (componentManager_->GetComponent<MeshComponent>(entity)) {
            MeshComponent& mesh = componentManager_->GetComponent<MeshComponent>(entity).value();
            nlohmann::ordered_json meshJ;

            meshJ["meshID"] = { mesh.meshID };

            compJ["mesh"] = meshJ;
        }

        // RenderComponent
        if (componentManager_->GetComponent<RenderComponent>(entity)) {
            RenderComponent& render = componentManager_->GetComponent<RenderComponent>(entity).value();
            nlohmann::ordered_json renderJ;

            renderJ["visible"] = render.visible;

            compJ["render"] = renderJ;
        }

        // PhysicsComponent
        if (componentManager_->GetComponent<PhysicsComponent>(entity)) {
            PhysicsComponent& physics = componentManager_->GetComponent<PhysicsComponent>(entity).value();
            nlohmann::ordered_json physicsJ;

            physicsJ["velocity"] = { physics.velocity.x,physics.velocity.y,physics.velocity.z };
            physicsJ["acceleration"] = { physics.acceleration.x,physics.acceleration.y,physics.acceleration.z };

            compJ["physics"] = physicsJ;
        }

        // ScriptComponent
        if (componentManager_->GetComponent<ScriptComponent>(entity)) {
            ScriptComponent& script = componentManager_->GetComponent<ScriptComponent>(entity).value();
            nlohmann::ordered_json scriptJ;

            scriptJ["scriptName"] = script.status.name;
            scriptJ["type"] = script.status.type;
            scriptJ["id"] = script.id;
            scriptJ["isScript"] = script.isScript;

            compJ["script"] = scriptJ;
        }

        // コンポーネントを追加
        json["Components"] = compJ;

        gameObjJson["GameObject"] = json;

        // ゲームオブジェクトをシーンに追加
        sceneJson.push_back(gameObjJson);
    }

    //// カメラオブジェクト
    //for (auto& cameraObject : sceneManager_->GetCameraObjects()) {
    //    nlohmann::ordered_json json;
    //    nlohmann::ordered_json cameraObjJson;

    //    // ID,type,nameを取得
    //    Entity entity = cameraObject.second.GetEntityID();
    //    ObjectType type = cameraObject.second.GetObjectType();
    //    std::string name = cameraObject.first;

    //    // 名前とIDを設定
    //    json["name"] = name;
    //    json["entityId"] = entity;
    //    json["objectType"] = type;

    //    // コンポーネント
    //    nlohmann::ordered_json compJ;

    //    // CameraComponent
    //    if (componentManager_->GetCamera(entity)) {
    //        CameraComponent* camera = componentManager_.GetCamera(entity);
    //        nlohmann::ordered_json cameraJ;

    //        cameraJ["position"] = { camera.translation.x,camera.translation.y,camera.translation.z };
    //        cameraJ["rotation"] = { camera.degrees.x,camera.degrees.y,camera.degrees.z };

    //        cameraJ["fovAngleY"] = camera.fovAngleY;
    //        cameraJ["aspectRatio"] = camera.aspectRatio;
    //        cameraJ["nearZ"] = camera.nearZ;
    //        cameraJ["farZ"] = camera.farZ;

    //        compJ["camera"] = cameraJ;
    //    }
    //    // コンポーネントを追加
    //    json["Components"] = compJ;

    //    cameraObjJson["CameraObject"] = json;

    //    // ゲームオブジェクトをシーンに追加
    //    sceneJson.push_back(cameraObjJson);
    //}

    // スクリプト
    nlohmann::ordered_json scriptJ;
    for (const auto& [type, scriptMap] : scriptManager_->GetScripts()) {
        std::string typeStr = ObjectTypeToString(type);
        nlohmann::ordered_json typeJson;
        for (const auto& [scriptName, status] : scriptMap) {
            nlohmann::ordered_json script;
            script["name"] = status.name;

            // ObjectType ごとにスクリプトをまとめる
            typeJson[scriptName] = script;
        }
        scriptJ[typeStr] = typeJson;
    }

    // スクリプト全体をシーンJSONに追加
    sceneJson.push_back({ {"Script", scriptJ} });

    // シーンを全体のJSONに追加
    j[sceneName] = sceneJson;
    // }

    // ファイルに書き込む
    std::ofstream file(dataFileName);
    if (!file.is_open()) {
        // ファイルが開けない
        assert(0);
    }
    file << j.dump(4);
    file.close();
}