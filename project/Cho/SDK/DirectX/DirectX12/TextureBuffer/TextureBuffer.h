#pragma once
#include "Cho/SDK/DirectX/DirectX12/PixelBuffer/PixelBuffer.h"
struct BUFFER_TEXTURE_DESC
{
	UINT width;
	UINT height;
	DXGI_FORMAT format;
	D3D12_RESOURCE_STATES state;
	uint32_t suvDHIndex;
};
class TextureBuffer : public PixelBuffer
{
public:
	// Constructor
	TextureBuffer(BUFFER_TEXTURE_DESC& desc) :
		PixelBuffer()
	{
		SetWidth(desc.width);
		SetHeight(desc.height);
		SetFormat(desc.format);
		SetResourceState(desc.state);
		SetSUVHandleIndex(desc.suvDHIndex);
	}
	// Constructor
	TextureBuffer(ID3D12Resource* pResource, BUFFER_TEXTURE_DESC& desc) :
		PixelBuffer(pResource, desc.state)
	{
		SetWidth(desc.width);
		SetHeight(desc.height);
		SetFormat(desc.format);
		SetSUVHandleIndex(desc.suvDHIndex);
	}
	// Destructor
	~TextureBuffer() = default;
private:

};

