#include"PrecompiledHeader.h"
#include "DXGIFactory.h"
#include<assert.h>

void DXGIFactory::Initialize([[maybe_unused]] bool enableDebugLayer)
{
#ifdef _DEBUG
	/*
	[ INITIALIZATION MESSAGE #1016: CREATEDEVICE_DEBUG_LAYER_STARTUP_OPTIONS]
	上記の警告メッセージが出てくるがデバッグ時のみのものなので無視していい
	*/
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController;
	if (enableDebugLayer) {
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {

			// デバッグレイヤーを有効化する
			debugController->EnableDebugLayer();

			// さらにGPU側でもチェックを行うようにする
			debugController->SetEnableGPUBasedValidation(TRUE);
		}
	}
#endif
	// DXGIファクトリーの生成
	HRESULT hr;
	hr = CreateDXGIFactory2(0,IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr));
}
