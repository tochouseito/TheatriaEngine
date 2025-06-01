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

DirectX12Common::DirectX12Common()
{
	Initialize();
}

DirectX12Common::~DirectX12Common()
{
}

void DirectX12Common::Initialize()
{
	// Log出力
	Log::Write(LogLevel::Info, "DirectX12Common::Initialize");
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
			// Log出力
			Log::Write(LogLevel::Info, "Enable Debug Layer");

			// デバッグレイヤーを有効化する
			debugController->EnableDebugLayer();

			// さらにGPU側でもチェックを行うようにする
			debugController->SetEnableGPUBasedValidation(TRUE);
		}
	}
#endif
	// DXGIファクトリーの生成
	// Log出力
	Log::Write(LogLevel::Info, "Create DXGI Factory");
	HRESULT hr;
	hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_DXGIFactory));
	Log::Write(LogLevel::Assert, "DXGI Factory created.", hr);
}

void DirectX12Common::CreateDevice()
{
	HRESULT hr;

	// 使用するアダプタ用の変数。最初にNullptrを入れておく
	Microsoft::WRL::ComPtr < IDXGIAdapter4> useAdapter = nullptr;

	// 良い順にアダプタを頼む
	for (UINT i = 0; m_DXGIFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {

		// アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);

		// 取得できないのは一大事
		Log::Write(LogLevel::Assert, "Adapter description", hr);

		// ソフトウェアアダプタでなければ
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// 採用したアダプタの情報をログに出力。wstringの方なので注意
			Log::Write(LogLevel::Info, ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
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
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&m_Device));

		// 指定した機能レベルでデバイスが生成できたか確認
		if (SUCCEEDED(hr)) {

			// 生成できたのでログ出力を行ってループを抜ける
			Log::Write(LogLevel::Info, std::format("Create D3D12 Device : {}", featureLevelStrings[i]));
			break;
		}
	}
	// デバイスの生成がうまくいかなかったので起動できない
	if (!m_Device) {
		Log::Write(LogLevel::Assert, "Failed to create D3D12 Device");
	}

	// 初期化完了ログ
	Log::Write(LogLevel::Info, "Complete create D3D12Device!!!");

	// デバイスの機能をチェック
	CheckD3D12Features();

#ifdef _DEBUG
	Microsoft::WRL::ComPtr< ID3D12InfoQueue> infoQueue;
	// フィルタリングを一時的に無効化してみる

	if (SUCCEEDED(m_Device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
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

// 各サポートチェック
void DirectX12Common::CheckD3D12Features()
{
	//// シェーダモデルをチェック.
	//{
	//	D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_5 };
	//	if (SUCCEEDED(m_Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel))))
	//	{
	//		Log::Write(LogLevel::Info, "Shader Model 6.5 is supported");
	//	} else
	//	{
	//		Log::Write(LogLevel::Assert, "Shader Model 6.5 is not supported");
	//	}
	//}

	//// メッシュシェーダをサポートしているかどうかチェック.
	//{
	//	D3D12_FEATURE_DATA_D3D12_OPTIONS7 features = {};
	//	if (SUCCEEDED(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &features, sizeof(features))))
	//	{
	//		std::cout << "Mesh Shader: ";
	//		if (features.MeshShaderTier != D3D12_MESH_SHADER_TIER_NOT_SUPPORTED)
	//		{
	//			// メッシュシェーダのティアを表示
	//			Log::Write(LogLevel::Info, std::format("Mesh Shader Tier: {}", (int)features.MeshShaderTier));
	//			std::cout << "Supported (Tier " << (int)features.MeshShaderTier << ")\n";
	//		} else
	//		{
	//			Log::Write(LogLevel::Assert, "Mesh Shader: Not Supported");
	//		}
	//	}
	//}

	// Raytracing
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
		if (SUCCEEDED(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5))))
		{
			std::cout << "Raytracing (DXR): ";
			if (options5.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
			{
				Log::Write(LogLevel::Info, std::format("Raytracing Tier: {}", (int)options5.RaytracingTier));
			} else {
				Log::Write(LogLevel::Assert, "Raytracing: Not Supported");
			}
		}
	}

	// Variable Rate Shading (VRS)
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6 = {};
		if (SUCCEEDED(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &options6, sizeof(options6))))
		{
			std::cout << "Variable Rate Shading (VRS): ";
			if (options6.VariableShadingRateTier != D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED)
			{
				Log::Write(LogLevel::Info, std::format("Variable Rate Shading Tier: {}", (int)options6.VariableShadingRateTier));
			} else
			{
				Log::Write(LogLevel::Assert, "Variable Rate Shading (VRS): Not Supported");
			}
		}
	}

	// Sampler Feedback
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7 = {};
		if (SUCCEEDED(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &options7, sizeof(options7))))
		{
			if (options7.SamplerFeedbackTier != D3D12_SAMPLER_FEEDBACK_TIER_NOT_SUPPORTED)
			{
				Log::Write(LogLevel::Info, std::format("Sampler Feedback Tier: {}", (int)options7.SamplerFeedbackTier));
			} else
			{
				Log::Write(LogLevel::Assert, "Sampler Feedback: Not Supported");
			}
		}
	}

	// ExecuteIndirect (Resource Binding Tierを間接的に利用可否チェック)
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
		if (SUCCEEDED(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options))))
		{
			if (options.ResourceBindingTier != D3D12_RESOURCE_BINDING_TIER_1)
			{
				Log::Write(LogLevel::Info, std::format("Resource Binding Tier: {}", (int)options.ResourceBindingTier));
			} else
			{
				Log::Write(LogLevel::Assert, "Resource Binding Tier: Not Supported");
			}
		}
	}

	//// Neural Network Acceleration (NNA)
	//{
	//	D3D12_FEATURE_DATA_D3D12_OPTIONS9 options9 = {};
	//	if (SUCCEEDED(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS9, &options9, sizeof(options9))))
	//	{
	//		if (options9.NNAccelerationTier != D3D12_NN_ACCELERATION_TIER_NOT_SUPPORTED)
	//		{
	//			Log::Write(LogLevel::Info, std::format("NN Acceleration Tier: {}", (int)options9.NNAccelerationTier));
	//		} else
	//		{
	//			Log::Write(LogLevel::Assert, "NN Acceleration: Not Supported");
	//		}
	//	}
	//}

	// Neural Shader
	{
		//D3D12_FEATURE_DATA_D3D12_OPTIONS15
	}
}
