#pragma once
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
struct BUFFER_VERTEX_DESC
{
	UINT numElements;
	UINT structuredByteStride;
	D3D12_RESOURCE_STATES state;
	uint32_t dHIndex;
};
class VertexBuffer : public GpuBuffer
{
public:
	// Constructor
	VertexBuffer(BUFFER_VERTEX_DESC desc) : GpuBuffer()
	{
		SetDHandleIndex(desc.dHIndex);
		SetResourceState(desc.state);
	}
	// Constructor
	VertexBuffer(ID3D12Resource* pResource, BUFFER_VERTEX_DESC desc) :
		GpuBuffer(pResource, desc.state)
	{
	}
	// Destructor
	~VertexBuffer()
	{
	}
	void CreateVertexResource(ID3D12Device8* device, const UINT& numElements, const UINT& structuredByteStride);
private:

};

