#include "PrecompiledHeader.h"
#include "DSVManager.h"
#include"WinApp/WinApp.h"
#include"D3D12/D3DDevice/D3DDevice.h"
#include "D3D12/ResourceViewManager/ResourceViewManager.h"
#include<assert.h>

void DSVManager::Initialize(WinApp* win, D3DDevice* d3dDevice, ResourceViewManager* rvManager)
{
	win_ = win;
	d3dDevice_ = d3dDevice;
	rvManager_ = rvManager;

	// デスクリプタヒープ作成
	descriptorHeap_ = d3dDevice_->CreateDescriptorHeap(
		HEAP_TYPE,
		1,
		false
	);
	// デスクリプタサイズ取得
	descriptorSize_ = d3dDevice->GetDevice()->GetDescriptorHandleIncrementSize(
		HEAP_TYPE
	);
	// SRVを作成
	srvIndex_ = rvManager_->GetNewHandle();
	CreateDepthStencil();
}

void DSVManager::Resize()
{
	// まず解放
	resource_.Reset();
	resource_ = nullptr;

	CreateDepthStencil();
}

void DSVManager::CreateDepthStencil()
{
	resource_ = CreateDepthStencilTextureResource(
		WindowWidth(),
		WindowHeight()
	);
	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;       // Format。基本的にはResourceと合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;// 2dTexture
	// DSVHeapの先頭にDSVを作る
	d3dDevice_->GetDevice()->CreateDepthStencilView(
		resource_.Get(),
		&dsvDesc,
		descriptorHeap_->GetCPUDescriptorHandleForHeapStart()
	);

	rvManager_->GetHandle(srvIndex_).resource = resource_;
	rvManager_->CreateSRVForTexture2D(srvIndex_, DXGI_FORMAT_R24_UNORM_X8_TYPELESS, 1);
}

Microsoft::WRL::ComPtr<ID3D12Resource> DSVManager::CreateDepthStencilTextureResource(int32_t width, int32_t height)
{
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;// Textureの幅
	resourceDesc.Height = height;// Textureの高さ
	resourceDesc.MipLevels = 1;//mipmapの数
	resourceDesc.DepthOrArraySize = 1;// 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;// サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;// 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;// DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;// 1.0f（最大値）でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// フォーマット。Resourceと合わせる

	// Resourceの生成
	HRESULT hr;
	Microsoft::WRL::ComPtr < ID3D12Resource> resource = nullptr;
	hr = d3dDevice_->GetDevice()->CreateCommittedResource(
		&heapProperties,// Heapの設定
		D3D12_HEAP_FLAG_NONE,// Heapの特殊な設定。特になし
		&resourceDesc,// Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,// 深度値を書き込む状態にしておく
		&depthClearValue,// Clear最適値
		IID_PPV_ARGS(&resource));// 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}
