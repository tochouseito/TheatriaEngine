#include "pch.h"
#include "Toolbar.h"
#include "Editor/EditorManager/EditorManager.h"

void Toolbar::Initialize()
{
}

void Toolbar::Update()
{
	Window();
}

void Toolbar::Window()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 4));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(40, 40, 40, 255)); // 背景色

    // ビューポートの横幅を取得して横いっぱいにツールバーを描画
    ImGui::BeginChild("Toolbar", ImVec2(0, 32), false, ImGuiWindowFlags_NoScrollbar);

    // ツールボタン（アイコン風）
    if (ImGui::Button("☀")) { /* Scene Tool */ }
    ImGui::SameLine();
    if (ImGui::Button("🡆")) { /* Move Tool */ }
    ImGui::SameLine();
    if (ImGui::Button("⤾")) { /* Rotate Tool */ }
    ImGui::SameLine();
    if (ImGui::Button("⇲")) { /* Scale Tool */ }

    ImGui::SameLine(0, 40); // 大きめにスペース

    if (ImGui::Button("▶")) { /* Play */ }
    ImGui::SameLine();
    if (ImGui::Button("⏸")) { /* Pause */ }
    ImGui::SameLine();
    if (ImGui::Button("⏭")) { /* Step */ }

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
