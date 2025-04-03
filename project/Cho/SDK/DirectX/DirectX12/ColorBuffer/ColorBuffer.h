#pragma once
#include "Cho/SDK/DirectX/DirectX12/PixelBuffer/PixelBuffer.h"

class ColorBuffer : public PixelBuffer
{
public:
	// Constructor
	ColorBuffer()
		: PixelBuffer()
	{
	}
	// Constructor
	ColorBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState)
		: PixelBuffer(pResource, CurrentState)
	{
	}
	// Destructor
	~ColorBuffer()
	{
		m_RTVCpuHandle = {};
		m_RTVHandleIndex = std::nullopt;
	}
	bool CreateRTV(ID3D12Device8* device, D3D12_RENDER_TARGET_VIEW_DESC& rtvDesc, DescriptorHeap* pDescriptorHeap);
private:
	// ディスクリプタハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE m_RTVCpuHandle = {};
	// ディスクリプタハンドルインデックス
	std::optional<uint32_t> m_RTVHandleIndex = std::nullopt;
};

