#include "PrecompiledHeader.h"
#include "StartSetting.h"

#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"D3D12/RTVManager/RTVManager.h"
#include"D3D12/DrawExecution/DrawExecution.h"

// SceneManager
#include"Scene/SceneManager/SceneManager.h"

#include"ECS/GameObject/GameObject.h"

#include"Editor/EditorManager/EditorManager.h"

#include"Generator/GameProject/GameProject.h"
#include"Generator/ScriptProject/ScriptProject.h"

// Json
#include"Load/JsonFileLoader/JsonFileLoader.h"

#include"imgui.h"

void StartSetting::Initialize(
	ResourceViewManager* rvManager,
	RTVManager* rtvManager,
	DrawExecution* drawExe,
	EntityManager* entityManager,
	ComponentManager* componentManager,
	SystemManager* systemManager,
	PrefabManager* prefabManager,
	SceneManager* sceneManager,
	EditorManager* editorManager
)
{
	// ひとまず開発用プロジェクトの作成
	//GameProject::CreateGameFolder(projectName);

	//std::string name = "SceneCamera";
	//name = sceneManager->AddCameraObject(name);
	//Entity ent = sceneManager->GetCameraObject(name)->GetEntityID();
	//entityManager->SetCameraID(ent);
	//// デフォルトオブジェクト生成
	//name = "Cube";
	//name = sceneManager->AddGameObject(name);
	//TransformComponent TFComp;
	//MeshComponent MeshComp;
	//MeshComp.meshID = static_cast<uint32_t>(MeshPattern::Cube);
	//MaterialComponent MaterialComp;
	//RenderComponent renderComp;
 //   renderComp.visible = false;
	//sceneManager->GetGameObject(name)->AddComponent(TFComp);
	//sceneManager->GetGameObject(name)->AddComponent(MeshComp);
	//sceneManager->GetGameObject(name)->AddComponent(MaterialComp);
	//sceneManager->GetGameObject(name)->AddComponent(renderComp);
	//editorManager->SetSelectedGOName(name);
	//editorManager->SetSelectedGO(sceneManager->GetGameObject(name));
    sceneManager;
    editorManager;
    entityManager;
	rtvManager;
    rvManager;
	drawExe;
	componentManager;
	systemManager;
	prefabManager;
}

bool StartSetting::IsProject()
{
	return isProject;
}

void StartSetting::SelectedProject()
{
    {
        // エンジンデバッグのため
        // ロード機能が実装されるまで
        isProject = true;
        createNewProject = true;
        projectName = "test";
        return;
    }
    //FindProjects();
    //// モーダルウィンドウを表示するフラグ
    //if (!ImGui::IsPopupOpen("Project Selector"))
    //{
    //    ImGui::OpenPopup("Project Selector");
    //}

    //// モーダルウィンドウのスタイル
    //ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
    //    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    //// モーダルウィンドウの表示
    //if (ImGui::BeginPopupModal("Project Selector", nullptr, windowFlags))
    //{
    //    // ウィンドウを中央に配置
    //    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    //    ImGui::SetNextWindowSize(ImVec2(400, 300));

    //    // ラジオボタンでプロジェクトの作成か読み込みを選択
    //    if (ImGui::RadioButton("Create New Project", createNewProject)) {
    //        createNewProject = true;
    //    }
    //    if (ImGui::RadioButton("Load Existing Project", !createNewProject)) {
    //        createNewProject = false;
    //    }

    //    ImGui::Spacing();

    //    // プロジェクト名の入力
    //    ImGui::Text("Project Name:");
    //    ImGui::InputText("##ProjectName", projectNameBuffer, sizeof(projectNameBuffer));
    //    projectName = projectNameBuffer; // 入力内容を std::string に保存

    //    ImGui::Spacing();

    //    // プロジェクト名が空の場合ボタンを無効化
    //    bool isNameValid = !projectName.empty();

    //    if (createNewProject) {
    //        if (!isNameValid) {
    //            ImGui::BeginDisabled(); // ボタンを無効化
    //        }
    //        if (ImGui::Button("Create Project", ImVec2(120, 0))) {
    //            isProject = true;
    //            ImGui::CloseCurrentPopup(); // モーダルを閉じる
    //        }
    //        if (!isNameValid) {
    //            ImGui::EndDisabled(); // ボタンを再度有効化
    //        }
    //    } else {
    //        if (!isNameValid) {
    //            ImGui::BeginDisabled(); // ボタンを無効化
    //        }
    //        if (ImGui::Button("Load Project", ImVec2(120, 0))) {
    //            isProject = true;
    //            ImGui::CloseCurrentPopup(); // モーダルを閉じる
    //        }
    //        if (!isNameValid) {
    //            ImGui::EndDisabled(); // ボタンを再度有効化
    //        }
    //    }

    //    ImGui::SeparatorText("Projects");

    //    for (auto& project : projectList) {
    //        if (ImGui::Selectable(project.c_str())) {
    //            projectName = project;
    //            isProject = true;
    //            createNewProject = false;
    //            ImGui::CloseCurrentPopup(); // モーダルを閉じる
    //        }
    //    }

    //    ImGui::EndPopup();
    //}
}

void StartSetting::CreateProject()
{
	
}

void StartSetting::LoadProject(JsonFileLoader* jsonLoader)
{
    jsonLoader->LoadProject();
}

void StartSetting::FindProjects()
{
    // プロジェクトのルートディレクトリを絶対パスに変換
    std::string fullProjectsPath = fs::absolute(projectRoot).string();

    // プロジェクトリストを初期化
    projectList.clear();

    try {
        // ディレクトリ内を探索
        for (const auto& entry : fs::directory_iterator(fullProjectsPath)) {
            if (entry.is_directory()) { 
                projectList.push_back(entry.path().filename().string()); // ファイル名をリストに追加
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error accessing directory: " << e.what() << std::endl;
    }

    // 結果をデバッグ表示
    std::cout << "Found project files:\n";
    for (const auto& project : projectList) {
        std::cout << "  " << project << "\n";
    }
}
