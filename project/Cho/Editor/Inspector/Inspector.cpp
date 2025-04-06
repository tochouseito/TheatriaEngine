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
		ImGui::Text("Name: %s", ConvertString(objectName).c_str());
		// タイプを表示
		ImGui::Text("Type: %s", ObjectTypeToWString(objectType));
		// コンポーネントの情報を表示
		ComponentsView(object);
		// コンポーネントの追加ボタンを表示
		AddComponent(object);
	}

	ImGui::End();
}

void Inspector::ComponentsView(GameObject* object)
{
	TransformComponentView(object);
	MeshFilterComponentView(object);
	MeshRendererComponentView(object);
	CameraComponentView(object);
}

void Inspector::TransformComponentView(GameObject* object)
{
	// TransformComponentを取得
	TransformComponent* transform = m_EditorCommand->GetGameCoreCommandPtr()->GetECSManagerPtr()->GetComponent<TransformComponent>(object->GetEntity());
	if (transform)
	{
		ImGui::Text("Transform Component");
	} 
}

void Inspector::MeshFilterComponentView(GameObject* object)
{
	// メッシュオブジェクトじゃないならスキップ
	if (object->GetType() != ObjectType::MeshObject)
	{
		return;
	}
	// MeshFilterComponentを取得
	MeshFilterComponent* mesh = m_EditorCommand->GetGameCoreCommandPtr()->GetECSManagerPtr()->GetComponent<MeshFilterComponent>(object->GetEntity());
	if (mesh)
	{
		ImGui::Text("Mesh Component");
	}
}

void Inspector::MeshRendererComponentView(GameObject* object)
{
	// メッシュオブジェクトじゃないならスキップ
	if (object->GetType() != ObjectType::MeshObject)
	{
		return;
	}
	// MeshRendererComponentを取得
	MeshRendererComponent* render = m_EditorCommand->GetGameCoreCommandPtr()->GetECSManagerPtr()->GetComponent<MeshRendererComponent>(object->GetEntity());
	if (render)
	{
		ImGui::Text("Render Component");
	}
}

void Inspector::CameraComponentView(GameObject* object)
{
	// カメラオブジェクトじゃないならスキップ
	if (object->GetType() != ObjectType::Camera)
	{
		return;
	}
	// CameraComponentを取得
	CameraComponent* camera = m_EditorCommand->GetGameCoreCommandPtr()->GetECSManagerPtr()->GetComponent<CameraComponent>(object->GetEntity());
	if (camera)
	{
		ImGui::Text("Camera Component");
	}
}

void Inspector::AddComponent(GameObject* object)
{
	static bool isOpen = false;
	MeshFilterComponent* mesh;
	MeshRendererComponent* render;
	if (!isOpen)
	{
		// コンポーネントの追加
		if (ImGui::Button("Add Component"))
		{
			isOpen = true;
		}
	} else
	{
		ObjectType objectType = object->GetType();
		switch (objectType)
		{
		case ObjectType::MeshObject:
			// MeshFilterComponentがあるか
			mesh = m_EditorCommand->GetGameCoreCommandPtr()->GetECSManagerPtr()->GetComponent<MeshFilterComponent>(object->GetEntity());
			if (!mesh)
			{
				if (ImGui::Selectable("MeshFilterComponent"))
				{
					// MeshFilterComponentを追加
					std::unique_ptr<AddMeshFilterComponent> addMeshComp = std::make_unique<AddMeshFilterComponent>(object->GetEntity());
					m_EditorCommand->ExecuteCommand(std::move(addMeshComp));
					isOpen = false;
				}
			}
			// MeshRendererComponentがあるか
			render = m_EditorCommand->GetGameCoreCommandPtr()->GetECSManagerPtr()->GetComponent<MeshRendererComponent>(object->GetEntity());
			if (!render)
			{
				if (ImGui::Selectable("MeshRendererComponent"))
				{
					// MeshRendererComponentを追加
					std::unique_ptr<AddMeshRendererComponent> addRenderComp = std::make_unique<AddMeshRendererComponent>(object->GetEntity());
					m_EditorCommand->ExecuteCommand(std::move(addRenderComp));
					isOpen = false;
				}
			}
			break;
		case ObjectType::Camera:
			break;
		case ObjectType::Count:
			break;
		default:
			break;
		}
	}
}
