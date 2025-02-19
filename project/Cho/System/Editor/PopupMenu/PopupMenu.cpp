#include "PrecompiledHeader.h"
#include "PopupMenu.h"

// ImGui
#include"imgui.h"

#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"D3D12/RTVManager/RTVManager.h"
#include"D3D12/DrawExecution/DrawExecution.h"

// SceneManager
#include"Scene/SceneManager/SceneManager.h"

#include"ECS/GameObject/GameObject.h"

#include"Editor/EditorManager/EditorManager.h"

// Generator
#include"Generator/ScriptProject/ScriptProject.h"

#include"Script/ScriptManager/ScriptManager.h"

void PopupMenu::Initialize(
	ResourceViewManager* rvManager,
	RTVManager* rtvManager,
	DrawExecution* drawExe,
	EntityManager* entityManager,
	ComponentManager* componentManager,
	SystemManager* systemManager,
	PrefabManager* prefabManager,
	SceneManager* sceneManager,
    EditorManager* editManager,
    ScriptManager* scriptManager
)
{
	// D3D12
	rvManager_ = rvManager;
	rtvManager_ = rtvManager;
	drawExe_ = drawExe;

	// ECS
	entityManager_ = entityManager;
	componentManager_ = componentManager;
	systemManager_ = systemManager;
	prefabManager_ = prefabManager;

	// SceneManager
	sceneManager_ = sceneManager;

    editManager_ = editManager;

    scriptManager_ = scriptManager;
}

void PopupMenu::Update(bool excludeRightClick)
{
    // メニューを開く位置を右クリックで検出
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && !excludeRightClick)
    {
        ImGui::OpenPopup("PopupMenu");  // 右クリックでメニューを開く
    }
    // ポップアップメニューの定義
    if (ImGui::BeginPopup("PopupMenu"))
    {
        // 「Add」メニュー項目の追加
        if (ImGui::BeginMenu("Create"))
        {
            // 「3DObject」サブメニューを追加
            if (ImGui::BeginMenu("3DObject"))
            {
                // 「3DObject」メニューの横にさらにリスト表示
                if (ImGui::MenuItem("BaseObject")) {
                    editManager_->CreateObject(ObjectType::Object);
                }

                ImGui::EndMenu(); // 「3DObject」サブメニューを終了
            }

            // 他のメニュー項目の追加
            if (ImGui::MenuItem("Camera")) { 
                editManager_->CreateObject(ObjectType::Camera);;
            }

			if (ImGui::BeginMenu("Light")) {
				if (ImGui::MenuItem("DirectionalLight")) {
					editManager_->CreateObject(ObjectType::DirectionalLight);;
				}
				if (ImGui::MenuItem("PointLight")) {
					editManager_->CreateObject(ObjectType::PointLight);;
				}
				if (ImGui::MenuItem("SpotLight")) {
					editManager_->CreateObject(ObjectType::SpotLight);
				}

				ImGui::EndMenu();
			}
            
            if (ImGui::MenuItem("Sprite")) {
                editManager_->CreateObject(ObjectType::Sprite);;
            }

            if (ImGui::MenuItem("Particle")) {
                editManager_->CreateObject(ObjectType::Particle);;
            }

			if (ImGui::MenuItem("MapChipBlock")) {
				editManager_->CreateObject(ObjectType::MapChipBlock);;
			}

			if (ImGui::MenuItem("Effect")) {
				editManager_->CreateObject(ObjectType::Effect);
			}

            if (ImGui::BeginMenu("C++Script")) {
                if (ImGui::MenuItem("ObjectScript")) {
                    static std::string name = "NewScript";

                    name = scriptManager_->AddScript(ObjectType::Object, name);
                    ScriptProject::GenerateScriptTemplate(name);
                    ScriptProject::Update();
                    editManager_->UpdateFiles();
                }
                if (ImGui::MenuItem("CameraScript")) {

                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("OpenVS:Test")) {
                ScriptProject::OpenVisualStudio();
            }

            ImGui::EndMenu(); // 「Add」メニューを終了
        }
        ImGui::EndPopup();
    }
}