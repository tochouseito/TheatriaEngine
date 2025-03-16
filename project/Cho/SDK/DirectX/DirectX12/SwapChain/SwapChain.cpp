#include "pch.h"
#include "SwapChain.h"
#include "Resources/ResourceManager/ResourceManager.h"

SwapChain::SwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* queue, const HWND& hwnd, const int32_t& width, const int32_t& height)
{
	CreateSwapChain(dxgiFactory, queue,  hwnd, width, height);
}

SwapChain::~SwapChain()
{

}

void SwapChain::CreateSwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* queue, const HWND& hwnd, const int32_t& width, const int32_t& height)
{
	HRESULT hr;

	// スワップチェーンを生成する
	m_SwapChainDesc.Width = width;                     // 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	m_SwapChainDesc.Height = height;                   // 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	m_SwapChainDesc.Format = PixelFormat;          // 色の形式
	m_SwapChainDesc.SampleDesc.Count = 1;                         // マルチサンプルしない
	m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;// 描画のターゲットとして利用する
	m_SwapChainDesc.BufferCount = bufferCount;                              // ダブルバッファ
	m_SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;   // モニタにうつしたら、中身を破棄
	m_SwapChainDesc.Flags =
		DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING |
		DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT; // ティアリングサポート

	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory->CreateSwapChainForHwnd(
		queue,
		hwnd,
		&m_SwapChainDesc,
		nullptr,
		nullptr,
		reinterpret_cast<IDXGISwapChain1**>(m_SwapChain.GetAddressOf())
	);

	ChoAssertLog("Failed to create swap chain.", hr, __FILE__, __LINE__);

	// リフレッシュレートを取得。floatで取るのは大変なので大体あってれば良いので整数で。
	HDC hdc = GetDC(hwnd);
	m_RefreshRate = GetDeviceCaps(hdc, VREFRESH);
	ReleaseDC(hwnd, hdc);

	// VSync共存型FPS固定のためにレイテンシ1
	m_SwapChain->SetMaximumFrameLatency(1);

	// OSが行うAlt+Enterのフルスクリーンは制御不能なので禁止
	dxgiFactory->MakeWindowAssociation(
		hwnd,
		DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER
	);
	//{// 1つ目
	//	// SwapChainからResourceを引っ張ってくる
	//	ComPtr<ID3D12Resource> pResource = nullptr;
	//	hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pResource));
	//	ChoAssertLog("Failed to get buffer from swap chain.", hr, __FILE__, __LINE__);
	//	// ピクセルバッファの作成
	//	ColorBuffer

	//	m_Index[0] = resourceManager->CreatePixelBuffer(width, height, PixelFormat, pResource.Get());
	//}
	//{// 2つ目
	//	// SwapChainからResourceを引っ張ってくる
	//	ComPtr<ID3D12Resource> pResource = nullptr;
	//	hr = m_SwapChain->GetBuffer(1, IID_PPV_ARGS(&pResource));
	//	ChoAssertLog("Failed to get buffer from swap chain.", hr, __FILE__, __LINE__);
	//	// ピクセルバッファの生成
	//	m_Index[1] = resourceManager->CreatePixelBuffer(width, height, PixelFormat, pResource.Get());
	//}
}

void SwapChain::Present()
{
	HRESULT hr = m_SwapChain->Present(1, 0);
	ChoAssertLog("Failed to present.", hr, __FILE__, __LINE__);
}

ID3D12Resource* SwapChain::GetBackBuffer(const uint32_t& index) const
{
	ComPtr<ID3D12Resource> pResource = nullptr;
	HRESULT hr = m_SwapChain->GetBuffer(index, IID_PPV_ARGS(&pResource));
	ChoAssertLog("Failed to get buffer from swap chain.", hr, __FILE__, __LINE__);
	return pResource.Get();
}
