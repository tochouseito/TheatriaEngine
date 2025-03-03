#pragma once

#include "Cho/SDK/DirectX/DirectX12/stdafx/stdafx.h"
#include "Cho/SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "Cho/SDK/DirectX/DirectX12/PixelBuffer/PixelBuffer.h"

class ResourceManager;
class BufferManager
{
public:// メンバ関数

	// Constructor
	BufferManager(ID3D12Device8* device,ResourceManager* resourceManager);
	// Destructor
	~BufferManager();
	// Create Buffer
	uint32_t CreatePixelBuffer(const uint32_t& width, const uint32_t& height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);
	// Get Buffer
	PixelBuffer& GetPixelBuffer(const uint32_t& index) { return m_PixelBuffers[index]; }
private:// メンバ変数
	ID3D12Device8* m_Device = nullptr;
	ResourceManager* m_ResourceManager = nullptr;

	FVector<PixelBuffer> m_PixelBuffers;
};

