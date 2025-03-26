#pragma once

#include "Cho/SDK/DirectX/DirectX12/GpuResource/GpuResource.h"

class GpuBuffer : public GpuResource
{
public:
	// Constructor
	GpuBuffer() {}

	// Constructor
	GpuBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
		GpuResource(pResource, CurrentState)
	{
	}

	// Destructor
	virtual ~GpuBuffer() { Destroy(); }

	// Create Buffer
	void Create(
		ID3D12Device* device,
		D3D12_HEAP_PROPERTIES& heapProperties,
		D3D12_HEAP_FLAGS heapFlags,
		D3D12_RESOURCE_DESC& desc,
		D3D12_RESOURCE_STATES InitialState,
		D3D12_CLEAR_VALUE* pClearValue = nullptr
	);
	void Map(void** pData) { GpuResource::Map(pData); }
	void UnMap() { GpuResource::UnMap(); }

	// Getters
    size_t GetBufferSize() const { return m_BufferSize; }
    uint32_t GetElementCount() const { return m_ElementCount; }
    uint32_t GetElementSize() const { return m_ElementSize; }

	// Setter
	void SetElementCount(const uint32_t& elementCount) { m_ElementCount = elementCount; }
	void SetElementSize(const uint32_t& elementSize) { m_ElementSize = elementSize; }
protected:
    size_t m_BufferSize = {};
    uint32_t m_ElementCount = {};
    uint32_t m_ElementSize = {};
    D3D12_RESOURCE_FLAGS m_ResourceFlags = {};
};

struct BUFFER_CONSTANT_DESC
{
	UINT numElements;
	UINT structuredByteStride;
	D3D12_RESOURCE_STATES state;
};
class ConstantBuffer : public GpuBuffer
{
public:
	// Constructor
	ConstantBuffer(BUFFER_CONSTANT_DESC desc) : GpuBuffer()
	{
		SetResourceState(desc.state);
	}
	// Constructor
	ConstantBuffer(ID3D12Resource* pResource, BUFFER_CONSTANT_DESC desc) :
		GpuBuffer(pResource, desc.state)
	{
	}
	// Destructor
	~ConstantBuffer()
	{
	}
private:
};

struct BUFFER_STRUCTURED_DESC
{
	UINT numElements;
	UINT structuredByteStride;
	D3D12_RESOURCE_STATES state;
	uint32_t suvDHIndex;
};
class StructuredBuffer : public GpuBuffer
{
public:
	// Constructor
	StructuredBuffer(BUFFER_STRUCTURED_DESC desc) : GpuBuffer()
	{
		SetSUVHandleIndex(desc.suvDHIndex);
		SetResourceState(desc.state);
	}
	// Constructor
	StructuredBuffer(ID3D12Resource* pResource, BUFFER_STRUCTURED_DESC desc) :
		GpuBuffer(pResource, desc.state)
	{
	}
	// Destructor
	~StructuredBuffer()
	{
	}
private:
};


