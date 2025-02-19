#include "PrecompiledHeader.h"
#include "ToolBar.h"

#include"imgui.h"

#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"D3D12/RTVManager/RTVManager.h"
#include"D3D12/DrawExecution/DrawExecution.h"

// SceneManager
#include"Scene/SceneManager/SceneManager.h"

#include"ECS/System/SystemManager/SystemManager.h"
#include"ECS/GameObject/GameObject.h"

#include"Editor/EditorManager/EditorManager.h"

#include"Generator/ScriptProject/ScriptProject.h"

#include"Script/ScriptManager/ScriptManager.h"

void ToolBar::Initialize(
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

void ToolBar::Update()
{
    // 移動を無効にするフラグ
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;
    ImGui::Begin("ToolBar",nullptr,windowFlags);
    PlaybackControls();
    ImGui::End();
}

void ToolBar::PlaybackControls() {
    // ボタンのスタイル設定
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f); // 角丸の設定
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));    // ボタンの色
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f)); // ホバー時の色
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));  // クリック時の色

    // レイアウト設定
    ImGui::BeginGroup(); // グループ化してボタンを横並びにする

    // 再生ボタン (Play)
    //if (ImGui::Button("▶", ImVec2(30, 30))) {
    if (systemManager_->IsPlay()) {
        if (ImGui::Button("End", ImVec2(30, 30))) {
            // 再生ボタンの処理
            systemManager_->PlaySwitch();
            systemManager_->Start(*entityManager_, *componentManager_);
            systemManager_->UnLoadScripts();
        }
    }else{
        if (ImGui::Button("Play", ImVec2(30, 30))) {
            // 再生ボタンの処理
            systemManager_->PlaySwitch();
            systemManager_->LoadScripts();
            systemManager_->Start(*entityManager_,*componentManager_);
        }
    }
    ImGui::SameLine();

    // 一時停止ボタン (Pause)
    if (ImGui::Button("⏸", ImVec2(30, 30))) {
        // 一時停止ボタンの処理
       // printf("Pause button pressed!\n");
    }
    ImGui::SameLine();

    // ステップボタン (Step)
    if (ImGui::Button("⏭", ImVec2(30, 30))) {
        // ステップボタンの処理
        //printf("Step button pressed!\n");
    }

    ImGui::EndGroup(); // グループ化終了

    // スタイルのリセット
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();
}

