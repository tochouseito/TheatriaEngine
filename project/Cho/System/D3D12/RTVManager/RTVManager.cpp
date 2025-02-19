#include "PrecompiledHeader.h"
#include "RTVManager.h"
#include"Base/Format.h"
#include"D3D12/D3DDevice/D3DDevice.h"
#include"D3D12/D3DSwapChain/D3DSwapChain.h"
#include"assert.h"

void RTVManager::Initialize(D3DDevice* d3dDevice, D3DSwapChain* d3dSwapChain)
{
	d3dDevice_ = d3dDevice;
	d3dSwapChain_ = d3dSwapChain;

	// デスクリプタヒープの生成
	descriptorHeap_ = d3dDevice_->CreateDescriptorHeap(
		HEAP_TYPE,
		kMaxDescriptor,
		false
	);
	// デスクリプタ1個分のサイズを取得して記録
	descriptorSize_ =
		d3dDevice_->GetDevice()->GetDescriptorHandleIncrementSize(
			HEAP_TYPE
		);

	// スワップチェーン用のRTVを作成
	CreateSwapChainRTV();
}

uint32_t RTVManager::CreateRTV(ID3D12Resource* textureResource, DXGI_FORMAT format)
{
	uint32_t index = GetNewHandle();

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	// RTVの設定
	rtvDesc.Format = format;// 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;// 2dテクスチャとして書き込む

	d3dDevice_->GetDevice()->CreateRenderTargetView(
		textureResource,
		&rtvDesc,
		handles[index].CPUHandle
	);

	return index;
}

uint32_t RTVManager::GetNewHandle()
{
	uint32_t index = Allocate();

	RTVHandleData& descriptorData = handles[index];

	descriptorData.CPUHandle = GetCPUDescriptorHandle(index);

	return index;
}

void RTVManager::ResizeSwapChain()
{
	// バッファリングの数だけ解放
	for (uint32_t i = 0; i < d3dSwapChain_->GetBufferCount(); i++) {
		handles[i].resource.Reset();
		handles[i].resource = nullptr;
	}
	d3dSwapChain_->Resize();

	HRESULT hr;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	// RTVの設定
	rtvDesc.Format = dxgiFormat;// 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;// 2dテクスチャとして書き込む

	for (uint32_t i = 0; i < d3dSwapChain_->GetBufferCount(); i++) {
		// SwapChainからResourceを引っ張ってくる
		hr = d3dSwapChain_->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&handles[i].resource));

		d3dDevice_->GetDevice()->CreateRenderTargetView(
			handles[i].resource.Get(),
			&rtvDesc,
			handles[i].CPUHandle
		);
	}
}

void RTVManager::RemakeRTV(uint32_t& index, ID3D12Resource* textureResource, DXGI_FORMAT format)
{
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	// RTVの設定
	rtvDesc.Format = format;// 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;// 2dテクスチャとして書き込む

	d3dDevice_->GetDevice()->CreateRenderTargetView(
		textureResource,
		&rtvDesc,
		handles[index].CPUHandle
	);
}

void RTVManager::CreateRenderTargetView()
{
}

void RTVManager::CreateSwapChainRTV()
{
	HRESULT hr;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	// RTVの設定
	rtvDesc.Format = dxgiFormat;// 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;// 2dテクスチャとして書き込む

	//// ディスクリプタの先頭取得する
	//D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle =
	//	descriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	// RTVを２つ作るのでディスクリプタを２つ用意
	// まず１つ目を作る。１つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	uint32_t swpIndex = GetNewHandle();
	d3dSwapChain_->SetHandleIndex(swpIndex);
	// SwapChainからResourceを引っ張ってくる
	hr = d3dSwapChain_->GetSwapChain()->GetBuffer(swpIndex, IID_PPV_ARGS(&handles[swpIndex].resource));
	// うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));

	d3dDevice_->GetDevice()->CreateRenderTargetView(
		handles[swpIndex].resource.Get(),
		&rtvDesc,
		handles[swpIndex].CPUHandle
	);

	// ２つ目のディスクリプタハンドル
	uint32_t swpIndex2 = GetNewHandle();
	d3dSwapChain_->SetHandleIndex(swpIndex2);

	hr = d3dSwapChain_->GetSwapChain()->GetBuffer(swpIndex2, IID_PPV_ARGS(&handles[swpIndex2].resource));
	// うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));

	d3dDevice_->GetDevice()->CreateRenderTargetView(
		handles[swpIndex2].resource.Get(),
		&rtvDesc,
		handles[swpIndex2].CPUHandle
	);
}

uint32_t RTVManager::Allocate()
{
	if (useIndex_ >= kMaxDescriptor) {
		assert(0);
	}
	// returnする番号を一旦記録する
	int index = useIndex_;
	// 次回のため番号を1進める
	useIndex_++;
	// 上で記録した番号をreturn
	return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE RTVManager::GetCPUDescriptorHandle(uint32_t& index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize_ * index);
	return handleCPU;
}
