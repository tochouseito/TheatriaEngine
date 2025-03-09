#pragma once

#include "Cho/SDK/DirectX/DirectX12/stdafx/stdafx.h"
#include "Cho/SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "Cho/SDK/DirectX/DirectX12/ColorBuffer/ColorBuffer.h"

// 有効なBufferの型
template<typename T>
using BufferType = std::enable_if_t<
	std::is_same_v<T, ColorBuffer>
>;
template<typename T>
using BufferDescType = std::enable_if_t<
	std::is_same_v<T, BUFFER_COLOR_DESC>
>;

class ResourceManager;
class BufferManager
{
public:// メンバ関数

	// Constructor
	BufferManager(ID3D12Device8* device,ResourceManager* resourceManager);
	// Destructor
	~BufferManager();
	// Create Buffer
	// return: BufferNumber
	template<typename T, typename = BufferDescType<T>>
	uint32_t CreateBuffer(const T& desc, ID3D12Resource* pResource = nullptr) { return CreateBufferProcess(desc, pResource); }

	// Get Buffer
	template<typename T, typename = BufferType<T>>
	T* GetBuffer(const uint32_t& index) const { return GetBufferProcess(index); }
private:
	// ColorBufferProcess
	uint32_t CreateBufferProcess(const BUFFER_COLOR_DESC& desc, ID3D12Resource* pResource = nullptr);
	ColorBuffer* GetBufferProcess(const uint32_t& index) { return &m_ColorBuffers[index]; }

	ID3D12Device8* m_Device = nullptr;
	ResourceManager* m_ResourceManager = nullptr;
	// BufferContainer
	FVector<ColorBuffer> m_ColorBuffers;
};

