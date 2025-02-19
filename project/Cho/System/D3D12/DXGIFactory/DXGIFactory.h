#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<wrl.h>

class DXGIFactory
{
public:// メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(bool enableDebugLayer);

	IDXGIFactory7* GetDXGIFactory()const { return dxgiFactory_.Get(); }

private:// メンバ変数

	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;

};

