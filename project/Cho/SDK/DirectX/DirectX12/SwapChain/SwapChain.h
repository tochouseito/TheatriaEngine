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
		//// ① Presentの完了を100%待つ
		//HANDLE frameEvent = m_SwapChain->GetFrameLatencyWaitableObject();
		//if (frameEvent)
		//{
		//	WaitForSingleObject(frameEvent, INFINITE); // 🔒 Presentの完了まで確実に待つ
		//}

		//// ② バックバッファを先に解放
		//for (uint32_t i = 0; i < bufferCount; ++i)
		//{
		//	if (m_BufferData[i])
		//	{
		//		m_BufferData[i]->pResource.Reset();
		//		m_BufferData[i].reset();
		//	}
		//}

		//// ③ 同じフラグで ResizeBuffers を呼ぶ（変更しない！）
		//m_SwapChain->ResizeBuffers(
		//	bufferCount,
		//	0, 0,
		//	DXGI_FORMAT_R8G8B8A8_UNORM,
		//	m_Desc.Flags // ← 作成時の値をそのまま
		//);

		//// ④ SwapChain 自体を解放
		//m_SwapChain.Reset();
		WaitForGPU();
	}

	// SwapChainの生成
	void CreateSwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* queue, const HWND& hwnd, const int32_t& width, const int32_t& height);
	// Resourceの生成
	void CreateResource(ID3D12Device8* device,ResourceManager* resourceManager);
	// Present
	void Present();
	// Wait
	void WaitForGPU();
	// GetBuffer
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

