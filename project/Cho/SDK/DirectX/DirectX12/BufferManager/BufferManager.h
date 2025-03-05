#pragma once

#include "Cho/SDK/DirectX/DirectX12/stdafx/stdafx.h"
#include "Cho/SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "Cho/SDK/DirectX/DirectX12/PixelBuffer/PixelBuffer.h"

// 有効なBufferの型
//template<typename T>
//using BufferType = std::enable_if_t<
//	std::is_same_v<T, PixelBuffer>
//>;

class ResourceManager;
class BufferManager
{
public:// メンバ関数

	// Constructor
	BufferManager(ID3D12Device8* device,ResourceManager* resourceManager);
	// Destructor
	~BufferManager();
	// Create Buffer
	/*template<typename T, typename = BufferType<T>>
	void CreateBuffer();*/

	uint32_t AddPixelBuffer() { return static_cast<uint32_t>(m_PixelBuffers.push_back(PixelBuffer())); }
	uint32_t CreatePixelBuffer(const uint32_t& width, const uint32_t& height, DXGI_FORMAT format,const D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,const uint32_t& dhIndex, ID3D12Resource* pResource = nullptr);
	// Get Buffer
	PixelBuffer GetPixelBuffer(const uint32_t& index) const { return m_PixelBuffers[index]; }
private:// メンバ変数
	ID3D12Device8* m_Device = nullptr;
	ResourceManager* m_ResourceManager = nullptr;

	FVector<PixelBuffer> m_PixelBuffers;
};

