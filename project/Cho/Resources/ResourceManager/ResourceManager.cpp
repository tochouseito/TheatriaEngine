#include "pch.h"
#include "ResourceManager.h"
#include "OS/Windows/WinApp/WinApp.h"
#include "SDK/DirectX/DirectX12/SwapChain/SwapChain.h"

ResourceManager::ResourceManager(ID3D12Device8* device)
{
	CreateHeap(device);// デバイスを受け取り、プールを作成する

	m_BufferManager = std::make_unique<BufferManager>(device, this);// バッファマネージャの生成
	m_TextureManager = std::make_unique<TextureManager>();// テクスチャマネージャの生成
	m_ModelManager = std::make_unique<ModelManager>(m_BufferManager.get());// モデルマネージャの生成
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
		desc.dHIndex = m_RTVDescriptorHeap->Create();
		swapChain->SetIndex(bufferindex, m_BufferManager->CreateForSwapChain(desc, pResource.Get()));
	}
	{// Buffer1
		uint32_t bufferindex = 1;
		ComPtr<ID3D12Resource> pResource = swapChain->GetBackBuffer(bufferindex);
		BUFFER_COLOR_DESC desc = {};
		desc.width = WinApp::GetWindowWidth();
		desc.height = WinApp::GetWindowHeight();
		desc.format = PixelFormat;
		desc.dHIndex = m_RTVDescriptorHeap->Create();
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

ID3D12Resource* ResourceManager::CreateGpuResource(ID3D12Device8* device, const size_t& sizeInBytes)
{
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;// UploadHeapを使う
	// リソースの設定
	D3D12_RESOURCE_DESC desc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = sizeInBytes;// リソースのサイズ。今回はVector4を３頂点分
	// バッファの場合はこれらは1にする決まり
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	// バッファの場合ははこれにする決まり
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際にリソースを作る
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr;
	hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	ChoAssertLog("Failed to create gpu resource.", hr, __FILE__, __LINE__);
	return resource.Get();
}

uint32_t ResourceManager::CreateColorBuffer(BUFFER_COLOR_DESC& desc)
{
	desc.dHIndex = m_RTVDescriptorHeap->Create();
	return m_BufferManager->CreateBuffer<BUFFER_COLOR_DESC>(desc);
}

uint32_t ResourceManager::CreateDepthBuffer(BUFFER_DEPTH_DESC& desc)
{
	desc.dHIndex = m_DSVDescriptorHeap->Create();
	return m_BufferManager->CreateBuffer<BUFFER_DEPTH_DESC>(desc);
}

uint32_t ResourceManager::CreateVertexBuffer(BUFFER_VERTEX_DESC& desc)
{
	desc.dHIndex = m_SUVDescriptorHeap->Create();
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

}
