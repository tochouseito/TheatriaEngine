#include "pch.h"
#include "PixelBuffer.h"

void PixelBuffer::Create(const uint32_t& width, const uint32_t& height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, const uint32_t& DHandleIndex)
{
	HRESULT hr = {};

	// 既存のリソースを破棄
	Destroy();

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = format;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;// 2dテクスチャとして書き込む

	// ハンドルの取得
	m_DHandleIndex = DHandleIndex;
}