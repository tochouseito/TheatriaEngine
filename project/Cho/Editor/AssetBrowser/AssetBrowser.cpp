#include "pch.h"
#include "AssetBrowser.h"
#include "Editor/EditorManager/EditorManager.h"

void AssetBrowser::Initialize()
{
}

void AssetBrowser::Update()
{
	Window();
}

void AssetBrowser::Window()
{
	// 移動を無効にするフラグ
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;
	ImGui::Begin("AssetBrowser", nullptr, windowFlags);
	ImGui::End();
}
