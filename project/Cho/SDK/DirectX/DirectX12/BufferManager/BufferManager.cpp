#include "pch.h"
#include "BufferManager.h"
#include "OS/Windows/WinApp/WinApp.h"
#include "Resources/ResourceManager/ResourceManager.h"

BufferManager::BufferManager(ID3D12Device8* device,ResourceManager* resourceManager)
{
	m_ResourceManager = resourceManager;
	m_Device = device;
}

BufferManager::~BufferManager()
{
}

void BufferManager::ReleaseColorBuffer(const uint32_t& index)
{
	m_ResourceManager->GetRTVDHeap()->RemoveHandle(m_ColorBuffers[index].GetRTVHandleIndex());
	m_ResourceManager->GetSUVDHeap()->RemoveHandle(m_ColorBuffers[index].GetSUVHandleIndex());
	m_ColorBuffers[index].Destroy();
	m_ColorBuffers.erase(index);
}

void BufferManager::ReleaseDepthBuffer(const uint32_t& index)
{
	m_ResourceManager->GetDSVDHeap()->RemoveHandle(m_DepthBuffers[index].GetDSVHandleIndex());
	m_DepthBuffers[index].Destroy();
	m_DepthBuffers.erase(index);
}

void BufferManager::ReleaseVertexBuffer(const uint32_t& index)
{
	m_ResourceManager->GetSUVDHeap()->RemoveHandle(m_VertexBuffers[index].GetIndexBufferDHandleIndex());
	m_ResourceManager->GetSUVDHeap()->RemoveHandle(m_VertexBuffers[index].GetSUVHandleIndex());
	m_VertexBuffers[index].DestroyIndexBuffer();
	m_VertexBuffers[index].Destroy();
	m_VertexBuffers.erase(index);
}


uint32_t BufferManager::CreateBufferProcess(BUFFER_COLOR_DESC& desc)
{
	if (!desc.width || !desc.height)
	{
		ChoAssertLog("Invalid buffer size", false, __FILE__, __LINE__);
	}
	if (desc.format == DXGI_FORMAT_UNKNOWN)
	{
		ChoAssertLog("Invalid buffer format", false, __FILE__, __LINE__);
	}
	if (desc.state == D3D12_RESOURCE_STATE_COMMON)
	{
		ChoAssertLog("Invalid buffer state", false, __FILE__, __LINE__);
	}
	if(desc.rtvDHIndex == UINT32_MAX)
	{
		ChoAssertLog("Invalid buffer dHIndex", false, __FILE__, __LINE__);
	}
	ColorBuffer buffer(desc);
	// Resource生成
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = desc.width;// Textureの幅
	resourceDesc.Height = desc.height;// Textureの高さ
	resourceDesc.MipLevels = 1;//mipmapの数
	resourceDesc.DepthOrArraySize = 1;// 奥行き or 配列Textureの配列数
	resourceDesc.Format = desc.format;// DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;// サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;// 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;// RenderTargetとして利用可能にする
	// 深度値のクリア設定
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = desc.format;
	clearValue.Color[0] = kClearColor[0];
	clearValue.Color[1] = kClearColor[1];
	clearValue.Color[2] = kClearColor[2];
	clearValue.Color[3] = kClearColor[3];
	buffer.CreateTextureResource(
		m_Device,
		resourceDesc,
		&clearValue,
		desc.state
	);

	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	// metadataを基にSRVの設定
	srvDesc.Format = desc.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// Viewの生成
	m_Device->CreateShaderResourceView(
		buffer.GetResource(),
		&srvDesc,
		m_ResourceManager->GetSUVDHeap()->GetCpuHandle(buffer.GetSUVHandleIndex())
	);

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = desc.format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;// 2dテクスチャとして書き込む
	// Viewの生成
	m_Device->CreateRenderTargetView(
		buffer.GetResource(),
		&rtvDesc,
		m_ResourceManager->GetRTVDHeap()->GetCpuHandle(buffer.GetRTVHandleIndex())
	);

	// コンテナに移動
	uint32_t index = static_cast<uint32_t>(m_ColorBuffers.push_back(std::move(buffer)));
	return index;
}

uint32_t BufferManager::CreateBufferProcess(BUFFER_DEPTH_DESC& desc)
{
	DepthBuffer buffer(desc);
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = desc.width;// Textureの幅
	resourceDesc.Height = desc.height;// Textureの高さ
	resourceDesc.MipLevels = 1;//mipmapの数
	resourceDesc.DepthOrArraySize = 1;// 奥行き or 配列Textureの配列数
	resourceDesc.Format = desc.format;// DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;// サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;// 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;// DepthStencilとして使う通知
	// 深度値のクリア設定
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.DepthStencil.Depth = 1.0f;// 1.0f（最大値）でクリア
	clearValue.Format = desc.format;// フォーマット。Resourceと合わせる
	// Resourceの生成
	buffer.CreateTextureResource(
		m_Device,
		resourceDesc,
		&clearValue,
		desc.state
	);
	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = desc.format;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	// Viewの生成
	m_Device->CreateDepthStencilView(
		buffer.GetResource(),
		&dsvDesc,
		m_ResourceManager->GetDSVDHeap()->GetCpuHandle(buffer.GetDSVHandleIndex())
	);
	// コンテナに移動
	uint32_t index = static_cast<uint32_t>(m_DepthBuffers.push_back(std::move(buffer)));
	return index;
}

uint32_t BufferManager::CreateBufferProcess(BUFFER_VERTEX_DESC& desc)
{
	VertexBuffer buffer(desc);
	// Resourceの生成
	{// VertexBuffer
		buffer.CreateVertexResource(m_Device, desc.numElements, desc.structuredByteStride);
		// SRVの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.NumElements = desc.numElements;
		srvDesc.Buffer.StructureByteStride = desc.structuredByteStride;
		// Viewの作成
		m_Device->CreateShaderResourceView(
			buffer.GetResource(),
			&srvDesc,
			m_ResourceManager->GetSUVDHeap()->GetCpuHandle(desc.suvDHIndex)
		);
		// Mapping
		buffer.GetResource()->Map(0, nullptr, &desc.mappedVertices);
	}
	{// IndexBuffer
		buffer.CreateIndexResource(m_Device, desc.numElementsForIBV, desc.structuredByteStrideForIBV);
		// SRVの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.NumElements = desc.numElementsForIBV;
		srvDesc.Buffer.StructureByteStride = desc.structuredByteStrideForIBV;
		// Viewの作成
		m_Device->CreateShaderResourceView(
			buffer.GetIndexResource(),
			&srvDesc,
			m_ResourceManager->GetSUVDHeap()->GetCpuHandle(desc.suvDHIndexForIBV)
		);
		// Mapping
		buffer.GetIndexResource()->Map(0, nullptr, &desc.mappedIndices);
	}
	// コンテナに移動
	uint32_t index = static_cast<uint32_t>(m_VertexBuffers.push_back(std::move(buffer)));
	return index;
}

uint32_t BufferManager::CreateBufferProcess(BUFFER_CONSTANT_DESC& desc)
{
	desc;
	return 0;
}

uint32_t BufferManager::CreateBufferProcess(BUFFER_STRUCTURED_DESC& desc)
{
	desc;
	return 0;
}

uint32_t BufferManager::CreateBufferProcess(BUFFER_TEXTURE_DESC& desc)
{
	TextureBuffer buffer(desc);
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = static_cast<UINT64>(desc.width);// Textureの幅
	resourceDesc.Height = desc.height;// Textureの高さ
	resourceDesc.MipLevels = desc.mipLevels;//mipmapの数
	resourceDesc.DepthOrArraySize = desc.arraySize;// 奥行き or 配列Textureの配列数
	resourceDesc.Format = desc.format;// Textureのフォーマット
	resourceDesc.SampleDesc.Count = 1;// サンプリングカウント。1固定。
	resourceDesc.Dimension = desc.dimension;// Textureの次元
	buffer.CreateTextureResource(m_Device, resourceDesc, nullptr, desc.state);
	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = desc.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;// 2Dテクスチャとして利用
	srvDesc.Texture2D.MipLevels = static_cast<UINT>(desc.mipLevels);// mipmapの数
	m_Device->CreateShaderResourceView(
		buffer.GetResource(),
		&srvDesc,
		m_ResourceManager->GetSUVDHeap()->GetCpuHandle(desc.suvDHIndex)
	);
	// コンテナに移動
	uint32_t index = static_cast<uint32_t>(m_TextureBuffers.push_back(std::move(buffer)));
	return index;
}

void BufferManager::RemakeBufferProcess(const uint32_t& index, const BUFFER_COLOR_DESC& desc)
{
	index;
	desc;
}

void BufferManager::RemakeBufferProcess(const uint32_t& index, const BUFFER_DEPTH_DESC& desc)
{
	index;
	desc;
}

void BufferManager::RemakeBufferProcess(const uint32_t& index, const BUFFER_VERTEX_DESC& desc)
{
	index;
	desc;
}
