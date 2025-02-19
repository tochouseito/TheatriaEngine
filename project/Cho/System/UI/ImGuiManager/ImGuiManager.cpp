#include "PrecompiledHeader.h"
#include "ImGuiManager.h"
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

#include"WinApp/WinApp.h"
#include"D3D12/D3DDevice/D3DDevice.h"
#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"D3D12/D3DCommand/D3DCommand.h"
#include"Base/Format.h"

void ImGuiManager::Initialize(WinApp* win, D3DDevice* d3dDevice, D3DCommand* d3dCommand, ResourceViewManager* RVManager)
{
	d3dCommand_ = d3dCommand;

	IMGUI_CHECKVERSION();
	// ImGuiのコンテキストを生成
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Dockingを有効化
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	// フォントファイルのパスとサイズを指定してフォントをロードする
	io.Fonts->AddFontFromFileTTF(
		"C:\\Windows\\Fonts\\Arial.ttf", 16.0f // フォントファイルのパスとフォントサイズ
	);
	// 標準フォントを追加する
	io.Fonts->AddFontDefault();
	// ImGuiのスタイルを設定
	ImGui::StyleColorsDark();

	// プラットフォームとレンダラーのバックエンドを設定する
	ImGui_ImplWin32_Init(win->GetHwnd());
	ImGui_ImplDX12_Init(
		d3dDevice->GetDevice(), 2,
		dxgiFormat, RVManager->GetDescriptorHeap(),
		RVManager->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		RVManager->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
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

void ImGuiManager::Draw()
{
	ID3D12GraphicsCommandList6* commandList = d3dCommand_->GetCommand(CommandType::Draw).list.Get();

	// 描画コマンドを発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}
