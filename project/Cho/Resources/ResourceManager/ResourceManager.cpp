#include "pch.h"
#include "ResourceManager.h"
#include "OS/Windows/WinApp/WinApp.h"
#include "SDK/DirectX/DirectX12/SwapChain/SwapChain.h"

ResourceManager::ResourceManager(ID3D12Device8* device)
{
	CreateHeap(device);// デバイスを受け取り、プールを作成する

	m_BufferManager = std::make_unique<BufferManager>(device, this);// バッファマネージャの生成
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

void ResourceManager::GenerateManager(GraphicsEngine* graphicsEngine, IntegrationBuffer* intBuf)
{
	graphicsEngine;
	intBuf;

	//m_ModelManager = std::make_unique<ModelManager>(this, intBuf);
	//m_TextureManager = std::make_unique<TextureManager>(this, graphicsEngine,m_Device);
}

ComPtr<ID3D12Resource> ResourceManager::CreateGPUResource(const D3D12_HEAP_PROPERTIES& heapProp, const D3D12_RESOURCE_DESC& desc, const D3D12_RESOURCE_STATES& state, const D3D12_CLEAR_VALUE* clearValue)
{
	ComPtr<ID3D12Resource> resource;
	m_Device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		state,
		clearValue,
		IID_PPV_ARGS(&resource)
	);
	return resource;
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

D3D12_CPU_DESCRIPTOR_HANDLE ResourceManager::GetCPUHandle(const uint32_t& index, D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	switch (type)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
		return m_RTVDescriptorHeap->GetCpuHandle(m_BufferManager->GetColorBuffer(index)->GetRTVHandleIndex());
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
		return m_DSVDescriptorHeap->GetCpuHandle(m_BufferManager->GetDepthBuffer(index)->GetDSVHandleIndex());
		break;
	default:
		break;
	}
	return D3D12_CPU_DESCRIPTOR_HANDLE();
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

