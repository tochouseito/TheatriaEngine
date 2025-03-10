#pragma once
#include "SDK/DirectX/DirectX12/PixelBuffer/PixelBuffer.h"
struct BUFFER_DEPTH_DESC
{
	uint32_t width;
	uint32_t height;
	DXGI_FORMAT format;
	D3D12_RESOURCE_STATES state;
	uint32_t dHIndex;
};
class DepthBuffer : public PixelBuffer
{
public:
	// Constructor
	DepthBuffer(const BUFFER_DEPTH_DESC& desc):
		PixelBuffer()
	{
		SetWidth(desc.width);
		SetHeight(desc.height);
		SetDHandleIndex(desc.dHIndex);
	}
	// Constructor
	DepthBuffer(ID3D12Resource* pResource,const BUFFER_DEPTH_DESC& desc) :
		PixelBuffer(pResource, desc.state) 
	{
	}
private:

};

