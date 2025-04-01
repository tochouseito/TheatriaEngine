#include "pch.h"
#include "Inspector.h"
#include "Editor/EditorManager/EditorManager.h"

void Inspector::Initialize()
{
}

void Inspector::Update()
{
	Window();
}

void Inspector::Window()
{
	// 移動を無効にするフラグ
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;
	ImGui::Begin("Inspector", nullptr, windowFlags);
	ImGui::End();
}
