#include "pch.h"
#include "Inspector.h"
#include "Editor/EditorManager/EditorManager.h"
#include "GameCore/GameCore.h"
#include "Core/ChoLog/ChoLog.h"

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

	// 選択中のオブジェクトがある場合に情報を表示
	if (m_EditorCommand->GetSelectedObject())
	{
		// 選択中のオブジェクトを取得
		GameObject* object = m_EditorCommand->GetSelectedObject();
		// オブジェクトの名前を取得
		std::wstring objectName = object->GetName();
		// オブジェクトのタイプを取得
		ObjectType objectType = object->GetType();
		// オブジェクト名とタイプを表示
		ImGui::Text("%s (%s)", ConvertString(objectName).c_str(), ObjectTypeToWString(objectType));
	}

	ImGui::End();
}
