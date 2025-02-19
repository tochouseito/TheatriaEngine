#include "PrecompiledHeader.h"
#include "EffectEditor.h"

#include"imgui.h"

#include"WinApp/WinApp.h"
#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"Editor/DebugCamera/DebugCamera.h"
#include"Input/InputManager.h"
#include"SystemState/SystemState.h"

// C++
#include<algorithm>

void EffectEditor::Initialize(const uint32_t& index, WinApp* win, ResourceViewManager* rvManager, InputManager* inputManager, DebugCamera* debugCamera)
{
	index;
	win_ = win;
	rvManager_ = rvManager;
	inputManager_ = inputManager;
	debugCamera_ = debugCamera;
}

void EffectEditor::Update(const uint32_t& cameraIndex)
{
	cameraIndex;
}
