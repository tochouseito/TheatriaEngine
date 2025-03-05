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

uint32_t BufferManager::CreatePixelBuffer(const uint32_t& width, const uint32_t& height, DXGI_FORMAT format ,const D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, const uint32_t& dhIndex, ID3D12Resource* pResource)
{
	//HRESULT hr = {};
	width;
	height;
	// バッファの追加
	uint32_t index;
	if (pResource)
	{
		index = static_cast<uint32_t>(m_PixelBuffers.push_back(PixelBuffer(pResource, D3D12_RESOURCE_STATE_PRESENT)));
	} else {
		index = static_cast<uint32_t>(m_PixelBuffers.push_back(PixelBuffer()));
	}
	PixelBuffer& buffer = m_PixelBuffers[index];

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = format;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;// 2dテクスチャとして書き込む

	// ハンドルの取得
	buffer.SetDHandleIndex(dhIndex);

	// Viewの生成
	m_Device->CreateRenderTargetView(
		buffer.GetResource(),
		&desc,
		cpuHandle
	);

	return index;
}
