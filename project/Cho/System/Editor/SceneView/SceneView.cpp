#include "PrecompiledHeader.h"
#include "SceneView.h"

#include"imgui.h"

#include"WinApp/WinApp.h"
#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"Editor/DebugCamera/DebugCamera.h"
#include"Input/InputManager.h"
#include"SystemState/SystemState.h"

// C++
#include<algorithm>

void SceneView::Initialize(const uint32_t& index,WinApp* win, ResourceViewManager* rvManager, InputManager* inputManager, DebugCamera* debugCamera)
{
	sceneTextureIndex = index;
    win_ = win;
	rvManager_ = rvManager;
	inputManager_ = inputManager;
    debugCamera_ = debugCamera;
}

void SceneView::Update(const uint32_t& cameraIndex)
{
    // シーンビュー（ゲームカメラ）のビュー
    GameView(cameraIndex);

    // デバッグ用ビュー
    DebugView();
}

void SceneView::DebugView()
{
    // ウィンドウのパディングをゼロに設定
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    // 移動を無効にするフラグ
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    // ウィンドウの開始
    ImGui::Begin("Debug View", nullptr, windowFlags);

    // ウィンドウ内で利用可能な領域のサイズを取得
    ImVec2 availableSize = ImGui::GetContentRegionAvail();

	//ImVec2 windowPos = ImGui::GetWindowPos(); // 現在のImGuiウィンドウの位置を取得
	//float diffY = ImGui::GetWindowSize().y- ImGui::GetContentRegionAvail().y;
	//windowPos.y += diffY;
	//ImVec2 windowSize = ImGui::GetContentRegionAvail(); // 現在のImGuiウィンドウのサイズを取得

    Vector2 vec = CheckAndWarpMouseInImGuiWindow();
    /*if (vec.x != 0.0f || vec.y != 0.0f) {
        
    }*/
    debugCamera_->SetCurrentMousePos(vec);
    // アスペクト比を計算してカメラに設定
    float newAspect = availableSize.x / availableSize.y;
    debugCamera_->SetAspect(newAspect);

    // テクスチャを描画
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = rvManager_->GetHandle(sceneTextureIndex).GPUHandle;
    ImTextureID textureID = (ImTextureID)srvHandle.ptr;

    ImVec4 tintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // アルファ値を無視
    ImVec4 borderColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    ImGui::Image(textureID, availableSize,ImVec2(0, 0), ImVec2(1, 1), tintColor, borderColor);

    // "Debug View" 上にカーソルがあるとき、右クリックメニューを開かないようにする
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)&&!excludeRightClickMenu) {
        excludeRightClickMenu = true;
        if (inputManager_->IsPressMouse(Left) ||
            inputManager_->IsPressMouse(Right) ||
            inputManager_->IsPressMouse(Center)) 
        {
            if (inputManager_->IsTriggerMouse(Left) ||
                inputManager_->IsTriggerMouse(Right) ||
                inputManager_->IsTriggerMouse(Center))
            {
                // 初回クリック時にロック位置を決定
                GetCursorPos(&lockPosition);
                prevCursorPos = lockPosition;
				ShowCursor(false);
            }
			windowClick = true;
            setMousePos = true;
        }
        else {
			windowClick = false;
        }
    }
    else {
        excludeRightClickMenu = false;
    }

    ImGui::End();

    // スタイルを元に戻す
    ImGui::PopStyleVar();
}

void SceneView::GameView(const uint32_t& cameraIndex)
{
    // ウィンドウのパディングをゼロに設定
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    // 移動を無効にするフラグ
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    // ウィンドウの開始
    ImGui::Begin("Scene View", nullptr, windowFlags);

    // ウィンドウ内の描画可能領域を取得
    ImVec2 contentRegion = ImGui::GetContentRegionAvail();

    // 固定解像度のテクスチャサイズ
    ImVec2 textureResolution = {
        static_cast<float>(WindowWidth()),
        static_cast<float>(WindowHeight())
    };

    // アスペクト比を維持しつつ、ウィンドウ内の描画可能領域に収まるようにスケーリング係数を計算
    float scaleX = contentRegion.x / textureResolution.x;
    float scaleY = contentRegion.y / textureResolution.y;
    float scale = std::min(scaleX, scaleY);

    // スケーリング後のテクスチャサイズを計算
    ImVec2 scaledTextureSize = ImVec2(textureResolution.x * scale, textureResolution.y * scale);

    // 描画位置のオフセットを計算（ウィンドウの中央に配置するため）
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    float offsetX = cursorPos.x + (contentRegion.x - scaledTextureSize.x) * 0.5f;
    float offsetY = cursorPos.y + (contentRegion.y - scaledTextureSize.y) * 0.5f;

    // カーソル位置を設定し、スクリーン上にテクスチャを中央に描画
    ImGui::SetCursorScreenPos(ImVec2(offsetX, offsetY));

    // テクスチャの描画
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = rvManager_->GetHandle(cameraIndex).GPUHandle;
    ImTextureID textureID = (ImTextureID)srvHandle.ptr;

    ImVec4 tintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // アルファ値を無視
    ImVec4 borderColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    ImGui::Image(textureID, scaledTextureSize, ImVec2(0, 0), ImVec2(1, 1), tintColor, borderColor);

    // "Scene View" 上にカーソルがあるとき、右クリックメニューを開かないようにする
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)) {
        excludeRightClickMenu = true;
    }
    else {
        excludeRightClickMenu = false;
    }

    // ウィンドウの終了
    ImGui::End();

    // スタイルを元に戻す
    ImGui::PopStyleVar();
}

Vector2 SceneView::CheckAndWarpMouseInImGuiWindow()
{
    if (windowClick) {
        // 現在のカーソル位置を取得
        POINT cursorPos;
        GetCursorPos(&cursorPos);

        // マウスの移動量を計算
        float deltaX = static_cast<float>(cursorPos.x - prevCursorPos.x);
        float deltaY = static_cast<float>(cursorPos.y - prevCursorPos.y);

        // カメラ操作用に移動量を補正
        Vector2 result = { deltaX, deltaY };

        // カーソルをロック位置に戻す
        SetCursorPos(lockPosition.x, lockPosition.y);
        prevCursorPos = lockPosition;
        return result;
    }
    else {
        // 操作終了時にカーソルを元の位置に戻す
        if (setMousePos) {
            SetCursorPos(lockPosition.x, lockPosition.y);
			ShowCursor(true);
            setMousePos = false;
        }
    }
	return { 0.0f,0.0f };
}



