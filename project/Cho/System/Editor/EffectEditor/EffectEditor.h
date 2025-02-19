#pragma once

#include<cstdint>
#include<Windows.h>

class WinApp;
class DebugCamera;
class ResourceViewManager;
class InputManager;
class EffectEditor
{
public:
	void Initialize(const uint32_t& index, WinApp* win, ResourceViewManager* rvManager, InputManager* inputManager, DebugCamera* debugCamera);
	void Update(const uint32_t& cameraIndex);
private:
	
private:
	// ポインタ
	WinApp* win_ = nullptr;
	ResourceViewManager* rvManager_ = nullptr;
	DebugCamera* debugCamera_ = nullptr;
	InputManager* inputManager_ = nullptr;
};

