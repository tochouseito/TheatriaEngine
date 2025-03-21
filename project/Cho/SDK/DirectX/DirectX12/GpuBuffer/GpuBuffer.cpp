#include "pch.h"
#include "GpuBuffer.h"

void GpuBuffer::Create(ID3D12Device* device, D3D12_HEAP_PROPERTIES& heapProperties, D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES InitialState, D3D12_CLEAR_VALUE* pClearValue)
{
	GpuResource::Create(device, heapProperties, heapFlags, desc, InitialState, pClearValue);
}

