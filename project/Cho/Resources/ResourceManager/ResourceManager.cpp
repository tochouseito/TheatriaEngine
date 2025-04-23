#include "pch.h"
#include "ResourceManager.h"
#include "OS/Windows/WinApp/WinApp.h"
#include "SDK/DirectX/DirectX12/SwapChain/SwapChain.h"

ResourceManager::ResourceManager(ID3D12Device8* device)
{
	// ポインタをセット
	m_Device = device;
	// ヒープの作成
	CreateHeap(device);
	// 統合データの作成
	CreateIntegrationBuffers();
	// ダミーマテリアルの作成
	CreateDummyMaterial();
	// スクリプトコンテナの作成
	m_ScriptContainer = std::make_unique<ScriptContainer>();
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

void ResourceManager::GenerateManager(GraphicsEngine* graphicsEngine)
{
	m_TextureManager = std::make_unique<TextureManager>(this, graphicsEngine, m_Device);
	m_ModelManager = std::make_unique<ModelManager>(this);
	m_SoundManager = std::make_unique<SoundManager>(this);
}

uint32_t ResourceManager::CreateColorBuffer(D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE* clearValue, D3D12_RESOURCE_STATES state)
{
	std::unique_ptr<ColorBuffer> buffer = std::make_unique<ColorBuffer>();
	buffer->CreatePixelBufferResource(m_Device, desc, clearValue, state);
	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// Viewの生成
	buffer->CreateSRV(m_Device, srvDesc, m_SUVDescriptorHeap.get());
	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = desc.Format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	// Viewの生成
	buffer->CreateRTV(m_Device, rtvDesc, m_RTVDescriptorHeap.get());
	// コンテナに移動
	uint32_t index = static_cast<uint32_t>(m_ColorBuffers.push_back(std::move(buffer)));
	return index;
}

uint32_t ResourceManager::CreateDepthBuffer(D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES state)
{
	std::unique_ptr<DepthBuffer> buffer = std::make_unique<DepthBuffer>();
	buffer->CreateDepthBufferResource(m_Device, desc, state);
	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = desc.Format;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	// Viewの生成
	buffer->CreateDSV(m_Device, dsvDesc, m_DSVDescriptorHeap.get());
	// コンテナに移動
	uint32_t index = static_cast<uint32_t>(m_DepthBuffers.push_back(std::move(buffer)));
	return index;
}

uint32_t ResourceManager::CreateTextureBuffer(D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE* clearValue, D3D12_RESOURCE_STATES state)
{
	std::unique_ptr<PixelBuffer> buffer = std::make_unique<PixelBuffer>();
	buffer->CreatePixelBufferResource(m_Device, desc, clearValue, state);
	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = static_cast<UINT>(desc.MipLevels);
	// Viewの生成
	buffer->CreateSRV(m_Device, srvDesc, m_SUVDescriptorHeap.get());
	// コンテナに移動
	uint32_t index = static_cast<uint32_t>(m_TextureBuffers.push_back(std::move(buffer)));
	return index;
}

bool ResourceManager::RemakeColorBuffer(std::optional<uint32_t>& index, D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE* clearValue, D3D12_RESOURCE_STATES state)
{
	ColorBuffer* buffer = GetBuffer<ColorBuffer>(index);
	buffer->RemakePixelBufferResource(m_Device, desc, clearValue, state);
	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// Viewの生成
	buffer->CreateSRV(m_Device, srvDesc, m_SUVDescriptorHeap.get());
	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = desc.Format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	// Viewの生成
	buffer->CreateRTV(m_Device, rtvDesc, m_RTVDescriptorHeap.get());
	return true;
}

void ResourceManager::CreateIntegrationBuffers()
{
	// Transform統合バッファ
	std::optional<uint32_t> transformIndex = CreateStructuredBuffer<BUFFER_DATA_TF>(kIntegrationTFBufferSize);
	m_IntegrationData[IntegrationDataType::Transform] = std::make_unique<IntegrationData<BUFFER_DATA_TF>>(transformIndex, kIntegrationTFBufferSize);
	// Line統合バッファ
	std::optional<uint32_t> lineIndex = CreateVertexBuffer<BUFFER_DATA_LINE>(kIntegrationLineBufferSize);
	m_IntegrationData[IntegrationDataType::Line] = std::make_unique<IntegrationData<BUFFER_DATA_LINE>>(lineIndex, kIntegrationLineBufferSize);
	// Material統合バッファ
	std::optional<uint32_t> materialIndex = CreateStructuredBuffer<BUFFER_DATA_MATERIAL>(kIntegrationMaterialBufferSize);
	m_IntegrationData[IntegrationDataType::Material] = std::make_unique<IntegrationData<BUFFER_DATA_MATERIAL>>(materialIndex, kIntegrationMaterialBufferSize);
}

void ResourceManager::CreateHeap(ID3D12Device8* device)
{
	m_SUVDescriptorHeap = std::make_unique<DescriptorHeap>(device, kMaxSUVDescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);
	m_RTVDescriptorHeap = std::make_unique<DescriptorHeap>(device, kMaxRTVDescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false);
	m_DSVDescriptorHeap = std::make_unique<DescriptorHeap>(device, kMaxDSVDescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false);
}

void ResourceManager::CreateDummyMaterial()
{
	uint32_t mapID = m_IntegrationData[IntegrationDataType::Material]->GetMapID();
	if (mapID == 0) { Log::Write(LogLevel::Assert, "DummyMaterial Create"); }
	StructuredBuffer<BUFFER_DATA_MATERIAL>* pIntegrationBuffer = dynamic_cast<StructuredBuffer<BUFFER_DATA_MATERIAL>*>(GetIntegrationBuffer(IntegrationDataType::Material));
	// ダミーマテリアルの初期化
	BUFFER_DATA_MATERIAL data = {};
	Color color;
	data.color = color.From255(200, 200, 200);
	data.enableLighting = true;
	data.enableTexture = false;
	data.matUV = Matrix4::Identity();
	data.shininess = 0.0f;
	data.textureId = 0;
	pIntegrationBuffer->UpdateData(data, mapID);
}
