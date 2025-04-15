#include "pch.h"
#include "SceneView.h"
#include "Editor/EditorManager/EditorManager.h"

void SceneView::Initialize()
{
}

void SceneView::Update()
{
    Window();
}

void SceneView::Window()
{
    // ウィンドウのパディングをゼロに設定
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    // 移動を無効にするフラグ
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    // ウィンドウの開始
    ImGui::Begin("Scene", nullptr, windowFlags);

    // ウィンドウ内で利用可能な領域のサイズを取得
    ImVec2 availableSize = ImGui::GetContentRegionAvail();

    //ImVec2 windowPos = ImGui::GetWindowPos(); // 現在のImGuiウィンドウの位置を取得
    //float diffY = ImGui::GetWindowSize().y- ImGui::GetContentRegionAvail().y;
    //windowPos.y += diffY;
    //ImVec2 windowSize = ImGui::GetContentRegionAvail(); // 現在のImGuiウィンドウのサイズを取得

    //Vector2 vec = CheckAndWarpMouseInImGuiWindow();
    /*if (vec.x != 0.0f || vec.y != 0.0f) {

    }*/
    //debugCamera_->SetCurrentMousePos(vec);
    // アスペクト比を計算してカメラに設定
    float newAspect = availableSize.x / availableSize.y;
	m_DebugCamera->SetAspect(newAspect);

    // テクスチャを描画
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = m_EngineCommand->GetSceneTextureHandle();
    ImTextureID textureID = (ImTextureID)srvHandle.ptr;

    ImVec4 tintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // アルファ値を無視
    ImVec4 borderColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    ImGui::Image(textureID, availableSize, ImVec2(0, 0), ImVec2(1, 1), tintColor, borderColor);

    //// "Debug View" 上にカーソルがあるとき、右クリックメニューを開かないようにする
    //if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && !excludeRightClickMenu)
    //{
    //    excludeRightClickMenu = true;
    //    if (inputManager_->IsPressMouse(Left) ||
    //        inputManager_->IsPressMouse(Right) ||
    //        inputManager_->IsPressMouse(Center))
    //    {
    //        if (inputManager_->IsTriggerMouse(Left) ||
    //            inputManager_->IsTriggerMouse(Right) ||
    //            inputManager_->IsTriggerMouse(Center))
    //        {
    //            // 初回クリック時にロック位置を決定
    //            GetCursorPos(&lockPosition);
    //            prevCursorPos = lockPosition;
    //            ShowCursor(false);
    //        }
    //        windowClick = true;
    //        setMousePos = true;
    //    } else
    //    {
    //        windowClick = false;
    //    }
    //} else
    //{
    //    excludeRightClickMenu = false;
    //}

    // ウィンドウ上にカーソルがあるときにデバッグカメラを更新する
    if (ImGui::IsWindowHovered())
    {
        m_DebugCamera->Update();
    }

    ImGui::End();

    // スタイルを元に戻す
    ImGui::PopStyleVar();
}