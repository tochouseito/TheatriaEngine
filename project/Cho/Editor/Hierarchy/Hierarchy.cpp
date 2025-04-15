#include "pch.h"
#include "Hierarchy.h"
#include "Editor/EditorManager/EditorManager.h"
#include "GameCore/GameCore.h"
#include "Core/ChoLog/ChoLog.h"

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

	// 現在のシーンのオブジェクトリストを取得
	// シーンを取得
	ScenePrefab* currentScene = m_EditorCommand->GetGameCoreCommandPtr()->GetSceneManagerPtr()->GetCurrentScene();
	// シーンが存在しない場合は終了
	if (!currentScene)
	{
		ImGui::Text("No Scene Loaded");
		ImGui::End();
		return;
	}
	std::vector<ObjectID> currentSceneObjects = currentScene->GetUseObjects();
	// オブジェクトをリストで表示
	for (const ObjectID& objectID : currentSceneObjects)
	{
		// オブジェクトを取得
		GameObject* object = m_EditorCommand->GetGameCoreCommandPtr()->GetObjectContainerPtr()->GetGameObject(objectID);
		// オブジェクトの名前を取得
		std::wstring objectName = object->GetName();

		// ツリーノードとして表示
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
		if (m_EditorCommand->GetSelectedObject()==object)
		{
			flags |= ImGuiTreeNodeFlags_Selected; // 選択中のオブジェクトをハイライト
		}
		bool isTreeOpen = ImGui::TreeNodeEx(ConvertString(objectName).c_str(), flags);
		
		// 親ノードが左クリックされた場合の処理
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			// 選択中のオブジェクトを更新
			m_EditorCommand->SetSelectedObject(object);
		}
		// 右クリックされた場合の処理
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			// 選択中のオブジェクトを更新
			m_EditorCommand->SetSelectedObject(object);
			ImGui::OpenPopup("HierarchyPopupMenu");
		}

		// ポップアップメニュー
		if (ImGui::BeginPopup("HierarchyPopupMenu"))
		{
			if (ImGui::MenuItem("削除"))
			{
				// 選択中のオブジェクトを削除
				//m_EditorCommand->GetGameCoreCommandPtr()->GetObjectContainerPtr()->DeleteGameObject(object);
				// ポップアップメニューを閉じる
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("名前変更"))
			{
				// 名前変更処理

				// ポップアップメニューを閉じる
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}


		// ツリーノード展開処理
		if (isTreeOpen)
		{
			// 子ノードを表示する場合の処理
			ImGui::TreePop();
		}
	}

	ImGui::End();
}
