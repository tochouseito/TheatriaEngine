#pragma once

#include "SDK/DirectX/DirectX12/stdafx/stdafx.h"

class ResourceManager;

static const uint32_t bufferCount = 2;
struct SwapChainBuffer
{
	ComPtr<ID3D12Resource> pResource;
	uint32_t index = UINT32_MAX;
	uint32_t dHIndex = UINT32_MAX;
};
class SwapChain
{
public:
	// Constructor
	SwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* queue, const HWND& hwnd, const int32_t& width, const int32_t& height);
	// Destructor
	~SwapChain()
	{

	}
	// SwapChainの生成
	void CreateSwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* queue, const HWND& hwnd, const int32_t& width, const int32_t& height);
	// Resourceの生成
	void CreateResource(ID3D12Device8* device, ResourceManager* resourceManager);
	// Present
	void Present();
	SwapChainBuffer* GetBuffer(const uint32_t& index) { return m_BufferData[index].get(); }
	// GetBackBufferIndex
	UINT GetCurrentBackBufferIndex() const { return m_SwapChain->GetCurrentBackBufferIndex(); }
private:
	ComPtr<IDXGISwapChain4> m_SwapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 m_Desc = {};
	int32_t m_RefreshRate = {};
	// BufferIndex
	std::array<std::unique_ptr<SwapChainBuffer>, bufferCount> m_BufferData = {};
};

