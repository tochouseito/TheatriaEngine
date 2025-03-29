#include "pch.h"
#include "SwapChain.h"
#include "Resources/ResourceManager/ResourceManager.h"

SwapChain::SwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* queue, const HWND& hwnd, const int32_t& width, const int32_t& height)
{
	CreateSwapChain(dxgiFactory, queue,  hwnd, width, height);
}


void SwapChain::CreateSwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* queue, const HWND& hwnd, const int32_t& width, const int32_t& height)
{
	HRESULT hr;

	// スワップチェーンを生成する
	m_Desc.Width = width;                     // 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	m_Desc.Height = height;                   // 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	m_Desc.Format = PixelFormat;          // 色の形式
	m_Desc.SampleDesc.Count = 1;                         // マルチサンプルしない
	m_Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;// 描画のターゲットとして利用する
	m_Desc.BufferCount = bufferCount;                              // ダブルバッファ
	m_Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;   // モニタにうつしたら、中身を破棄
	m_Desc.Flags =
		DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING |
		DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT; // ティアリングサポート

	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory->CreateSwapChainForHwnd(
		queue,
		hwnd,
		&m_Desc,
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
}

void SwapChain::CreateResource(ID3D12Device8* device, ResourceManager* resourceManager)
{
	for (uint32_t i = 0; i < bufferCount; ++i)
	{
		m_BufferData[i] = std::make_unique<SwapChainBuffer>();
		m_BufferData[i]->pResource.Attach(nullptr);
		ID3D12Resource* pResource = nullptr;
		HRESULT hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&pResource));
		ChoAssertLog("Failed to get buffer from swap chain.", hr, __FILE__, __LINE__);
		m_BufferData[i]->pResource.Attach(pResource);
		m_BufferData[i]->index = i;
		m_BufferData[i]->dHIndex = resourceManager->GetRTVDHeap()->Create();
		// RTVの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = m_Desc.Format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;// 2dテクスチャとして書き込む
		// Viewの生成
		device->CreateRenderTargetView(
			m_BufferData[i]->pResource.Get(),
			&rtvDesc,
			resourceManager->GetRTVDHeap()->GetCpuHandle(m_BufferData[i]->dHIndex)
		);
	}
}

void SwapChain::Present()
{
	HRESULT hr = m_SwapChain->Present(1, 0);
	ChoAssertLog("Failed to present.", hr, __FILE__, __LINE__);
}

void SwapChain::WaitForGPU()
{
	HANDLE hWaitable = m_SwapChain->GetFrameLatencyWaitableObject();
	WaitForSingleObject(hWaitable, INFINITE);
}

