#pragma once

/*--------------------------------------------
DirectX12共通クラス
--------------------------------------------*/

#include<wrl.h>
using Microsoft::WRL::ComPtr;

#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxgidebug.h>

class DirectX12Common
{
public:// メンバ関数

	// 初期化
	void Initialize();

	// 終了処理
	void Finalize();

	/*Getter*/
	ID3D12Device8* GetDevice() const { return m_Device.Get(); }

private:// メンバ関数

	// DXGIファクトリーの生成
	void CreateDXGIFactory(const bool& enableDebugLayer);

	// デバイスの生成
	void CreateDevice();
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

