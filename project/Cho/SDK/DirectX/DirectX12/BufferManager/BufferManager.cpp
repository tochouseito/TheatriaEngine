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

uint32_t BufferManager::CreateBufferForSwapChain(const BUFFER_COLOR_DESC& desc, ID3D12Resource* pResource)
{
	ColorBuffer buffer(pResource, desc);
	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = desc.format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;// 2dテクスチャとして書き込む
	// Viewの生成
	m_Device->CreateRenderTargetView(
		buffer.GetResource(),
		&rtvDesc,
		m_ResourceManager->GetRTVDHeap()->GetCpuHandle(buffer.GetDHandleIndex())
	);
	// コンテナに移動
	uint32_t index = static_cast<uint32_t>(m_ColorBuffers.push_back(std::move(buffer)));
	return index;
}

uint32_t BufferManager::CreateBufferProcess(const BUFFER_COLOR_DESC& desc, ID3D12Resource* pResource)
{
	if (pResource) {// リソースがある場合
		ColorBuffer buffer(pResource, desc);
		// RTVの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = desc.format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;// 2dテクスチャとして書き込む
		// Viewの生成
		m_Device->CreateRenderTargetView(
			buffer.GetResource(),
			&rtvDesc,
			m_ResourceManager->GetRTVDHeap()->GetCpuHandle(buffer.GetDHandleIndex())
		);
		// コンテナに移動
		uint32_t index = static_cast<uint32_t>(m_ColorBuffers.push_back(std::move(buffer)));
		return index;
	} else {// リソースがない場合
		ColorBuffer buffer(desc);
		// RTVの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = desc.format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;// 2dテクスチャとして書き込む
		
		//// Viewの生成
		//m_Device->CreateRenderTargetView(
		//	buffer.GetResource
		//)
		// コンテナに移動
		uint32_t index = static_cast<uint32_t>(m_ColorBuffers.push_back(std::move(buffer)));
		return index;
	}
}

uint32_t BufferManager::CreateBufferProcess(const BUFFER_DEPTH_DESC& desc, ID3D12Resource* pResource)
{
	if (pResource) {
		return 0;
	} else {
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
		// 利用するHeapの設定
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// VRAM上に作る
		// 深度値のクリア設定
		D3D12_CLEAR_VALUE clearValue{};
		clearValue.DepthStencil.Depth = 1.0f;// 1.0f（最大値）でクリア
		clearValue.Format = desc.format;// フォーマット。Resourceと合わせる
		// Resourceの生成
		buffer.CreateTextureResource(
			m_Device,
			resourceDesc,
			clearValue,
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
			m_ResourceManager->GetDSVDHeap()->GetCpuHandle(buffer.GetDHandleIndex())
		);
		// コンテナに移動
		uint32_t index = static_cast<uint32_t>(m_DepthBuffers.push_back(std::move(buffer)));
		return index;
	}
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
