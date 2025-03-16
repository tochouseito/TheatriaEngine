#pragma once

#include "SDK/DirectX/DirectX12/stdafx/stdafx.h"

class ResourceManager;

static const uint32_t bufferCount = 2;

class SwapChain
{
public:
	// Constructor
	SwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* queue, const HWND& hwnd, const int32_t& width, const int32_t& height);
	// Destructor
	~SwapChain();
	// SwapChainの生成
	void CreateSwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* queue, const HWND& hwnd, const int32_t& width, const int32_t& height);
	// Present
	void Present();

	// GetBackBuffer
	ID3D12Resource* GetBackBuffer(const uint32_t& index) const;
	// SetIndex
	void SetIndex(const uint32_t& index, const uint32_t& value) { m_Index[index] = value; }
	// GetBackBufferIndex
	UINT GetBackBufferIndex() const { return m_SwapChain->GetCurrentBackBufferIndex(); }
private:
	ComPtr<IDXGISwapChain4> m_SwapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 m_SwapChainDesc = {};
	int32_t m_RefreshRate = {};

	// Index
	std::array<uint32_t, bufferCount> m_Index = {};
};

