#include "pch.h"
#include "PixelBuffer.h"

void PixelBuffer::CreateTextureResource(ID3D12Device8* device, D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE* clearValue, D3D12_RESOURCE_STATES& state)
{
	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// VRAM上に作る
	Create(device, heapProperties, D3D12_HEAP_FLAG_NONE, desc, state, clearValue);
}
