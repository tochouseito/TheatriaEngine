#pragma once

#include<cstdint>
#include<Windows.h>

class WinApp;
class DebugCamera;
class ResourceViewManager;
class InputManager;
class SceneView
{
public:
	void Initialize(const uint32_t& index,WinApp* win, ResourceViewManager* rvManager,InputManager* inputManager,DebugCamera* debugCamera);

	void Update(const uint32_t& cameraIndex);

	bool IsWindowHovered()const { return excludeRightClickMenu; }

	bool IsWindowClicked()const { return windowClick; }

private:

	void DebugView();

	void GameView(const uint32_t& cameraIndex);

    Vector2 CheckAndWarpMouseInImGuiWindow();

private:
	// ポインタ
	WinApp* win_ = nullptr;
	ResourceViewManager* rvManager_ = nullptr;
	DebugCamera* debugCamera_ = nullptr;
	InputManager* inputManager_ = nullptr;

	uint32_t sceneTextureIndex;

	POINT lockPosition = { 0, 0 }; // ロックする位置
	POINT prevCursorPos = { 0, 0 }; // 前フレームのカーソル位置

	// このウィンドウの右クリックでのポップアップウィンドウを無効にするためのフラグ
	bool excludeRightClickMenu = false;

	bool windowClick = false;
	bool setMousePos = false;
};

