#include "pch.h"
#include "ResourceManager.h"
#include "OS/Windows/WinApp/WinApp.h"
#include "SDK/DirectX/DirectX12/SwapChain/SwapChain.h"

ResourceManager::ResourceManager(ID3D12Device8* device)
{
	CreateHeap(device);// デバイスを受け取り、プールを作成する

	m_BufferManager = std::make_unique<BufferManager>(device, this);// バッファマネージャの生成
	m_TextureManager = std::make_unique<TextureManager>();// テクスチャマネージャの生成
	m_ModelManager = std::make_unique<ModelManager>(this);// モデルマネージャの生成
	m_Device = device;// デバイスの設定
	
	Initialize();// 初期化
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::Initialize()
{
	
}

void ResourceManager::Finalize()
{
}

void ResourceManager::Update()
{
}

void ResourceManager::Release()
{
}

void ResourceManager::CreateSwapChain(SwapChain* swapChain)
{
	// SwapChainのリソースでRTVを作成
	{// Buffer0
		uint32_t bufferindex = 0;
		ComPtr<ID3D12Resource> pResource = swapChain->GetBackBuffer(bufferindex);
		BUFFER_COLOR_DESC desc = {};
		desc.width = WinApp::GetWindowWidth();
		desc.height = WinApp::GetWindowHeight();
		desc.format = PixelFormat;
		desc.rtvDHIndex = m_RTVDescriptorHeap->Create();
		swapChain->SetIndex(bufferindex, m_BufferManager->CreateForSwapChain(desc, pResource.Get()));
	}
	{// Buffer1
		uint32_t bufferindex = 1;
		ComPtr<ID3D12Resource> pResource = swapChain->GetBackBuffer(bufferindex);
		BUFFER_COLOR_DESC desc = {};
		desc.width = WinApp::GetWindowWidth();
		desc.height = WinApp::GetWindowHeight();
		desc.format = PixelFormat;
		desc.rtvDHIndex = m_RTVDescriptorHeap->Create();
		swapChain->SetIndex(bufferindex, m_BufferManager->CreateForSwapChain(desc, pResource.Get()));
	}
}

void ResourceManager::CreateSUVDescriptorHeap(ID3D12Device8* device)
{
	m_SUVDescriptorHeap = std::make_unique<SUVDescriptorHeap>(device, 1024);
}

void ResourceManager::CreateRTVDescriptorHeap(ID3D12Device8* device)
{
	m_RTVDescriptorHeap = std::make_unique<RTVDescriptorHeap>(device, 16);
}

void ResourceManager::CreateDSVDescriptorHeap(ID3D12Device8* device)
{
	m_DSVDescriptorHeap = std::make_unique<DSVDescriptorHeap>(device, 1);
}

void ResourceManager::CreateHeap(ID3D12Device8* device)
{
	CreateSUVDescriptorHeap(device);
	CreateRTVDescriptorHeap(device);
	CreateDSVDescriptorHeap(device);
}

uint32_t ResourceManager::CreateColorBuffer(BUFFER_COLOR_DESC& desc)
{
	desc.rtvDHIndex = m_RTVDescriptorHeap->Create();
	desc.srvDHIndex = m_SUVDescriptorHeap->Create();
	return m_BufferManager->CreateBuffer<BUFFER_COLOR_DESC>(desc);
}

uint32_t ResourceManager::CreateDepthBuffer(BUFFER_DEPTH_DESC& desc)
{
	desc.dsvDHIndex = m_DSVDescriptorHeap->Create();
	return m_BufferManager->CreateBuffer<BUFFER_DEPTH_DESC>(desc);
}

uint32_t ResourceManager::CreateVertexBuffer(BUFFER_VERTEX_DESC& desc)
{
	desc.suvDHIndex = m_SUVDescriptorHeap->Create();
	desc.suvDHIndexForIBV = m_SUVDescriptorHeap->Create();
	return m_BufferManager->CreateBuffer<BUFFER_VERTEX_DESC>(desc);
}

void ResourceManager::RemakeColorBuffer(const uint32_t& index, BUFFER_COLOR_DESC& desc)
{
	m_BufferManager->RemakeBuffer(index, desc);
}

void ResourceManager::RemakeDepthBuffer(const uint32_t& index, BUFFER_DEPTH_DESC& desc)
{
	m_BufferManager->RemakeBuffer(index, desc);
}

void ResourceManager::RemakeVertexBuffer(const uint32_t& index, BUFFER_VERTEX_DESC& desc)
{
	m_BufferManager->RemakeBuffer(index, desc);
}

void ResourceManager::ReleaseColorBuffer(const uint32_t& index)
{
	index;
}

void ResourceManager::ReleaseDepthBuffer(const uint32_t& index)
{
	index;
}

void ResourceManager::ReleaseVertexBuffer(const uint32_t& index)
{
	index;
}

