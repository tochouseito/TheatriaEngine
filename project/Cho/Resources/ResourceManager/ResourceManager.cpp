#include "pch.h"
#include "ResourceManager.h"
#include "OS/Windows/WinApp/WinApp.h"
#include "SDK/DirectX/DirectX12/SwapChain/SwapChain.h"

ResourceManager::ResourceManager(ID3D12Device8* device)
{
	// デバイスの設定
	m_Device = device;
	// ヒープの作成
	CreateHeap(device);
	// 初期化
	Initialize();
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

void ResourceManager::CreateHeap(ID3D12Device8* device)
{
	m_SUVDescriptorHeap = std::make_unique<DescriptorHeap>(device, kMaxSUVDescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);
	m_RTVDescriptorHeap = std::make_unique<DescriptorHeap>(device, kMaxRTVDescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false);
	m_DSVDescriptorHeap = std::make_unique<DescriptorHeap>(device, kMaxDSVDescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false);
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

uint32_t ResourceManager::CreateConstantBuffer(BUFFER_CONSTANT_DESC& desc)
{
	return m_BufferManager->CreateBuffer<BUFFER_CONSTANT_DESC>(desc);
}

uint32_t ResourceManager::CreateStructuredBuffer(BUFFER_STRUCTURED_DESC& desc)
{
	desc.suvDHIndex = m_SUVDescriptorHeap->Create();
	return m_BufferManager->CreateBuffer<BUFFER_STRUCTURED_DESC>(desc);
}

uint32_t ResourceManager::CreateTextureBuffer(BUFFER_TEXTURE_DESC& desc)
{
	desc.suvDHIndex = m_SUVDescriptorHeap->Create();
	return m_BufferManager->CreateBuffer<BUFFER_TEXTURE_DESC>(desc);
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

