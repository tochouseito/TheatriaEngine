#pragma once
#include "Cho/SDK/DirectX/DirectX12/PixelBuffer/PixelBuffer.h"
struct BUFFER_COLOR_DESC
{
	uint32_t width;
	uint32_t height;
	DXGI_FORMAT format;
	D3D12_RESOURCE_STATES state;
	uint32_t dHIndex;
};
class ColorBuffer : public PixelBuffer
{
public:
	// Constructor
	ColorBuffer(BUFFER_COLOR_DESC desc) : PixelBuffer() 
	{
		SetWidth(desc.width);
		SetHeight(desc.height);
		SetFormat(desc.format);
		SetDHandleIndex(desc.dHIndex);
	}
	// Constructor
	ColorBuffer(ID3D12Resource* pResource, BUFFER_COLOR_DESC desc) :
		PixelBuffer(pResource, desc.state) 
	{
	}
	// Destructor
	~ColorBuffer() = default;
};

