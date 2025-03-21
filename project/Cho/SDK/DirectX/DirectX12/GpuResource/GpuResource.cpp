#include "pch.h"
#include "GpuResource.h"

void GpuResource::Create(ID3D12Device* device, D3D12_HEAP_PROPERTIES& heapProperties, D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES InitialState, D3D12_CLEAR_VALUE* pClearValue)
{
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		heapFlags,
		&desc,
		InitialState,
		pClearValue,
		IID_PPV_ARGS(&m_pResource)
	);
	ChoAssertLog("Failed to create gpu resource.", hr, __FILE__, __LINE__);
}
