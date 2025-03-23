#include "pch.h"
#include "GraphicsEngine.h"
#include "OS/Windows/WinApp/WinApp.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Cho/SDK/ImGui/ImGuiManager/ImGuiManager.h"

void GraphicsEngine::Init(IDXGIFactory7* dxgiFactory)
{
	// SwapChainの生成
	CreateSwapChain(dxgiFactory);
	// DepthBufferの生成
	CreateDepthBuffer();
	// オフスクリーンレンダリング用のリソースを作成
	CreateOffscreenBuffer();
	// パイプラインの生成
	m_PipelineManager->Initialize(m_Device);
}

void GraphicsEngine::PreRender()
{
	// コマンドマネージャー
	CommandManager* commandManager = m_GraphicsCore->GetCommandManager();
	// コマンドリストの取得
	CommandContext* context = commandManager->GetCommandContext();
	context->Reset();
	context->SetDescriptorHeap(m_ResourceManager->GetSUVDHeap()->GetDescriptorHeap());

	ColorBuffer* offScreenTex = m_ResourceManager->GetBufferManager()->GetColorBuffer(m_RenderTextures[RenderTextureType::OffScreen].m_BufferID);
	context->BarrierTransition(
		offScreenTex->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	
	// コマンドリストのクローズ
	context->Close();
	// コマンドリストの実行
	commandManager->ExecuteCommandList(context->GetCommandList(), QueueType::Graphics);
	// SwapChainのPresent
	m_SwapChain->Present();
	// シグナル
	commandManager->Signal(QueueType::Graphics);
	// GPUの完了待ち
	commandManager->WaitForFence(QueueType::Graphics);
	// コマンドリストの返却
	commandManager->ReturnCommandContext(context);
}

void GraphicsEngine::Render()
{
	
}

void GraphicsEngine::PostRender()
{
	// コマンドマネージャー
	CommandManager* commandManager = m_GraphicsCore->GetCommandManager();
	// コマンドリストの取得
	CommandContext* context = commandManager->GetCommandContext();
	context->Reset();
	context->SetDescriptorHeap(m_ResourceManager->GetSUVDHeap()->GetDescriptorHeap());
	// SwapChainのBackBufferIndexを取得
	UINT backBufferIndex = m_SwapChain->GetBackBufferIndex();
	// SwapChainResourceの状態遷移
	context->BarrierTransition(
		m_SwapChain->GetBackBuffer(backBufferIndex),
		D3D12_RESOURCE_STATE_PRESENT, 
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	// RTVの設定
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_ResourceManager->GetRTVDHeap()->GetCpuHandle(backBufferIndex);
	context->SetRenderTarget(rtvHandle);
	context->ClearRenderTarget(rtvHandle);
	// ビューポートの設定
	D3D12_VIEWPORT viewport =
		D3D12_VIEWPORT(
			0.0f, 0.0f,
			static_cast<float>(WinApp::GetWindowWidth()),
			static_cast<float>(WinApp::GetWindowHeight()),
			0.0f, 1.0f
		);
	context->SetViewport(viewport);
	// シザーレクトの設定
	D3D12_RECT rect = D3D12_RECT(
		0, 0,
		WinApp::GetWindowWidth(),
		WinApp::GetWindowHeight()
	);
	context->SetScissorRect(rect);
	// プリミティブトポロジの設定
	context->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// レンダリング結果をスワップチェーンに描画
	ColorBuffer* offScreenTex = m_ResourceManager->GetBufferManager()->GetColorBuffer(m_RenderTextures[RenderTextureType::OffScreen].m_BufferID);
	context->BarrierTransition(
		offScreenTex->GetResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	context->SetGraphicsPipelineState(m_PipelineManager->GetScreenCopyPSO().pso.Get());
	context->SetGraphicsRootSignature(m_PipelineManager->GetScreenCopyPSO().rootSignature.Get());
	context->SetGraphicsRootDescriptorTable(0, m_ResourceManager->GetSUVDHeap()->GetGpuHandle(offScreenTex->GetSUVHandleIndex()));
	context->DrawInstanced(3, 1, 0, 0);
	// SwapChainResourceの状態遷移
	context->BarrierTransition(
		m_SwapChain->GetBackBuffer(backBufferIndex),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	// コマンドリストのクローズ
	context->Close();
	// コマンドリストの実行
	commandManager->ExecuteCommandList(context->GetCommandList(), QueueType::Graphics);
	// SwapChainのPresent
	m_SwapChain->Present();
	// シグナル
	commandManager->Signal(QueueType::Graphics);
	// GPUの完了待ち
	commandManager->WaitForFence(QueueType::Graphics);
	// コマンドリストの返却
	commandManager->ReturnCommandContext(context);
}

void GraphicsEngine::PostRenderWithImGui(ImGuiManager* imgui)
{
	// コマンドマネージャー
	CommandManager* commandManager = m_GraphicsCore->GetCommandManager();
	// コマンドリストの取得
	CommandContext* context = commandManager->GetCommandContext();
	context->Reset();
	context->SetDescriptorHeap(m_ResourceManager->GetSUVDHeap()->GetDescriptorHeap());
	// SwapChainのBackBufferIndexを取得
	UINT backBufferIndex = m_SwapChain->GetBackBufferIndex();
	// SwapChainResourceの状態遷移
	context->BarrierTransition(
		m_SwapChain->GetBackBuffer(backBufferIndex),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	// RTVの設定
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_ResourceManager->GetRTVDHeap()->GetCpuHandle(backBufferIndex);
	context->SetRenderTarget(rtvHandle);
	context->ClearRenderTarget(rtvHandle);
	// ビューポートの設定
	D3D12_VIEWPORT viewport =
		D3D12_VIEWPORT(
			0.0f, 0.0f,
			static_cast<float>(WinApp::GetWindowWidth()),
			static_cast<float>(WinApp::GetWindowHeight()),
			0.0f, 1.0f
		);
	context->SetViewport(viewport);
	// シザーレクトの設定
	D3D12_RECT rect = D3D12_RECT(
		0, 0,
		WinApp::GetWindowWidth(),
		WinApp::GetWindowHeight()
	);
	context->SetScissorRect(rect);
	// プリミティブトポロジの設定
	context->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ImGuiの描画
	imgui->Draw(context->GetCommandList());
	// SwapChainResourceの状態遷移
	context->BarrierTransition(
		m_SwapChain->GetBackBuffer(backBufferIndex),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	// コマンドリストのクローズ
	context->Close();
	// コマンドリストの実行
	commandManager->ExecuteCommandList(context->GetCommandList(), QueueType::Graphics);
	// SwapChainのPresent
	m_SwapChain->Present();
	// シグナル
	commandManager->Signal(QueueType::Graphics);
	// GPUの完了待ち
	commandManager->WaitForFence(QueueType::Graphics);
	// コマンドリストの返却
	commandManager->ReturnCommandContext(context);
}

void GraphicsEngine::CreateSwapChain(IDXGIFactory7* dxgiFactory)
{	
	// SwapChainの生成
	m_SwapChain = std::make_unique<SwapChain>(
		dxgiFactory,
		m_GraphicsCore->GetCommandQueue(QueueType::Graphics),
		WinApp::GetHWND(),
		WinApp::GetWindowWidth(),
		WinApp::GetWindowHeight()
	);
	// SwapChainのリソースでRTVを作成
	m_ResourceManager->CreateSwapChain(m_SwapChain.get());
}

void GraphicsEngine::CreateDepthBuffer()
{
	// DepthBufferの生成
	// DepthBufferのリソースを作成
	BUFFER_DEPTH_DESC desc = {};
	desc.width = WinApp::GetWindowWidth();
	desc.height = WinApp::GetWindowHeight();
	desc.format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	// DepthBufferのリソースを作成
	m_DepthManager->SetDepthBufferIndex(m_ResourceManager->CreateDepthBuffer(desc));
}

void GraphicsEngine::CreateOffscreenBuffer()
{
	BUFFER_COLOR_DESC desc = {};
	desc.width = WinApp::GetWindowWidth();
	desc.height = WinApp::GetWindowHeight();
	desc.format = PixelFormat;
	desc.state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	m_RenderTextures[OffScreen].m_BufferID = m_ResourceManager->CreateColorBuffer(desc);
	m_RenderTextures[OffScreen].m_Width = desc.width;
	m_RenderTextures[OffScreen].m_Height = desc.height;
}
