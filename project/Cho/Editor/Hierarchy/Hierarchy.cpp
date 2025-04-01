#include "pch.h"
#include "Hierarchy.h"
#include "Editor/EditorManager/EditorManager.h"

void Hierarchy::Initialize()
{
}

void Hierarchy::Update()
{
	Window();
}

void Hierarchy::Window()
{
	// 移動を無効にするフラグ
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;
	ImGui::Begin("Hierarchy", nullptr, windowFlags);
	ImGui::End();
}
