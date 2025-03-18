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
    void Create();

	// Getters
    size_t GetBufferSize() const { return m_BufferSize; }
    uint32_t GetElementCount() const { return m_ElementCount; }
    uint32_t GetElementSize() const { return m_ElementSize; }
protected:
    size_t m_BufferSize = {};
    uint32_t m_ElementCount = {};
    uint32_t m_ElementSize = {};
    D3D12_RESOURCE_FLAGS m_ResourceFlags = {};
};

class ConstantBuffer : public GpuBuffer
{
public:
private:
};

class StructuredBuffer : public GpuBuffer
{
public:
private:
};


