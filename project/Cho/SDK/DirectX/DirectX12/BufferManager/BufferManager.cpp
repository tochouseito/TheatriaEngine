#include "pch.h"
#include "BufferManager.h"
#include "Resources/ResourceManager/ResourceManager.h"

BufferManager::BufferManager(ID3D12Device8* device,ResourceManager* resourceManager)
{
	m_ResourceManager = resourceManager;
	m_Device = device;
}

BufferManager::~BufferManager()
{
}

uint32_t BufferManager::CreatePixelBuffer(ID3D12Device8* device, ResourceManager* resourceManager, const uint32_t& width, const uint32_t& height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE)
{
	PixelBuffer buffer;
	buffer.Create(
		m_Device,
		width, height, format, flags,
		m_ResourceManager->GetRTVDHeap()->Create()
	);
	return m_PixelBuffers.push_back(std::move(buffer));
}
