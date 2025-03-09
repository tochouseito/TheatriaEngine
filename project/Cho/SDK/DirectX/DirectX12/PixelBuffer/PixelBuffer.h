#pragma once

#include "SDK/DirectX/DirectX12/GpuResource/GpuResource.h"

class PixelBuffer : public GpuResource
{
public:
	// Constructor
	PixelBuffer() : 
		m_Width(0), 
		m_Height(0), 
		m_Format(DXGI_FORMAT_UNKNOWN) 
	{
	}
	// Constructor
	PixelBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) : 
		m_Width(0),
		m_Height(0),
		m_Format(DXGI_FORMAT_UNKNOWN),
		GpuResource(pResource,CurrentState)
	{
	}
	// Destructor
	~PixelBuffer() = default;

	// Getters
	// ピクセルバッファの幅を取得
	uint32_t GetWidth() const { return m_Width; }
	// ピクセルバッファの高さを取得
	uint32_t GetHeight() const { return m_Height; }
	// ピクセルバッファのフォーマットを取得
	const DXGI_FORMAT& GetFormat() const { return m_Format; }

	// Setters
	// ピクセルバッファの幅を設定
	void SetWidth(const uint32_t& width) { m_Width = width; }
	// ピクセルバッファの高さを設定
	void SetHeight(const uint32_t& height) { m_Height = height; }
	// ピクセルバッファのフォーマットを設定
	void SetFormat(const DXGI_FORMAT& format) { m_Format = format; }
protected:
	// ピクセルバッファの幅
	uint32_t m_Width;
	// ピクセルバッファの高さ
	uint32_t m_Height;
	// ピクセルバッファのフォーマット
	DXGI_FORMAT m_Format;
};

