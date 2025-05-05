#include "pch.h"
#include "EffectEditor.h"
#include "Editor/EditorManager/EditorManager.h"
#include "EngineCommand/EngineCommand.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "GameCore/GameCore.h"
#include "Core/Utility/FontCode.h"

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

    // nodeが選択されているか
	uint32_t nodeIndex = 0;
    if (m_EngineCommand->GetEffectNode().has_value())
    {
		nodeIndex = m_EngineCommand->GetEffectNode().value();
    } else
    {
		ImGui::Text("No Node");
		ImGui::End();
        return;
    }
	// nodeの情報を取得
	EffectNodeData& node = effect->nodeData[nodeIndex];

    ImGui::Separator();
    if (ImGui::BeginTabBar("Node"))
    {
        // Common
		if (ImGui::BeginTabItem("Common"))
		{
            // 名前表示
            char nameBuffer[128];
            strncpy_s(nameBuffer, sizeof(nameBuffer), node.name.c_str(), _TRUNCATE);
            if (ImGui::InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer)))
            {
                node.name = std::string(nameBuffer);
            }
            // 生成数
            int emitCount = static_cast<int>(node.common.emitCount);
			if (ImGui::DragInt("生成数", &emitCount, 1, 0, 1024))
			{
				node.common.emitCount = static_cast<uint32_t>(emitCount);
			}
            // 無限生成フラグ
            bool isUnlimited = node.common.isUnlimited != 0 ? true : false;
            ImGui::Checkbox("無限生成", &isUnlimited);
			node.common.isUnlimited = isUnlimited ? 1 : 0;
            // 位置影響タイプ
            // 回転影響タイプ
			// スケール影響タイプ
            // 寿命で削除フラグ
			// 親削除で削除フラグ
			// 全ての子削除で削除フラグ
            // 生存時間
			ImGui::DragFloat2("生存時間", &node.common.lifeTime.median, 0.1f, 0.0f, 0.0f);
            // 生成時間
			ImGui::DragFloat2("生成時間", &node.common.emitTime.median, 0.1f, 0.0f, 0.0f);
			// 生成開始時間
			ImGui::DragFloat2("生成開始時間", &node.common.emitStartTime.median, 0.1f, 0.0f, 0.0f);

            // End
			ImGui::EndTabItem();
		}
        // Position
        if (ImGui::BeginTabItem("位置"))
        {
            // PositionType
            static const char* positionTypeStrings[] = {
                "Standard",     // PositionStandard
                "PVA",          // PositionPVA
                //"Easing",       // PositionEasing
                //"FCurve",       // PositionFCurve
                //"NURBS",        // PositionNURBS
                //"CameraOffset", // PositionCameraOffset
            };
            int positionType = static_cast<int>(node.position.type);
            if (ImGui::Combo("Position Type", &positionType, positionTypeStrings, IM_ARRAYSIZE(positionTypeStrings)))
            {
                node.position.type = static_cast<uint32_t>(positionType);
            }
			// Position
			EFFECT_SRT_TYPE type = static_cast<EFFECT_SRT_TYPE>(node.position.type);
            switch (type)
            {
            case EFFECT_SRT_TYPE::SRT_TYPE_STANDARD:
				ImGui::DragFloat3("位置", &node.position.value.x, 0.1f);
                break;
                break;
            case EFFECT_SRT_TYPE::SRT_TYPE_PVA:
                break;
            case EFFECT_SRT_TYPE::SRT_TYPE_EASING:
                break;
            default:
                break;
            }

            // End
			ImGui::EndTabItem();
        }

        // End
		ImGui::EndTabBar();
    }

	ImGui::Text("Effect Editor");
	ImGui::End();
}

void EffectEditor::ControlWindow()
{
	ImGui::Begin("EffectControl");

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

    // 中央に配置するための計算
    float toolbarWidth = 0.0f;
    float buttonSize = 28.0f; // ボタンサイズ
    float buttonSpacing = 8.0f; // ボタン間のスペース
    int buttonCount = 3;
    toolbarWidth = buttonCount * buttonSize + (buttonCount - 1) * buttonSpacing;

    // ウィンドウ中央位置に移動
    float availableWidth = ImGui::GetContentRegionAvail().x;
    float startX = (availableWidth - toolbarWidth) * 0.5f;
    ImGui::SetCursorPosX(startX);

    // ゲーム実行中なら
    if (effect->isRun)
    {
        // ボタンを描画（中央に並べる）
        if (ImGui::Button(ICON_FA_PAUSE, ImVec2(buttonSize, buttonSize)))
        {
            /* Pause */
            effect->isRun = false;
        }
    } else // 実行中じゃないなら
    {
        // ボタンを描画（中央に並べる）
        if (ImGui::Button(ICON_FA_PLAY, ImVec2(buttonSize, buttonSize)))
        {
            // Play
            effect->isRun = true;
        }
    }
    if (ImGui::Button("Reset"))
    {
		effect->isReset = true;
    }
	ImGui::End();
}
