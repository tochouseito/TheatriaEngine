#include "pch.h"
#include "GraphicsEngine.h"
#include "OS/Windows/WinApp/WinApp.h"
#include "Resources/ResourceManager/ResourceManager.h"

void GraphicsEngine::Initialize()
{
}

void GraphicsEngine::Finalize()
{
}

void GraphicsEngine::Operation()
{
}

void GraphicsEngine::Update(ResourceManager& resourceManager)
{
	resourceManager;
}

void GraphicsEngine::CreateSwapChain(IDXGIFactory7* dxgiFactory, ResourceManager* resourceManager)
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
	resourceManager->CreateSwapChain(m_SwapChain.get());
}
