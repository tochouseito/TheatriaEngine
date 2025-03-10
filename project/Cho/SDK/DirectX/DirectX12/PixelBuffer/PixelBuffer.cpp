#include "pch.h"
#include "PixelBuffer.h"

void PixelBuffer::CreateTextureResource(ID3D12Device8* device, const D3D12_RESOURCE_DESC& desc, const D3D12_CLEAR_VALUE& clearValue, const D3D12_RESOURCE_STATES& state)
{
	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// VRAM上に作る
	ID3D12Resource* pResource = GetResource();
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		state,
		&clearValue,
		IID_PPV_ARGS(&pResource)
	);
	ChoAssertLog("CreateCommittedResource", hr, __FILE__, __LINE__);
}
