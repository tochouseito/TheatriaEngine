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

}

void GraphicsEngine::Render()
{
	
}

void GraphicsEngine::PostRender()
{

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
