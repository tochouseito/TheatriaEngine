#include "pch.h"
#include "AssetBrowser.h"
#include "Editor/EditorManager/EditorManager.h"
#include "Core/Utility/FontCode.h"

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

	//ImGui::Text("\ue88a");
	//ImGui::Text(ICON_MATERIAL_MODEL);
	//ImGui::Text(ICON_MATERIAL_IMAGE);
	//ImGui::Text(ICON_MATERIAL_AUDIO);
	//ImGui::Text("\ue86f  スクリプト");
	//ImGui::Text("\ue30a  スクリプト");
	ImGui::Text(ICON_MATERIAL_EFFECT);
	//ImGui::Text(ICON_MATERIAL_SETTING);
	//ImGui::Text(ICON_MATERIAL_SCENE);

	ImGui::End();
}
