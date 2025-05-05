#include "pch.h"
#include "EffectHierarchy.h"
#include "Editor/EditorManager/EditorManager.h"
#include "EngineCommand/EngineCommand.h"
#include "GameCore/GameCore.h"

void EffectHierarchy::Initialize()
{
}

void EffectHierarchy::Update()
{
	Window();
}

void EffectHierarchy::Window()
{
	// 移動を無効にするフラグ
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;
	ImGui::Begin("Effect Hierarchy", nullptr, windowFlags);

	if (!m_EngineCommand->GetEffectEntity().has_value())
	{
		ImGui::Text("No Root");
		ImGui::End();
		return;
	} else
	{
		ImGui::Text("Effect Root");
	}
	EffectComponent* effect = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<EffectComponent>(m_EngineCommand->GetEffectEntity().value());
	if (!effect)
	{
		ImGui::Text("No Effect");
		ImGui::End();
		return;
	}
	for (auto& node : effect->nodeData)
	{
		// ツリーノードとして表示
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
		if (m_EngineCommand->GetEffectNode() && m_EngineCommand->GetEffectNode().value() == node.id)
		{
			flags |= ImGuiTreeNodeFlags_Selected; // 選択中のオブジェクトをハイライト
		}
		bool isTreeOpen = ImGui::TreeNodeEx(node.name.c_str(), flags);

		// 親ノードが左クリックされた場合の処理
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			// 選択中のオブジェクトを更新
			m_EngineCommand->SetEffectNode(node.id);
		}
		// 右クリックされた場合の処理
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			// 選択中のオブジェクトを更新
			m_EngineCommand->SetEffectNode(node.id);
			ImGui::OpenPopup("HierarchyPopupMenu");
		}
		// ダブルクリックで編集モードに切り替え
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			//editing = true;
			//editingName = objectName;                       // 編集中の名前を設定
			//wcsncpy_s(inputBuffer, IM_ARRAYSIZE(inputBuffer), objectName.c_str(), _TRUNCATE); // 現在の名前をバッファにコピー
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
