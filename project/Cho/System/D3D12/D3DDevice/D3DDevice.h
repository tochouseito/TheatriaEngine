#pragma once

#include<d3d12.h>
#include<dxgi1_6.h>
#include<wrl.h>

class D3DDevice
{
public:// メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(IDXGIFactory7& dxgiFactory);
	
	/// <summary>
	/// ディスクリプタヒープの生成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		 D3D12_DESCRIPTOR_HEAP_TYPE heapType,
		UINT numDescriptors,
		bool shaderVisible
	);

	ID3D12Device8* GetDevice()const { return device.Get(); }

private:
	/*デバイス*/
	Microsoft::WRL::ComPtr<ID3D12Device8> device;
};

