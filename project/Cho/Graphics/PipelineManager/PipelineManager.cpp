#include "pch.h"
#include "PipelineManager.h"

void PipelineManager::Initialize(ID3D12Device8* device)
{
	m_pShaderCompiler->Initialize(device);
	CreatePipelineDemo(device);
	CreatePipelineScreenCopy(device);
	CreatePipelineIntegrate(device);
	CreatePipelineLine(device);
	CreatePipelineParticle(device);
	CreatePipelineParticleInit(device);
	CreatePipelineParticleUpdate(device);
	CreatePipelineParticleEmit(device);
	CreatePipelineEffectEditor(device);
	CreatePipelineEffectEditorInit(device);
	CreatePipelineEffectEditorUpdate(device);
	CreatePipelineEffectEditorEmit(device);
	CreatePipelineUI(device);
	CreatePipelineSkinningCS(device);
}

std::vector<std::pair<uint32_t, std::string>> PipelineManager::CreateRootParameters(ID3D12ShaderReflection* pReflector, std::vector<D3D12_ROOT_PARAMETER>& rootParameters, std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges, D3D12_SHADER_VISIBILITY VISIBILITY)
{
	std::vector<std::pair<uint32_t, std::string>> rootParm;

	assert(pReflector);
	// シェーダーの詳細情報を取得
	D3D12_SHADER_DESC shaderDesc;
	pReflector->GetDesc(&shaderDesc);

	//descriptorRanges.reserve(shaderDesc.BoundResources); // メモリの再割り当てを防ぐために容量を確保
	uint32_t offset = 0;
	// リソースバインディング情報を取得
	for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
	{
		D3D12_SHADER_INPUT_BIND_DESC resourceDesc;
		pReflector->GetResourceBindingDesc(i, &resourceDesc);
		if (resourceDesc.Type == D3D_SIT_SAMPLER)
		{// サンプラーだったら抜ける
			offset = 1;
			continue;
		}
		std::pair<uint32_t, std::string> desc;
		desc.first = i - offset;
		desc.second = resourceDesc.Name;
		rootParm.push_back(desc);

		D3D12_ROOT_PARAMETER rootParam = {};

		if (resourceDesc.Type == D3D_SIT_STRUCTURED)  // ストラクチャーバッファーの場合
		{
			D3D12_DESCRIPTOR_RANGE descriptorRange = {};
			descriptorRange.BaseShaderRegister = resourceDesc.BindPoint;
			descriptorRange.NumDescriptors = 1;
			descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			descriptorRanges.push_back(descriptorRange);

			rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParam.ShaderVisibility = VISIBILITY;					   // 必要に応じて設定
			rootParam.DescriptorTable.pDescriptorRanges = &descriptorRanges.back();// Tableの中身の配列を指定
			rootParam.DescriptorTable.NumDescriptorRanges = 1;			   // Tableで利用する数
		} else if (resourceDesc.Type == D3D_SIT_TEXTURE) // シェーダーリソースの場合
		{
			D3D12_DESCRIPTOR_RANGE descriptorRange = {};
			descriptorRange.BaseShaderRegister = resourceDesc.BindPoint;
			descriptorRange.NumDescriptors = 1;
			descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			descriptorRanges.push_back(descriptorRange);

			rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParam.ShaderVisibility = VISIBILITY;					   // 必要に応じて設定
			rootParam.DescriptorTable.pDescriptorRanges = &descriptorRanges.back();// Tableの中身の配列を指定
			rootParam.DescriptorTable.NumDescriptorRanges = 1;			   // Tableで利用する数
		} else if (resourceDesc.Type == D3D_SIT_CBUFFER) // 定数バッファの場合
		{
			D3D12_DESCRIPTOR_RANGE descriptorRange = {};
			descriptorRange.BaseShaderRegister = resourceDesc.BindPoint;
			descriptorRange.NumDescriptors = 1;
			descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			descriptorRanges.push_back(descriptorRange);

			//rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			//rootParam.ShaderVisibility = VISIBILITY;					   // 必要に応じて設定
			//rootParam.DescriptorTable.pDescriptorRanges = &descriptorRanges.back();// Tableの中身の配列を指定
			//rootParam.DescriptorTable.NumDescriptorRanges = 1;			   // Tableで利用する数

			rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootParam.ShaderVisibility = VISIBILITY;
			rootParam.Descriptor.ShaderRegister = resourceDesc.BindPoint;
		} else if (resourceDesc.Type == D3D_SIT_UAV_RWTYPED)  // 書き込み可能なリソースの場合
		{
			D3D12_DESCRIPTOR_RANGE descriptorRange = {};
			descriptorRange.BaseShaderRegister = resourceDesc.BindPoint;
			descriptorRange.NumDescriptors = 1;
			descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			descriptorRanges.push_back(descriptorRange);

			rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;      // 必要に応じて設定
			rootParam.DescriptorTable.pDescriptorRanges = &descriptorRanges.back();// Tableの中身の配列を指定
			rootParam.DescriptorTable.NumDescriptorRanges = 1;			   // Tableで利用する数
		} else if (resourceDesc.Type == D3D_SIT_UAV_RWSTRUCTURED)  // 書き込み可能なリソースの場合
		{
			D3D12_DESCRIPTOR_RANGE descriptorRange = {};
			descriptorRange.BaseShaderRegister = resourceDesc.BindPoint;
			descriptorRange.NumDescriptors = 1;
			descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			descriptorRanges.push_back(descriptorRange);

			rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;      // 必要に応じて設定
			rootParam.DescriptorTable.pDescriptorRanges = &descriptorRanges.back();// Tableの中身の配列を指定
			rootParam.DescriptorTable.NumDescriptorRanges = 1;			   // Tableで利用する数
		}

		rootParameters.push_back(rootParam);
	}
	return rootParm;
}

void PipelineManager::CreatePipelineDemo(ID3D12Device8* device)
{
	HRESULT hr = {};
	// Compile shaders
	ComPtr<IDxcBlob> pVSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/Demo/Demo.VS.hlsl",
			L"vs_6_5"
			);
	ComPtr<ID3D12ShaderReflection> pVSReflection = m_pShaderCompiler->ReflectShader(pVSBlob.Get());

	ComPtr<IDxcBlob> pPSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/Demo/Demo.PS.hlsl",
			L"ps_6_5"
		);
	ComPtr<ID3D12ShaderReflection> pPSReflection = m_pShaderCompiler->ReflectShader(pPSBlob.Get());
	// CreateRootSignature
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// RootParameters
	std::vector< D3D12_ROOT_PARAMETER>rootParameters;
	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;
	std::vector<std::pair<uint32_t, std::string>> parameterPair;
	// Vertex Buffer
	parameterPair = CreateRootParameters(pVSReflection.Get(), rootParameters, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);
	for (std::pair<uint32_t, std::string> rootParam : parameterPair)
	{
		m_DemoPSO.rootParameters.push_back(rootParam);
	}
	// Pixel Shader
	parameterPair = CreateRootParameters(pPSReflection.Get(), rootParameters, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);
	uint32_t offset = static_cast<uint32_t>(parameterPair.size());
	for (std::pair<uint32_t, std::string> rootParam : parameterPair)
	{
		rootParam.first += offset;
		m_DemoPSO.rootParameters.push_back(rootParam);
	}
	for (int32_t i = 0; i < rootParameters.size(); ++i)
	{
		rootParameters[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}
	rootSignatureDesc.pParameters = rootParameters.data();
	rootSignatureDesc.NumParameters = static_cast<UINT>(rootParameters.size());
	// Static Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	rootSignatureDesc.pStaticSamplers = staticSamplers;
	rootSignatureDesc.NumStaticSamplers = _countof(staticSamplers);
	// Serialize RootSignature
	ComPtr<ID3DBlob> pSignature;
	ComPtr<ID3DBlob> pError;
	hr = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&pSignature,
		&pError
	);
	if (FAILED(hr))
	{
		Log::Write(LogLevel::Info, reinterpret_cast<const char*>(pError->GetBufferPointer()));
	}
	// Create RootSignature
	hr = device->CreateRootSignature(
		0,
		pSignature->GetBufferPointer(),
		pSignature->GetBufferSize(),
		IID_PPV_ARGS(&m_DemoPSO.rootSignature)
	);
	Log::Write(LogLevel::Assert, "Root signature created.", hr);
	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[5] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].SemanticName = "TEXCOORD";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[2].SemanticName = "NORMAL";
	inputElementDesc[2].SemanticIndex = 0;
	inputElementDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[3].SemanticName = "COLOR";
	inputElementDesc[3].SemanticIndex = 0;
	inputElementDesc[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[4].SemanticName = "VERTEXID";
	inputElementDesc[4].SemanticIndex = 0;
	inputElementDesc[4].Format = DXGI_FORMAT_R32_UINT;
	inputElementDesc[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = _countof(inputElementDesc);

	// BlendState
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilState
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// Create PSO
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = m_DemoPSO.rootSignature.Get();
	psoDesc.VS = { pVSBlob->GetBufferPointer(),pVSBlob->GetBufferSize() };
	psoDesc.PS = { pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize() };
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = PixelFormat;
	// Create PSO
	hr = device->CreateGraphicsPipelineState(
		&psoDesc,
		IID_PPV_ARGS(&m_DemoPSO.pso)
	);
	Log::Write(LogLevel::Assert, "Pipeline state created.", hr);
}

void PipelineManager::CreatePipelineScreenCopy(ID3D12Device8* device)
{
	HRESULT hr = {};
	// Compile shaders
	ComPtr<IDxcBlob> pVSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/ScreenCopy/ScreenCopy.VS.hlsl",
			L"vs_6_5"
		);
	ComPtr<ID3D12ShaderReflection> pVSReflection = m_pShaderCompiler->ReflectShader(pVSBlob.Get());

	ComPtr<IDxcBlob> pPSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/ScreenCopy/ScreenCopy.PS.hlsl",
			L"ps_6_5"
		);
	ComPtr<ID3D12ShaderReflection> pPSReflection = m_pShaderCompiler->ReflectShader(pPSBlob.Get());
	// CreateRootSignature
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// RootParameters
	std::vector< D3D12_ROOT_PARAMETER>rootParameters;
	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;
	std::vector<std::pair<uint32_t, std::string>> parameterPair;
	// Vertex Buffer
	parameterPair = CreateRootParameters(pVSReflection.Get(), rootParameters, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);
	for (std::pair<uint32_t, std::string> rootParam : parameterPair)
	{
		m_ScreenCopyPSO.rootParameters.push_back(rootParam);
	}
	// Pixel Shader
	parameterPair = CreateRootParameters(pPSReflection.Get(), rootParameters, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);
	uint32_t offset = static_cast<uint32_t>(parameterPair.size());
	for (std::pair<uint32_t, std::string> rootParam : parameterPair)
	{
		rootParam.first += offset;
		m_ScreenCopyPSO.rootParameters.push_back(rootParam);
	}
	for (int32_t i = 0; i < rootParameters.size(); ++i)
	{
		rootParameters[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}
	rootSignatureDesc.pParameters = rootParameters.data();
	rootSignatureDesc.NumParameters = static_cast<UINT>(rootParameters.size());
	// Static Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	rootSignatureDesc.pStaticSamplers = staticSamplers;
	rootSignatureDesc.NumStaticSamplers = _countof(staticSamplers);
	// Serialize RootSignature
	ComPtr<ID3DBlob> pSignature;
	ComPtr<ID3DBlob> pError;
	hr = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&pSignature,
		&pError
	);
	if (FAILED(hr))
	{
		Log::Write(LogLevel::Error, reinterpret_cast<const char*>(pError->GetBufferPointer()));
	}
	// Create RootSignature
	hr = device->CreateRootSignature(
		0,
		pSignature->GetBufferPointer(),
		pSignature->GetBufferSize(),
		IID_PPV_ARGS(&m_ScreenCopyPSO.rootSignature)
	);
	Log::Write(LogLevel::Assert, "Root signature created.", hr);
	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[5] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].SemanticName = "TEXCOORD";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[2].SemanticName = "NORMAL";
	inputElementDesc[2].SemanticIndex = 0;
	inputElementDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[3].SemanticName = "COLOR";
	inputElementDesc[3].SemanticIndex = 0;
	inputElementDesc[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[4].SemanticName = "VERTEXID";
	inputElementDesc[4].SemanticIndex = 0;
	inputElementDesc[4].Format = DXGI_FORMAT_R32_UINT;
	inputElementDesc[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = _countof(inputElementDesc);

	// BlendState
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を無効化
	depthStencilDesc.DepthEnable = false;

	// Create PSO
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = m_ScreenCopyPSO.rootSignature.Get();
	psoDesc.VS = { pVSBlob->GetBufferPointer(),pVSBlob->GetBufferSize() };
	psoDesc.PS = { pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize() };
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = PixelFormat;
	// Create PSO
	hr = device->CreateGraphicsPipelineState(
		&psoDesc,
		IID_PPV_ARGS(&m_ScreenCopyPSO.pso)
	);
	Log::Write(LogLevel::Assert, "Pipeline state created.", hr);
}

void PipelineManager::CreatePipelineIntegrate(ID3D12Device8* device)
{
	HRESULT hr = {};
	// Compile shaders
	ComPtr<IDxcBlob> pVSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/IntegrationDraw/IntegrationDraw.VS.hlsl",
			L"vs_6_5"
		);
	//ComPtr<ID3D12ShaderReflection> pVSReflection = m_pShaderCompiler->ReflectShader(pVSBlob.Get());

	ComPtr<IDxcBlob> pPSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/IntegrationDraw/IntegrationDraw.PS.hlsl",
			L"ps_6_5"
		);
	//ComPtr<ID3D12ShaderReflection> pPSReflection = m_pShaderCompiler->ReflectShader(pPSBlob.Get());
	// CreateRootSignature
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// ジェネリックに作成
	D3D12_ROOT_PARAMETER rootParameters[8] = {};

	// ViewProjection
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	// IntegrationTransform
	D3D12_DESCRIPTOR_RANGE integrationTransformRange = {};
	integrationTransformRange.BaseShaderRegister = 0;//t0
	integrationTransformRange.RegisterSpace = 0;// space0
	integrationTransformRange.NumDescriptors = 1;
	integrationTransformRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	integrationTransformRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].DescriptorTable.pDescriptorRanges = &integrationTransformRange;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	// UseTransformList
	D3D12_DESCRIPTOR_RANGE useTransformListRange = {};
	useTransformListRange.BaseShaderRegister = 1;//t1
	useTransformListRange.RegisterSpace = 0;// space0
	useTransformListRange.NumDescriptors = 1;
	useTransformListRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	useTransformListRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[2].DescriptorTable.pDescriptorRanges = &useTransformListRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	// Lights
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 1;
	// Environment
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[4].Descriptor.ShaderRegister = 2;
	// IntegrationTransform
	D3D12_DESCRIPTOR_RANGE integrationTransformRange2 = {};
	integrationTransformRange2.BaseShaderRegister = 0;//t0
	integrationTransformRange2.RegisterSpace = 1;// space1
	integrationTransformRange2.NumDescriptors = 1;
	integrationTransformRange2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	integrationTransformRange2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[5].DescriptorTable.pDescriptorRanges = &integrationTransformRange2;
	rootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	// IntegrationMaterial
	D3D12_DESCRIPTOR_RANGE integrationMaterialRange = {};
	integrationMaterialRange.BaseShaderRegister = 1;//t1
	integrationMaterialRange.RegisterSpace = 1;// space1
	integrationMaterialRange.NumDescriptors = 1;
	integrationMaterialRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	integrationMaterialRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[6].DescriptorTable.pDescriptorRanges = &integrationMaterialRange;
	rootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	// Texture
	D3D12_DESCRIPTOR_RANGE textureRange = {};
	textureRange.BaseShaderRegister = 2;//t2
	textureRange.RegisterSpace = 0;// space0
	textureRange.NumDescriptors = 256;
	textureRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	textureRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[7].DescriptorTable.pDescriptorRanges = &textureRange;
	rootParameters[7].DescriptorTable.NumDescriptorRanges = 1;

	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.NumParameters = _countof(rootParameters);

	//// RootParameters
	//std::vector< D3D12_ROOT_PARAMETER>rootParameters;
	//std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;
	//std::vector<std::pair<uint32_t, std::string>> parameterPair;
	//// Vertex Buffer
	//parameterPair = CreateRootParameters(pVSReflection.Get(), rootParameters, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);
	//for (std::pair<uint32_t, std::string> rootParam : parameterPair)
	//{
	//	m_IntegratePSO.rootParameters.push_back(rootParam);
	//}
	//// Pixel Shader
	//parameterPair = CreateRootParameters(pPSReflection.Get(), rootParameters, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);
	//uint32_t offset = static_cast<uint32_t>(parameterPair.size());
	//for (std::pair<uint32_t, std::string> rootParam : parameterPair)
	//{
	//	rootParam.first += offset;
	//	m_IntegratePSO.rootParameters.push_back(rootParam);
	//}
	//for (int32_t i = 0; i < rootParameters.size(); ++i)
	//{
	//	rootParameters[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	//}
	//rootSignatureDesc.pParameters = rootParameters.data();
	//rootSignatureDesc.NumParameters = static_cast<UINT>(rootParameters.size());
	// Static Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	rootSignatureDesc.pStaticSamplers = staticSamplers;
	rootSignatureDesc.NumStaticSamplers = _countof(staticSamplers);
	// Serialize RootSignature
	ComPtr<ID3DBlob> pSignature;
	ComPtr<ID3DBlob> pError;
	hr = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&pSignature,
		&pError
	);
	if (FAILED(hr))
	{
		Log::Write(LogLevel::Error, reinterpret_cast<const char*>(pError->GetBufferPointer()));
	}
	// Create RootSignature
	hr = device->CreateRootSignature(
		0,
		pSignature->GetBufferPointer(),
		pSignature->GetBufferSize(),
		IID_PPV_ARGS(&m_IntegratePSO.rootSignature)
	);
	Log::Write(LogLevel::Assert, "Root signature created.", hr);
	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[5] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].SemanticName = "TEXCOORD";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[2].SemanticName = "NORMAL";
	inputElementDesc[2].SemanticIndex = 0;
	inputElementDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[3].SemanticName = "COLOR";
	inputElementDesc[3].SemanticIndex = 0;
	inputElementDesc[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[4].SemanticName = "VERTEXID";
	inputElementDesc[4].SemanticIndex = 0;
	inputElementDesc[4].Format = DXGI_FORMAT_R32_UINT;
	inputElementDesc[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = _countof(inputElementDesc);

	// BlendState
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilState
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// Create PSO
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = m_IntegratePSO.rootSignature.Get();
	psoDesc.VS = { pVSBlob->GetBufferPointer(),pVSBlob->GetBufferSize() };
	psoDesc.PS = { pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize() };
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = PixelFormat;
	// Create PSO
	hr = device->CreateGraphicsPipelineState(
		&psoDesc,
		IID_PPV_ARGS(&m_IntegratePSO.pso)
	);
	Log::Write(LogLevel::Assert, "Pipeline state created.", hr);
}

void PipelineManager::CreatePipelineLine(ID3D12Device8* device)
{
	HRESULT hr = {};
	// Compile shaders
	ComPtr<IDxcBlob> pVSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/GameLine/Line.VS.hlsl",
			L"vs_6_5"
		);
	ComPtr<ID3D12ShaderReflection> pVSReflection = m_pShaderCompiler->ReflectShader(pVSBlob.Get());

	ComPtr<IDxcBlob> pPSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/GameLine/Line.PS.hlsl",
			L"ps_6_5"
		);
	ComPtr<ID3D12ShaderReflection> pPSReflection = m_pShaderCompiler->ReflectShader(pPSBlob.Get());
	// CreateRootSignature
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// RootParameters
	std::vector< D3D12_ROOT_PARAMETER>rootParameters;
	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;
	std::vector<std::pair<uint32_t, std::string>> parameterPair;
	// Vertex Buffer
	parameterPair = CreateRootParameters(pVSReflection.Get(), rootParameters, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);
	for (std::pair<uint32_t, std::string> rootParam : parameterPair)
	{
		m_LinePSO.rootParameters.push_back(rootParam);
	}
	// Pixel Shader
	parameterPair = CreateRootParameters(pPSReflection.Get(), rootParameters, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);
	uint32_t offset = static_cast<uint32_t>(parameterPair.size());
	for (std::pair<uint32_t, std::string> rootParam : parameterPair)
	{
		rootParam.first += offset;
		m_LinePSO.rootParameters.push_back(rootParam);
	}
	for (int32_t i = 0; i < rootParameters.size(); ++i)
	{
		rootParameters[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}
	rootSignatureDesc.pParameters = rootParameters.data();
	rootSignatureDesc.NumParameters = static_cast<UINT>(rootParameters.size());
	
	// Serialize RootSignature
	ComPtr<ID3DBlob> pSignature;
	ComPtr<ID3DBlob> pError;
	hr = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&pSignature,
		&pError
	);
	if (FAILED(hr))
	{
		Log::Write(LogLevel::Info, reinterpret_cast<const char*>(pError->GetBufferPointer()));
	}
	// Create RootSignature
	hr = device->CreateRootSignature(
		0,
		pSignature->GetBufferPointer(),
		pSignature->GetBufferSize(),
		IID_PPV_ARGS(&m_LinePSO.rootSignature)
	);
	Log::Write(LogLevel::Assert, "Root signature created.", hr);
	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[2] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].SemanticName = "COLOR";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = _countof(inputElementDesc);

	// BlendState
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilState
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// Create PSO
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = m_LinePSO.rootSignature.Get();
	psoDesc.VS = { pVSBlob->GetBufferPointer(),pVSBlob->GetBufferSize() };
	psoDesc.PS = { pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize() };
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = PixelFormat;
	// Create PSO
	hr = device->CreateGraphicsPipelineState(
		&psoDesc,
		IID_PPV_ARGS(&m_LinePSO.pso)
	);
	Log::Write(LogLevel::Assert, "Pipeline state created.", hr);
}

void PipelineManager::CreatePipelineParticle(ID3D12Device8* device)
{
	HRESULT hr = {};
	// Compile shaders
	ComPtr<IDxcBlob> pVSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/Particle/Particle.VS.hlsl",
			L"vs_6_5"
		);
	//ComPtr<ID3D12ShaderReflection> pVSReflection = m_pShaderCompiler->ReflectShader(pVSBlob.Get());

	ComPtr<IDxcBlob> pPSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/Particle/Particle.PS.hlsl",
			L"ps_6_5"
		);
	//ComPtr<ID3D12ShaderReflection> pPSReflection = m_pShaderCompiler->ReflectShader(pPSBlob.Get());
	// CreateRootSignature
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// ジェネリックに作成
	D3D12_ROOT_PARAMETER rootParameters[3] = {};

	// ViewProjection
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	// Particle
	D3D12_DESCRIPTOR_RANGE ParticleRange = {};
	ParticleRange.BaseShaderRegister = 0;
	ParticleRange.NumDescriptors = 1;
	ParticleRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	ParticleRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].DescriptorTable.pDescriptorRanges = &ParticleRange;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	// Texture
	D3D12_DESCRIPTOR_RANGE textureRange = {};
	textureRange.BaseShaderRegister = 0;
	textureRange.NumDescriptors = 1;
	textureRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	textureRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = &textureRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;

	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.NumParameters = _countof(rootParameters);

	// Static Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	rootSignatureDesc.pStaticSamplers = staticSamplers;
	rootSignatureDesc.NumStaticSamplers = _countof(staticSamplers);
	// Serialize RootSignature
	ComPtr<ID3DBlob> pSignature;
	ComPtr<ID3DBlob> pError;
	hr = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&pSignature,
		&pError
	);
	if (FAILED(hr))
	{
		Log::Write(LogLevel::Error, reinterpret_cast<const char*>(pError->GetBufferPointer()));
	}
	// Create RootSignature
	hr = device->CreateRootSignature(
		0,
		pSignature->GetBufferPointer(),
		pSignature->GetBufferSize(),
		IID_PPV_ARGS(&m_ParticlePSO.rootSignature)
	);
	Log::Write(LogLevel::Assert, "Root signature created.", hr);
	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[5] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].SemanticName = "TEXCOORD";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[2].SemanticName = "NORMAL";
	inputElementDesc[2].SemanticIndex = 0;
	inputElementDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[3].SemanticName = "COLOR";
	inputElementDesc[3].SemanticIndex = 0;
	inputElementDesc[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[4].SemanticName = "VERTEXID";
	inputElementDesc[4].SemanticIndex = 0;
	inputElementDesc[4].Format = DXGI_FORMAT_R32_UINT;
	inputElementDesc[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = _countof(inputElementDesc);

	// BlendState
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = true;
	//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;

	// RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilState
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// Create PSO
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = m_ParticlePSO.rootSignature.Get();
	psoDesc.VS = { pVSBlob->GetBufferPointer(),pVSBlob->GetBufferSize() };
	psoDesc.PS = { pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize() };
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = PixelFormat;
	// Create PSO
	hr = device->CreateGraphicsPipelineState(
		&psoDesc,
		IID_PPV_ARGS(&m_ParticlePSO.pso)
	);
	Log::Write(LogLevel::Assert, "Pipeline state created.", hr);
}

void PipelineManager::CreatePipelineParticleInit(ID3D12Device8* device)
{
	HRESULT hr;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/Particle/ParticleInit.CS.hlsl",
			L"cs_6_5"
		);
	assert(computeShaderBlob);

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[3] = {};

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange = {};
	descriptorUAVRange.BaseShaderRegister = 0;
	descriptorUAVRange.NumDescriptors = 1;
	descriptorUAVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// UAV:Particle
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange1 = {};
	descriptorUAVRange1.BaseShaderRegister = 1;
	descriptorUAVRange1.NumDescriptors = 1;
	descriptorUAVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// UAV:FreeList
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &descriptorUAVRange1;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// Counter
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].Descriptor.ShaderRegister = 2;

	//D3D12_DESCRIPTOR_RANGE descriptorUAVRange2 = {};
	//descriptorUAVRange2.BaseShaderRegister = 2;
	//descriptorUAVRange2.NumDescriptors = 1;
	//descriptorUAVRange2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	//descriptorUAVRange2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorUAVRange2;// Tableの中身の配列を指定
	//rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	assert(SUCCEEDED(hr));
	// バイナリをもとに生成
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_ParticleInitPSO.rootSignature));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };
	computePipelineStateDesc.pRootSignature = m_ParticleInitPSO.rootSignature.Get();
	// 実際に生成
	hr = device->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&m_ParticleInitPSO.pso));
	assert(SUCCEEDED(hr));
}

void PipelineManager::CreatePipelineParticleUpdate(ID3D12Device8* device)
{
	HRESULT hr;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/Particle/ParticleUpdate.CS.hlsl",
			L"cs_6_5"
		);
	assert(computeShaderBlob);

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[3] = {};

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange = {};
	descriptorUAVRange.BaseShaderRegister = 0;
	descriptorUAVRange.NumDescriptors = 1;
	descriptorUAVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// UAV:Particle
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数
	// CBV:PerFrame
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// PixelShaderを使う
	rootParameter[1].Descriptor.ShaderRegister = 0;// レジスタ番号0とバインド

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange1 = {};
	descriptorUAVRange1.BaseShaderRegister = 1;
	descriptorUAVRange1.NumDescriptors = 1;
	descriptorUAVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// UAV:FreeList
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorUAVRange1;// Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	//D3D12_DESCRIPTOR_RANGE descriptorUAVRange2 = {};
	//descriptorUAVRange2.BaseShaderRegister = 2;
	//descriptorUAVRange2.NumDescriptors = 1;
	//descriptorUAVRange2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	//descriptorUAVRange2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//rootParameter[3].DescriptorTable.pDescriptorRanges = &descriptorUAVRange2;// Tableの中身の配列を指定
	//rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		assert(SUCCEEDED(hr));
	

	// バイナリをもとに生成
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_ParticleUpdatePSO.rootSignature));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };
	computePipelineStateDesc.pRootSignature = m_ParticleUpdatePSO.rootSignature.Get();
	// 実際に生成
	hr = device->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&m_ParticleUpdatePSO.pso));
	assert(SUCCEEDED(hr));
}

void PipelineManager::CreatePipelineParticleEmit(ID3D12Device8* device)
{
	HRESULT hr;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/Particle/ParticleEmit.CS.hlsl",
			L"cs_6_5"
		);
	assert(computeShaderBlob);

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[4] = {};

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange = {};
	descriptorUAVRange.BaseShaderRegister = 0;
	descriptorUAVRange.NumDescriptors = 1;
	descriptorUAVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// UAV:Particle
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数
	// CBV:Emitter
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// PixelShaderを使う
	rootParameter[1].Descriptor.ShaderRegister = 0;// レジスタ番号0とバインド
	// CBV:PerFrame
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// PixelShaderを使う
	rootParameter[2].Descriptor.ShaderRegister = 1;// レジスタ番号1とバインド

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange1 = {};
	descriptorUAVRange1.BaseShaderRegister = 1;
	descriptorUAVRange1.NumDescriptors = 1;
	descriptorUAVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[3].DescriptorTable.pDescriptorRanges = &descriptorUAVRange1;// Tableの中身の配列を指定
	rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	//D3D12_DESCRIPTOR_RANGE descriptorUAVRange2 = {};
	//descriptorUAVRange2.BaseShaderRegister = 2;
	//descriptorUAVRange2.NumDescriptors = 1;
	//descriptorUAVRange2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	//descriptorUAVRange2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//rootParameter[4].DescriptorTable.pDescriptorRanges = &descriptorUAVRange2;// Tableの中身の配列を指定
	//rootParameter[4].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		assert(SUCCEEDED(hr));

	// バイナリをもとに生成
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_ParticleEmitPSO.rootSignature));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };
	computePipelineStateDesc.pRootSignature = m_ParticleEmitPSO.rootSignature.Get();
	// 実際に生成
	hr = device->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&m_ParticleEmitPSO.pso));
	assert(SUCCEEDED(hr));
}

void PipelineManager::CreatePipelineEffectEditor(ID3D12Device8* device)
{
	HRESULT hr = {};
	// Compile shaders
	ComPtr<IDxcBlob> pVSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/EffectEditor/EffectEditor.VS.hlsl",
			L"vs_6_5"
		);
	//ComPtr<ID3D12ShaderReflection> pVSReflection = m_pShaderCompiler->ReflectShader(pVSBlob.Get());

	ComPtr<IDxcBlob> pPSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/EffectEditor/EffectEditor.PS.hlsl",
			L"ps_6_5"
		);
	//ComPtr<ID3D12ShaderReflection> pPSReflection = m_pShaderCompiler->ReflectShader(pPSBlob.Get());
	// CreateRootSignature
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// ジェネリックに作成
	D3D12_ROOT_PARAMETER rootParameters[8] = {};

	// ViewProjection
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	// EffectRoot
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 1;
	// EffectNode
	D3D12_DESCRIPTOR_RANGE NodeRange = {};
	NodeRange.BaseShaderRegister = 0;
	NodeRange.NumDescriptors = 1;
	NodeRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	NodeRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[2].DescriptorTable.pDescriptorRanges = &NodeRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	// EffectMeshData
	D3D12_DESCRIPTOR_RANGE MeshDataRange = {};
	MeshDataRange.BaseShaderRegister = 1;
	MeshDataRange.NumDescriptors = 1;
	MeshDataRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	MeshDataRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[3].DescriptorTable.pDescriptorRanges = &MeshDataRange;
	rootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	// EffectParticle
	D3D12_DESCRIPTOR_RANGE ParticleRange = {};
	ParticleRange.BaseShaderRegister = 0;
	ParticleRange.NumDescriptors = 1;
	ParticleRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	ParticleRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[4].DescriptorTable.pDescriptorRanges = &ParticleRange;
	rootParameters[4].DescriptorTable.NumDescriptorRanges = 1;

	// EffectNode
	D3D12_DESCRIPTOR_RANGE NodeRange1 = {};
	NodeRange1.BaseShaderRegister = 0;
	NodeRange1.NumDescriptors = 1;
	NodeRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	NodeRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[5].DescriptorTable.pDescriptorRanges = &NodeRange1;
	rootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	// EffectParticle
	D3D12_DESCRIPTOR_RANGE ParticleRange1 = {};
	ParticleRange1.BaseShaderRegister = 0;
	ParticleRange1.NumDescriptors = 1;
	ParticleRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	ParticleRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[6].DescriptorTable.pDescriptorRanges = &ParticleRange1;
	rootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	// Texture
	D3D12_DESCRIPTOR_RANGE textureRange = {};
	textureRange.BaseShaderRegister = 1;
	textureRange.NumDescriptors = 256;
	textureRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	textureRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[7].DescriptorTable.pDescriptorRanges = &textureRange;
	rootParameters[7].DescriptorTable.NumDescriptorRanges = 1;

	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.NumParameters = _countof(rootParameters);

	// Static Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	rootSignatureDesc.pStaticSamplers = staticSamplers;
	rootSignatureDesc.NumStaticSamplers = _countof(staticSamplers);
	// Serialize RootSignature
	ComPtr<ID3DBlob> pSignature;
	ComPtr<ID3DBlob> pError;
	hr = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&pSignature,
		&pError
	);
	if (FAILED(hr))
	{
		Log::Write(LogLevel::Error, reinterpret_cast<const char*>(pError->GetBufferPointer()));
	}
	// Create RootSignature
	hr = device->CreateRootSignature(
		0,
		pSignature->GetBufferPointer(),
		pSignature->GetBufferSize(),
		IID_PPV_ARGS(&m_EffectEditorPSO.rootSignature)
	);
	Log::Write(LogLevel::Assert, "Root signature created.", hr);
	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[5] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].SemanticName = "TEXCOORD";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[2].SemanticName = "NORMAL";
	inputElementDesc[2].SemanticIndex = 0;
	inputElementDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[3].SemanticName = "COLOR";
	inputElementDesc[3].SemanticIndex = 0;
	inputElementDesc[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[4].SemanticName = "VERTEXID";
	inputElementDesc[4].SemanticIndex = 0;
	inputElementDesc[4].Format = DXGI_FORMAT_R32_UINT;
	inputElementDesc[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = _countof(inputElementDesc);

	// BlendState
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = true;
	////blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	//blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	////blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	////!< 加算。Src * SrcA + Dest * 1
	//blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	////blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	//blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
	//blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;

	/*blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;*/

	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	// RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilState
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// Create PSO
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = m_EffectEditorPSO.rootSignature.Get();
	psoDesc.VS = { pVSBlob->GetBufferPointer(),pVSBlob->GetBufferSize() };
	psoDesc.PS = { pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize() };
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = PixelFormat;
	// Create PSO
	hr = device->CreateGraphicsPipelineState(
		&psoDesc,
		IID_PPV_ARGS(&m_EffectEditorPSO.pso)
	);
	Log::Write(LogLevel::Assert, "Pipeline state created.", hr);
}

void PipelineManager::CreatePipelineEffectEditorInit(ID3D12Device8* device)
{
	HRESULT hr;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/EffectEditor/EffectEditorInit.CS.hlsl",
			L"cs_6_5"
		);
	assert(computeShaderBlob);

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[3] = {};

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange = {};
	descriptorUAVRange.BaseShaderRegister = 0;
	descriptorUAVRange.NumDescriptors = 1;
	descriptorUAVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// UAV:Particle
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange1 = {};
	descriptorUAVRange1.BaseShaderRegister = 1;
	descriptorUAVRange1.NumDescriptors = 1;
	descriptorUAVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// UAV:FreeList
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &descriptorUAVRange1;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// Counter
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].Descriptor.ShaderRegister = 2;

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	assert(SUCCEEDED(hr));
	// バイナリをもとに生成
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_EffectEditorInitPSO.rootSignature));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };
	computePipelineStateDesc.pRootSignature = m_EffectEditorInitPSO.rootSignature.Get();
	// 実際に生成
	hr = device->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&m_EffectEditorInitPSO.pso));
	assert(SUCCEEDED(hr));
}

void PipelineManager::CreatePipelineEffectEditorUpdate(ID3D12Device8* device)
{
	HRESULT hr;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/EffectEditor/EffectEditorUpdate.CS.hlsl",
			L"cs_6_5"
		);
	assert(computeShaderBlob);

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[5] = {};

	// CBV:EffectRoot
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// ComputeShaderを使う
	rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号0とバインド
	// SRV:EffectNode
	D3D12_DESCRIPTOR_RANGE descriptorSRVRange = {};
	descriptorSRVRange.BaseShaderRegister = 0;
	descriptorSRVRange.NumDescriptors = 1;
	descriptorSRVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorSRVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &descriptorSRVRange;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数
	// SRV:EffectMeshData
	D3D12_DESCRIPTOR_RANGE descriptorSRVRange1 = {};
	descriptorSRVRange1.BaseShaderRegister = 1;
	descriptorSRVRange1.NumDescriptors = 1;
	descriptorSRVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorSRVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorSRVRange1;// Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数
	// UAV:EffectParticle
	D3D12_DESCRIPTOR_RANGE descriptorUAVRange = {};
	descriptorUAVRange.BaseShaderRegister = 0;
	descriptorUAVRange.NumDescriptors = 1;
	descriptorUAVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[3].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数
	// UAV:EffectParticleFreeList
	D3D12_DESCRIPTOR_RANGE descriptorUAVRange1 = {};
	descriptorUAVRange1.BaseShaderRegister = 1;
	descriptorUAVRange1.NumDescriptors = 1;
	descriptorUAVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[4].DescriptorTable.pDescriptorRanges = &descriptorUAVRange1;// Tableの中身の配列を指定
	rootParameter[4].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	assert(SUCCEEDED(hr));


	// バイナリをもとに生成
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_EffectEditorUpdatePSO.rootSignature));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };
	computePipelineStateDesc.pRootSignature = m_EffectEditorUpdatePSO.rootSignature.Get();
	// 実際に生成
	hr = device->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&m_EffectEditorUpdatePSO.pso));
	assert(SUCCEEDED(hr));
}

void PipelineManager::CreatePipelineEffectEditorEmit(ID3D12Device8* device)
{
	HRESULT hr;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/EffectEditor/EffectEditorEmit.CS.hlsl",
			L"cs_6_5"
		);
	assert(computeShaderBlob);

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[6] = {};

	// CBV:EffectRoot
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// ComputeShaderを使う
	rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号0とバインド
	// SRV:EffectNode
	D3D12_DESCRIPTOR_RANGE descriptorSRVRange = {};
	descriptorSRVRange.BaseShaderRegister = 0;
	descriptorSRVRange.NumDescriptors = 1;
	descriptorSRVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorSRVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &descriptorSRVRange;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数
	// SRV:EffectMeshData
	D3D12_DESCRIPTOR_RANGE descriptorSRVRange1 = {};
	descriptorSRVRange1.BaseShaderRegister = 1;
	descriptorSRVRange1.NumDescriptors = 1;
	descriptorSRVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorSRVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorSRVRange1;// Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数
	// UAV:EffectParticle
	D3D12_DESCRIPTOR_RANGE descriptorUAVRange = {};
	descriptorUAVRange.BaseShaderRegister = 0;
	descriptorUAVRange.NumDescriptors = 1;
	descriptorUAVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[3].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数
	// UAV:EffectParticleFreeList
	D3D12_DESCRIPTOR_RANGE descriptorUAVRange1 = {};
	descriptorUAVRange1.BaseShaderRegister = 1;
	descriptorUAVRange1.NumDescriptors = 1;
	descriptorUAVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[4].DescriptorTable.pDescriptorRanges = &descriptorUAVRange1;// Tableの中身の配列を指定
	rootParameter[4].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数
	// UAV:EffectParticleFreeListCounter
	rootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
	rootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[5].Descriptor.ShaderRegister = 2;// レジスタ番号2とバインド

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	assert(SUCCEEDED(hr));

	// バイナリをもとに生成
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_EffectEditorEmitPSO.rootSignature));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };
	computePipelineStateDesc.pRootSignature = m_EffectEditorEmitPSO.rootSignature.Get();
	// 実際に生成
	hr = device->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&m_EffectEditorEmitPSO.pso));
	assert(SUCCEEDED(hr));
}

void PipelineManager::CreatePipelineUI(ID3D12Device8* device)
{
	HRESULT hr = {};
	// Compile shaders
	ComPtr<IDxcBlob> pVSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/UIStandard/UIStandard.VS.hlsl",
			L"vs_6_5"
		);
	//ComPtr<ID3D12ShaderReflection> pVSReflection = m_pShaderCompiler->ReflectShader(pVSBlob.Get());

	ComPtr<IDxcBlob> pPSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/UIStandard/UIStandard.PS.hlsl",
			L"ps_6_5"
		);
	//ComPtr<ID3D12ShaderReflection> pPSReflection = m_pShaderCompiler->ReflectShader(pPSBlob.Get());
	// CreateRootSignature
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// ジェネリックに作成
	D3D12_ROOT_PARAMETER rootParameters[4] = {};

	// World
	D3D12_DESCRIPTOR_RANGE uiTransformRange = {};
	uiTransformRange.BaseShaderRegister = 0;
	uiTransformRange.NumDescriptors = 1;
	uiTransformRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	uiTransformRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[0].DescriptorTable.pDescriptorRanges = &uiTransformRange;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
	// UseList
	D3D12_DESCRIPTOR_RANGE useListRange = {};
	useListRange.BaseShaderRegister = 1;
	useListRange.NumDescriptors = 1;
	useListRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	useListRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].DescriptorTable.pDescriptorRanges = &useListRange;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	// IntegrationMaterial
	D3D12_DESCRIPTOR_RANGE integrationMaterialRange = {};
	integrationMaterialRange.BaseShaderRegister = 0;
	integrationMaterialRange.NumDescriptors = 1;
	integrationMaterialRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	integrationMaterialRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = &integrationMaterialRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	// Texture
	D3D12_DESCRIPTOR_RANGE textureRange = {};
	textureRange.BaseShaderRegister = 1;
	textureRange.NumDescriptors = 256;
	textureRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	textureRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].DescriptorTable.pDescriptorRanges = &textureRange;
	rootParameters[3].DescriptorTable.NumDescriptorRanges = 1;

	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.NumParameters = _countof(rootParameters);

	// Static Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	rootSignatureDesc.pStaticSamplers = staticSamplers;
	rootSignatureDesc.NumStaticSamplers = _countof(staticSamplers);
	// Serialize RootSignature
	ComPtr<ID3DBlob> pSignature;
	ComPtr<ID3DBlob> pError;
	hr = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&pSignature,
		&pError
	);
	if (FAILED(hr))
	{
		Log::Write(LogLevel::Error, reinterpret_cast<const char*>(pError->GetBufferPointer()));
	}
	// Create RootSignature
	hr = device->CreateRootSignature(
		0,
		pSignature->GetBufferPointer(),
		pSignature->GetBufferSize(),
		IID_PPV_ARGS(&m_UIPSO.rootSignature)
	);
	Log::Write(LogLevel::Assert, "Root signature created.", hr);
	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[4] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].SemanticName = "TEXCOORD";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[2].SemanticName = "COLOR";
	inputElementDesc[2].SemanticIndex = 0;
	inputElementDesc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[3].SemanticName = "VERTEXID";
	inputElementDesc[3].SemanticIndex = 0;
	inputElementDesc[3].Format = DXGI_FORMAT_R32_UINT;
	inputElementDesc[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = _countof(inputElementDesc);

	// BlendState
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilState
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;
	//// 書き込みをします
	//depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//// 比較関数はLessEqual。つまり、近ければ描画される
	//depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// Create PSO
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = m_UIPSO.rootSignature.Get();
	psoDesc.VS = { pVSBlob->GetBufferPointer(),pVSBlob->GetBufferSize() };
	psoDesc.PS = { pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize() };
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = PixelFormat;
	// Create PSO
	hr = device->CreateGraphicsPipelineState(
		&psoDesc,
		IID_PPV_ARGS(&m_UIPSO.pso)
	);
	Log::Write(LogLevel::Assert, "Pipeline state created.", hr);
}

void PipelineManager::CreatePipelineSkinningCS(ID3D12Device8* device)
{
	HRESULT hr;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/EngineAssets/Shader/Skinning/Skinning.CS.hlsl",
			L"cs_6_5"
		);
	assert(computeShaderBlob);

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[5] = {};

	D3D12_DESCRIPTOR_RANGE paletteRange = {};
	paletteRange.BaseShaderRegister = 0;
	paletteRange.NumDescriptors = 1;
	paletteRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	paletteRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// SRV:Palette
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &paletteRange;// Tableの中身の配列を指定
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE inputVertexRange = {};
	inputVertexRange.BaseShaderRegister = 1;
	inputVertexRange.NumDescriptors = 1;
	inputVertexRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	inputVertexRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// SRV:InputVertex
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &inputVertexRange;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE influenceRange = {};
	influenceRange.BaseShaderRegister = 2;
	influenceRange.NumDescriptors = 1;
	influenceRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	influenceRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// SRV:Influence
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = &influenceRange;// Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE outputVertexRange = {};
	outputVertexRange.BaseShaderRegister = 0;
	outputVertexRange.NumDescriptors = 1;
	outputVertexRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	outputVertexRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// UAV:OutputVertex
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[3].DescriptorTable.pDescriptorRanges = &outputVertexRange;// Tableの中身の配列を指定
	rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE skinningInfoRange = {};
	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// ComputeShaderを使う
	rootParameter[4].Descriptor.ShaderRegister = 0;// レジスタ番号0とバインド

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	assert(SUCCEEDED(hr));
	// バイナリをもとに生成
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_Skinning.rootSignature));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };
	computePipelineStateDesc.pRootSignature = m_Skinning.rootSignature.Get();
	// 実際に生成
	hr = device->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&m_Skinning.pso));
	assert(SUCCEEDED(hr));
}
