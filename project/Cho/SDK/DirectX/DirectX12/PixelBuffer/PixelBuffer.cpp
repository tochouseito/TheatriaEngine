#include "pch.h"
#include "PixelBuffer.h"
#include "Resources/ResourceManager/ResourceManager.h"

void PixelBuffer::Create(ID3D12Device8* device, ResourceManager* resourceManager, const uint32_t& width, const uint32_t& height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags)
{
	HRESULT hr = {};

	// 既存のリソースを破棄
	Destroy();

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = format;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;// 2dテクスチャとして書き込む

	// ハンドルの取得
	m_DHandleIndex = resourceManager->GetRTVDHeap()->Create();

	device->CreateRenderTargetView(
		GetResource(),
		&desc,
		resourceManager->GetRTVDHeap()->
	)
}