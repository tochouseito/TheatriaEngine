#include "pch.h"
#include "GpuResource.h"
#include "Core/ChoLog/ChoLog.h"
using namespace Cho;

void GpuResource::CreateResource(ID3D12Device* device, D3D12_HEAP_PROPERTIES& heapProperties, D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES InitialState, D3D12_CLEAR_VALUE* pClearValue)
{
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		heapFlags,
		&desc,
		InitialState,
		pClearValue,
		IID_PPV_ARGS(&m_pResource)
	);
	m_UseState = InitialState;
	Log::Write(LogLevel::Assert, "CreateCommittedResource", hr);
}

void GpuResource::RemakeResource(ID3D12Device* device, D3D12_HEAP_PROPERTIES& heapProperties, D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES InitialState, D3D12_CLEAR_VALUE* pClearValue)
{
	// リソースの破棄
	Destroy();
	// 新しい状態を設定
	m_UseState = InitialState;

	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		heapFlags,
		&desc,
		InitialState,
		pClearValue,
		IID_PPV_ARGS(&m_pResource)
	);
	m_UseState = InitialState;
	Log::Write(LogLevel::Assert, "CreateCommittedResource", hr);
}

void GpuResource::ResizeResource(ID3D12Device* device, ID3D12Resource** ppResource, D3D12_HEAP_PROPERTIES& heapProperties, D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES InitialState, D3D12_CLEAR_VALUE* pClearValue)
{
	if (*ppResource) {
		// 既存のリソースを破棄
		(*ppResource)->Release();
		*ppResource = nullptr;
	}
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		heapFlags,
		&desc,
		InitialState,
		pClearValue,
		IID_PPV_ARGS(ppResource)
	);
	Log::Write(LogLevel::Assert, "CreateCommittedResource", hr);
}
