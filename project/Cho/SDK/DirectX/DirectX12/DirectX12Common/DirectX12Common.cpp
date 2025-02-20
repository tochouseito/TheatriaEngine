#include "pch.h"
#include "DirectX12Common.h"

ResourceLeakChecker::~ResourceLeakChecker()
{
	// リソースリークチェック
	Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	}
}

void DirectX12Common::Initialize()
{
	// DXGIファクトリーの生成
	CreateDXGIFactory(true);
	// デバイスの生成
	CreateDevice();
}

void DirectX12Common::Finalize()
{
}

void DirectX12Common::CreateDXGIFactory([[maybe_unused]] const bool& enableDebugLayer)
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
	hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory));
#ifdef _DEBUG
	assert(SUCCEEDED(hr));
#endif // _DEBUG
}

void DirectX12Common::CreateDevice()
{
	HRESULT hr;

	// 使用するアダプタ用の変数。最初にNullptrを入れておく
	Microsoft::WRL::ComPtr < IDXGIAdapter4> useAdapter = nullptr;

	// 良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {

		// アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);

		// 取得できないのは一大事
		assert(SUCCEEDED(hr));

		// ソフトウェアアダプタでなければ
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// 採用したアダプタの情報をログに出力。wstringの方なので注意
			//Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		// ソフトウェアアダプタの場合は見なかったことにする
		useAdapter = nullptr;
	}
	// 適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);

	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
	};
	const char* featureLevelStrings[] = {
		"12.2",
		"12.1",
		"12.0"
	};

	// 高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {

		// 採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device));

		// 指定した機能レベルでデバイスが生成できたか確認
		if (SUCCEEDED(hr)) {

			// 生成できたのでログ出力を行ってループを抜ける
			//Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	// デバイスの生成がうまくいかなかったので起動できない
	assert(device != nullptr);

	// 初期化完了ログ
	//Log("Complete create D3D12Device!!!\n");

	// シェーダモデルをチェック.
	{
		D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_5 };
		hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
		if (FAILED(hr) || (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_5))
		{
			assert(0 && "Error : Shader Model 6.5 is not supported.");
		}
	}

	// メッシュシェーダをサポートしているかどうかチェック.
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS7 features = {};
		hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &features, sizeof(features));
		if (FAILED(hr) || (features.MeshShaderTier == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED))
		{
			assert(0 && "Error : Mesh Shader is not supported.");
		}
	}

#ifdef _DEBUG
	Microsoft::WRL::ComPtr< ID3D12InfoQueue> infoQueue;
	// フィルタリングを一時的に無効化してみる

	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// ヤバいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);

		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {

			// Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			// https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;

		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}

#endif // DEBUG
}
