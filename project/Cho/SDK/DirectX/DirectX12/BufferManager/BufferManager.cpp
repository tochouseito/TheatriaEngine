#include "pch.h"
#include "BufferManager.h"
#include "Resources/ResourceManager/ResourceManager.h"

BufferManager::BufferManager(ID3D12Device8* device,ResourceManager* resourceManager)
{
	m_ResourceManager = resourceManager;
	m_Device = device;
}

BufferManager::~BufferManager()
{
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
