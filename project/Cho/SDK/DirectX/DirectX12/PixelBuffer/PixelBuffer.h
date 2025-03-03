#pragma once

#include "SDK/DirectX/DirectX12/GpuResource/GpuResource.h"

class PixelBuffer : public GpuResource
{
public:// メンバ関数
	// コンストラクタ
	PixelBuffer() : m_Width(0), m_Height(0), m_Format(DXGI_FORMAT_UNKNOWN) {}
	// デストラクタ
	~PixelBuffer() = default;

	// ピクセルバッファの幅を取得
	uint32_t GetWidth() const { return m_Width; }
	// ピクセルバッファの高さを取得
	uint32_t GetHeight() const { return m_Height; }
	// ピクセルバッファのフォーマットを取得
	const DXGI_FORMAT& GetFormat() const { return m_Format; }
	
	// ピクセルバッファの生成
	void Create(const uint32_t& width, const uint32_t& height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,const uint32_t& DHandleIndex);
private:
	// ピクセルバッファの幅
	uint32_t m_Width;
	// ピクセルバッファの高さ
	uint32_t m_Height;
	// ピクセルバッファのフォーマット
	DXGI_FORMAT m_Format;
};

