#include "pch.h"
#include "EffectEditor.h"
#include "Editor/EditorManager/EditorManager.h"

void EffectEditor::Initialize()
{
}

void EffectEditor::Update()
{
	Window();
	ControlWindow();
}

void EffectEditor::Window()
{
	ImGui::Begin("Effect Editor");
	ImGui::Text("Effect Editor");
	ImGui::End();
}

void EffectEditor::ControlWindow()
{
	ImGui::Begin("EffectControl");
	ImGui::Text("EffectControl");
	ImGui::End();
}
