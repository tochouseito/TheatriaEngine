#pragma once

#include<d3d12.h>
#include<wrl.h>
#include<cstdint>

class WinApp;
class D3DDevice;
class ResourceViewManager;
class DSVManager
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* win, D3DDevice* d3dDevice, ResourceViewManager* rvManager);

	ID3D12DescriptorHeap* GetDescriptorHeap()const { return descriptorHeap_.Get(); }

	uint32_t GetSRVIndex()const { return srvIndex_; }

	ID3D12Resource* GetResource()const { return resource_.Get(); }

	void Resize();

private:// メンバ関数

	/// <summary>
	/// 深度リソースの作成
	/// </summary>
	void CreateDepthStencil();

	/// <summary>
	/// 深度リソースの作成
	/// </summary>
	Microsoft::WRL::ComPtr < ID3D12Resource	> CreateDepthStencilTextureResource(
		int32_t width, int32_t height
	);

private:
	/*借りインスタンス*/
	WinApp* win_ = nullptr;
	D3DDevice* d3dDevice_ = nullptr;
	ResourceViewManager* rvManager_ = nullptr;

	// デスクリプタサイズ
	uint32_t descriptorSize_ = 0;

	// デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

	// ヒープタイプ
	static const D3D12_DESCRIPTOR_HEAP_TYPE HEAP_TYPE =
		D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	// 深度リソース
	Microsoft::WRL::ComPtr < ID3D12Resource>resource_;
	uint32_t srvIndex_ = 0;

	// デバッグ用
	Microsoft::WRL::ComPtr < ID3D12Resource>debugResource_;
	uint32_t debugSRVIndex_ = 0;
};

