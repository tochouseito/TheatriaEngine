#include "PrecompiledHeader.h"
#include "D3DSwapChain.h"
#include"WinApp/WinApp.h"
#include "Format.h"
#include<assert.h>

void D3DSwapChain::Initialize(WinApp* win, IDXGIFactory7& dxgiFactory, ID3D12CommandQueue& commandQueue)
{
	HRESULT hr;

	// スワップチェーンを生成する
	desc_.Width = WindowWidth();                     // 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	desc_.Height = WindowHeight();                   // 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	desc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;          // 色の形式
	desc_.SampleDesc.Count = 1;                         // マルチサンプルしない
	desc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;// 描画のターゲットとして利用する
	desc_.BufferCount = buffers;                              // ダブルバッファ
	desc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;   // モニタにうつしたら、中身を破棄
	desc_.Flags =
		DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING |
		DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT; // ティアリングサポート

	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory.CreateSwapChainForHwnd(
		&commandQueue, win->GetHwnd(), &desc_,
		nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf())
	);
	
	assert(SUCCEEDED(hr));

	// リフレッシュレートを取得。floatで取るのは大変なので大体あってれば良いので整数で。
	HDC hdc = GetDC(win->GetHwnd());
	refreshRate_ = GetDeviceCaps(hdc, VREFRESH);
	ReleaseDC(win->GetHwnd(), hdc);

	// VSync共存型fps固定のためにレイテンシ1
	swapChain_->SetMaximumFrameLatency(1);

	// OSが行うAlt+Enterのフルスクリーンは制御不能なので禁止
	dxgiFactory.MakeWindowAssociation(
		win->GetHwnd(), DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
}

void D3DSwapChain::Present()
{
	//HRESULT result;
	// GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);
	// バッファをフリップ。60fps固定のため、30fpsなどのモニタはティアリング覚悟で垂直同期無視
	/*static constexpr int32_t kThreasholdRefreshRate = 58;
	result = swapChain_->Present(refreshRate_ < kThreasholdRefreshRate ? 0 : 1, 0);*/
}

void D3DSwapChain::Resize()
{
	HRESULT hr;

	hr = swapChain_->ResizeBuffers(
		buffers,
		WindowWidth(),
		WindowHeight(),
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT
	);
	assert(SUCCEEDED(hr));
}
