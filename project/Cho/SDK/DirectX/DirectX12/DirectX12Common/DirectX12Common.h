#pragma once

/*--------------------------------------------
DirectX12共通クラス
--------------------------------------------*/

#include "SDK/DirectX/DirectX12/stdafx/stdafx.h"

#include<dxgidebug.h>

class DirectX12Common
{
public:// メンバ関数

	// Constructor
	DirectX12Common();
	// Destructor
	~DirectX12Common();

	// 初期化
	void Initialize();

	// 終了処理
	void Finalize();

	/*Getter*/
	ID3D12Device8* GetDevice() const { return m_Device.Get(); }
	IDXGIFactory7* GetDXGIFactory() const { return m_DXGIFactory.Get(); }

private:// メンバ関数

	// DXGIファクトリーの生成
	void CreateDXGIFactory(const bool& enableDebugLayer);

	// デバイスの生成
	void CreateDevice();

    // 各サポートチェック
    void CheckD3D12Features();

private:// メンバ変数
	ComPtr<IDXGIFactory7> m_DXGIFactory = nullptr;// DXGIファクトリ
	ComPtr<ID3D12Device8> m_Device = nullptr;// デバイス
};

/*--------------------------------------------
リソースリークチェッカー
--------------------------------------------*/
class ResourceLeakChecker
{
public:// メンバ関数

	// デストラクタ
	~ResourceLeakChecker();
};

