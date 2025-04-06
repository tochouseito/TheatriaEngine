#include "pch.h"
#include "Toolbar.h"
#include "Editor/EditorManager/EditorManager.h"
#include "Core/ChoLog/ChoLog.h"

void Toolbar::Initialize()
{
}

void Toolbar::Update()
{
	Window();
}

void Toolbar::Window()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 2));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(40, 40, 40, 255)); // 背景色

    // ビューポートの横幅を取得して横いっぱいにツールバーを描画
    ImGui::BeginChild("Toolbar", ImVec2(0, 32), false, ImGuiWindowFlags_NoScrollbar);

    // ツールボタン（アイコン風）
    //if (ImGui::Button("☀")) { /* Scene Tool */ }
    //ImGui::SameLine();
    //if (ImGui::Button("🡆")) { /* Move Tool */ }
    //ImGui::SameLine();
    //if (ImGui::Button("⤾")) { /* Rotate Tool */ }
    //ImGui::SameLine();
    //if (ImGui::Button("⇲")) { /* Scale Tool */ }
    ImGui::SameLine(0, 40); // 大きめにスペース
    // 上部に余白（例：4px）
    ImGui::Dummy(ImVec2(1, 4));

    // 中央に配置するための計算
    float toolbarWidth = 0.0f;
    float buttonSize = 28.0f; // ボタンサイズ
    float buttonSpacing = 8.0f; // ボタン間のスペース
    int buttonCount = 3;
    toolbarWidth = buttonCount * buttonSize + (buttonCount - 1) * buttonSpacing;

    // ウィンドウ中央位置に移動
    float availableWidth = ImGui::GetContentRegionAvail().x;
    float startX = (availableWidth - toolbarWidth) * 0.5f;
    ImGui::SetCursorPosX(startX);

    // ボタンを描画（中央に並べる）
    if (ImGui::Button(ICON_FA_PLAY, ImVec2(buttonSize, buttonSize))) { /* Play */ }
    ImGui::SameLine(0, buttonSpacing);
    if (ImGui::Button(ICON_FA_PAUSE, ImVec2(buttonSize, buttonSize))) { /* Pause */ }
    ImGui::SameLine(0, buttonSpacing);
    if (ImGui::Button(ICON_FA_STEP_FORWARD, ImVec2(buttonSize, buttonSize))) { /* Step */ }

    ImGui::SameLine(0, 40);

    /*ImGui::Text("Layout:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    static int currentLayout = 0;
    const char* layouts[] = { "Default", "2D", "Debug" };
    ImGui::Combo("##Layout", &currentLayout, layouts, IM_ARRAYSIZE(layouts));*/

    ImGui::EndChild();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}
