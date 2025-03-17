#include "pch.h"
#include "ImGuiManager.h"
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include "OS/Windows/WinApp/WinApp.h"
#include "Cho/Resources/ResourceManager/ResourceManager.h"

void ImGuiManager::Initialize(ID3D12Device8* device, ResourceManager* resourceManager)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Dockingを有効化
	io.Fonts->AddFontFromFileTTF(
		"C:\\Windows\\Fonts\\Arial.ttf", 16.0f // フォントファイルのパスとフォントサイズ
	);
	// 標準フォントを追加する
	io.Fonts->AddFontDefault();
	// ImGuiのスタイルを設定
	ImGui::StyleColorsDark();

	// プラットフォームとレンダラーのバックエンドを設定する
	ImGui_ImplWin32_Init(WinApp::GetHWND());
	resourceManager->GetSUVDHeap()->Create();
	ImGui_ImplDX12_Init(
		device, 2,
		PixelFormat, resourceManager->GetSUVDHeap()->GetDescriptorHeap(),
		resourceManager->GetSUVDHeap()->GetCpuHandle(m_DHandleIndex),
		resourceManager->GetSUVDHeap()->GetGpuHandle(m_DHandleIndex)
	);
}

void ImGuiManager::Finalize()
{
	// 後始末
	//ImGui::DestroyPlatformWindows();
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::Begin()
{
	// ImGuiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::End()
{
	// 描画前準備
	ImGui::Render();
	//ImGui::UpdatePlatformWindows();
	//ImGui::RenderPlatformWindowsDefault();
}

void ImGuiManager::Draw(ID3D12GraphicsCommandList6* commandList)
{
	// 描画コマンドを発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}
