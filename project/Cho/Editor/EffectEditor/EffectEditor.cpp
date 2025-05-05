#include "pch.h"
#include "EffectEditor.h"
#include "Editor/EditorManager/EditorManager.h"
#include "EngineCommand/EngineCommand.h"
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
	ImGui::End();
}
