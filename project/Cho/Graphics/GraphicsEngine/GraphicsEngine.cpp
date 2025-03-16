#include "pch.h"
#include "GraphicsEngine.h"
#include "OS/Windows/WinApp/WinApp.h"
#include "Resources/ResourceManager/ResourceManager.h"

void GraphicsEngine::Init(IDXGIFactory7* dxgiFactory)
{
	// SwapChainの生成
	CreateSwapChain(dxgiFactory);
	// DepthBufferの生成
	CreateDepthBuffer();
	// オフスクリーンレンダリング用のリソースを作成
	
}

void GraphicsEngine::PreRender()
{
	/*CommandContext* context = m_GraphicsCore->GetCommandManager()->GetCommandContext();
	context->Reset();
	context->SetDescriptorHeap(m_ResourceManager->GetSUVDHeap()->GetDescriptorHeap());*/
}

void GraphicsEngine::Render()
{
	
}

void GraphicsEngine::PostRender()
{
	CommandContext* context = m_GraphicsCore->GetCommandManager()->GetCommandContext();
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
	// SwapChainResourceの状態遷移
	context->BarrierTransition(
		m_SwapChain->GetBackBuffer(backBufferIndex),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	// コマンドリストのクローズ
	context->Close();
	// コマンドリストの実行
	m_GraphicsCore->GetCommandManager()->ExecuteCommandList(context->GetCommandList(), QueueType::Graphics);
	// SwapChainのPresent
	m_SwapChain->Present();
	// シグナル
	m_GraphicsCore->GetCommandManager()->Signal(QueueType::Graphics);
	// GPUの完了待ち
	m_GraphicsCore->GetCommandManager()->WaitForFence(QueueType::Graphics);
	// コマンドリストの返却
	m_GraphicsCore->GetCommandManager()->ReturnCommandContext(context);
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
	m_DepthManager = std::make_unique<DepthManager>();
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
	m_OffscreenBufferIndex = m_ResourceManager->CreateColorBuffer(desc);
}
