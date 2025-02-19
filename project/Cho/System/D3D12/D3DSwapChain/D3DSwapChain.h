#pragma once

#include<dxgi1_6.h>
#include<d3d12.h>
#include<wrl.h>
#include<vector>

class WinApp;
class D3DSwapChain
{
public:// メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* win,IDXGIFactory7& dxgiFactory,ID3D12CommandQueue& commandQueue);

	void Present();

	//ID3D12Resource* GetResource(UINT index)const { return resources_[index].Get(); }

	IDXGISwapChain4* GetSwapChain()const { return swapChain_.Get(); }

	void SetHandleIndex(uint32_t index) { handleIndex_.push_back(index); }

	uint32_t GetBufferCount()const { return buffers; }

	void Resize();

private:// メンバ変数
	/*スワップチェーン*/
	uint32_t buffers = 2;// ダブルバッファリング
	Microsoft::WRL::ComPtr < IDXGISwapChain4>swapChain_;
	DXGI_SWAP_CHAIN_DESC1 desc_{};
	std::vector<uint32_t> handleIndex_;

	int32_t refreshRate_ = 0;
};

