#include "pch.h"
#include "ImGuiManager.h"
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include "OS/Windows/WinApp/WinApp.h"
#include "Resources/ResourceManager/ResourceManager.h"

void ImGuiManager::Initialize(ID3D12Device8* device, ResourceManager* resourceManager)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Dockingを有効化
	// 日本語フォント
	ImFontConfig font_config;
	font_config.MergeMode = false;
	font_config.PixelSnapH = true;
	io.Fonts->AddFontFromFileTTF(//"C:/Windows/Fonts/msgothic.ttc",
		"Cho/Resources/EngineAssets/Fonts/NotoSansJP-Regular.ttf",// フォントファイルのパス
		16.0f,// フォントファイルのパスとフォントサイズ
		&font_config, io.Fonts->GetGlyphRangesJapanese()// フォントの範囲
	);
	// アイコンフォントをマージ
	font_config.MergeMode = true;
	static const ImWchar icon_ranges[] = { 0xf000, 0xf3ff, 0 }; // FontAwesomeの範囲
	io.Fonts->AddFontFromFileTTF(
		"Cho/Resources/EngineAssets/Fonts/Font Awesome 6 Free-Solid-900.otf",
		14.0f,
		&font_config, icon_ranges);
	unsigned char* texPixels = nullptr;
	int texWidth, texHeight;
	io.Fonts->GetTexDataAsAlpha8(&texPixels, &texWidth, &texHeight);
	// 標準フォントを追加する
	io.Fonts->AddFontDefault();
	//io.Fonts->Build();
	// ImGuiのスタイルを設定
	ImGui::StyleColorsDark();

	// プラットフォームとレンダラーのバックエンドを設定する
	ImGui_ImplWin32_Init(WinApp::GetHWND());
	m_DescriptorHeapIndex = resourceManager->GetSUVDHeap()->Allocate();
	ImGui_ImplDX12_Init(
		device, 2,
		PixelFormat, resourceManager->GetSUVDHeap()->GetDescriptorHeap(),
		resourceManager->GetSUVDHeap()->GetCPUDescriptorHandle(m_DescriptorHeapIndex.value()),
		resourceManager->GetSUVDHeap()->GetGPUDescriptorHandle(m_DescriptorHeapIndex.value())
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
