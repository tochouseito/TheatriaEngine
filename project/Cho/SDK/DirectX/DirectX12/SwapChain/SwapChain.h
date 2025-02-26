#pragma once

#include "SDK/DirectX/DirectX12/stdafx/stdafx.h"

class SwapChain
{
public:
	SwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* queue,const HWND& hwnd,const int32_t& width,const int32_t& height);
	~SwapChain();

private:
	ComPtr<IDXGISwapChain4> m_SwapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 m_SwapChainDesc = {};
	int32_t m_RefreshRate = {};

	uint32_t m_BufferCount = 2;
};

