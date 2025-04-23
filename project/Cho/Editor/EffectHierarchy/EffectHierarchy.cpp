#include "pch.h"
#include "EffectHierarchy.h"
#include "Editor/EditorManager/EditorManager.h"

void EffectHierarchy::Initialize()
{
}

void EffectHierarchy::Update()
{
	Window();
}

void EffectHierarchy::Window()
{
	ImGui::Begin("Effect Hierarchy");
	ImGui::Text("Effect Hierarchy");
	ImGui::End();
}
