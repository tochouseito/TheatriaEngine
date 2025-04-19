#include "pch.h"
#include "Hierarchy.h"
#include "Editor/EditorManager/EditorManager.h"
#include "EngineCommand/EngineCommand.h"
#include "EngineCommand/EngineCommands.h"
#include "GameCore/GameCore.h"
#include "Core/ChoLog/ChoLog.h"
#include <wchar.h>

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
	ScenePrefab* currentScene = m_EngineCommand->GetGameCore()->GetSceneManager()->GetCurrentScene();
	// シーンが存在しない場合は終了
	if (!currentScene)
	{
		ImGui::Text("No Scene Loaded");
		ImGui::End();
		return;
	}
	static bool editing = false;
	static wchar_t inputBuffer[128] = L""; // 入力用バッファ
	static std::wstring editingName;   // 現在編集中のオブジェクト名
	std::vector<ObjectID> currentSceneObjects = currentScene->GetUseObjects();
	// オブジェクトをリストで表示
	for (const ObjectID& objectID : currentSceneObjects)
	{
		// オブジェクトを取得
		GameObject& object = m_EngineCommand->GetGameCore()->GetObjectContainer()->GetGameObject(objectID);
		if (!object.IsActive()) { continue; }// オブジェクトが存在しない場合はスキップ
		if (editing && editingName == object.GetName())
		{
			// 編集モード: InputTextを表示
			std::string inputName = ConvertString(inputBuffer);
			char sInputBuffer[128] = "";
			strncpy_s(sInputBuffer, IM_ARRAYSIZE(sInputBuffer), inputName.c_str(), _TRUNCATE);
			if (ImGui::InputText("##edit", sInputBuffer, IM_ARRAYSIZE(sInputBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				std::string newName(sInputBuffer);
				std::wstring wNewName = ConvertString(newName);

				// 名前が空でない場合に更新
				if (!wNewName.empty())
				{
					// オブジェクトの名前を更新
					std::unique_ptr<RenameObjectCommand> renameCommand = std::make_unique<RenameObjectCommand>(object.GetID().value(), wNewName);
					m_EngineCommand->ExecuteCommand(std::move(renameCommand));

					editing = false;  // 編集終了
				}
			}

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemActive())
			{
				editing = false;
			}
		} else
		{
			// オブジェクトの名前を取得
			std::wstring objectName = object.GetName();
			// ツリーノードとして表示
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
			if (m_EngineCommand->GetSelectedObject()->GetID() == object.GetID())
			{
				flags |= ImGuiTreeNodeFlags_Selected; // 選択中のオブジェクトをハイライト
			}
			bool isTreeOpen = ImGui::TreeNodeEx(ConvertString(objectName).c_str(), flags);

			// 親ノードが左クリックされた場合の処理
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			{
				// 選択中のオブジェクトを更新
				m_EngineCommand->SetSelectedObject(&object);
			}
			// 右クリックされた場合の処理
			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				// 選択中のオブジェクトを更新
				m_EngineCommand->SetSelectedObject(&object);
				ImGui::OpenPopup("HierarchyPopupMenu");
			}
			// ダブルクリックで編集モードに切り替え
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				editing = true;
				editingName = objectName;                       // 編集中の名前を設定
				wcsncpy_s(inputBuffer, IM_ARRAYSIZE(inputBuffer), objectName.c_str(), _TRUNCATE); // 現在の名前をバッファにコピー
			}
			// ツリーノード展開処理
			if (isTreeOpen)
			{
				// 子ノードを表示する場合の処理
				ImGui::TreePop();
			}
		}
	}
	// ポップアップメニュー
	if (ImGui::BeginPopup("HierarchyPopupMenu"))
	{
		if (ImGui::MenuItem("削除"))
		{
			// 選択中のオブジェクトを削除
			std::unique_ptr<DeleteObjectCommand> deleteCommand = std::make_unique<DeleteObjectCommand>(m_EngineCommand->GetSelectedObject()->GetID().value());
			m_EngineCommand->ExecuteCommand(std::move(deleteCommand));
			// 選択中オブジェクトをクリア
			m_EngineCommand->SetSelectedObject(nullptr);
			// ポップアップメニューを閉じる
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::End();
}
