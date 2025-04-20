#include "pch.h"
#include "EffectView.h"
#include "Editor/EditorManager/EditorManager.h"

void EffectView::Initialize()
{
}

void EffectView::Update()
{
	Window();
}

void EffectView::Window()
{
	ImGui::Begin("EffectView");
	ImGui::Text("EffectView");
	ImGui::End();
}
