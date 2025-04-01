#include "pch.h"
#include "GameView.h"
#include "Editor/EditorManager/EditorManager.h"

void GameView::Initialize()
{
}

void GameView::Update()
{
	Window();
}

void GameView::Window()
{
	// ウィンドウのパディングをゼロに設定
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	// 移動を無効にするフラグ
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::Begin("GameView", nullptr, windowFlags);
	ImGui::End();
	// スタイルを元に戻す
	ImGui::PopStyleVar();
}
