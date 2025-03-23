#include "pch.h"
#include "PipelineManager.h"

void PipelineManager::Initialize(ID3D12Device8* device)
{
	m_pShaderCompiler->Initialize(device);
	CreatePipelineDemo(device);
	CreatePipelineScreenCopy(device);
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
			L"Cho/Resources/Shader/Demo/Demo.VS.hlsl",
			L"vs_6_5"
			);
	ChoAssertLog("Failed to compile vertex shader", pVSBlob.Get(), __FILE__, __LINE__);
	ComPtr<ID3D12ShaderReflection> pVSReflection = m_pShaderCompiler->ReflectShader(pVSBlob.Get());

	ComPtr<IDxcBlob> pPSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/Shader/Demo/Demo.PS.hlsl",
			L"ps_6_5"
		);
	ChoAssertLog("Failed to compile pixel shader", pPSBlob.Get(), __FILE__, __LINE__);
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
		ChoLog(reinterpret_cast<const char*>(pError->GetBufferPointer()));
	}
	ChoAssertLog("Failed to serialize root signature", hr, __FILE__, __LINE__);
	// Create RootSignature
	hr = device->CreateRootSignature(
		0,
		pSignature->GetBufferPointer(),
		pSignature->GetBufferSize(),
		IID_PPV_ARGS(&m_DemoPSO.rootSignature)
	);
	ChoAssertLog("Failed to create root signature", hr, __FILE__, __LINE__);
	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[3] = {};
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
	ChoAssertLog("Failed to create pipeline state", hr, __FILE__, __LINE__);
}

void PipelineManager::CreatePipelineScreenCopy(ID3D12Device8* device)
{
	HRESULT hr = {};
	// Compile shaders
	ComPtr<IDxcBlob> pVSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/Shader/ScreenCopy/ScreenCopy.VS.hlsl",
			L"vs_6_5"
		);
	ChoAssertLog("Failed to compile vertex shader", pVSBlob.Get(), __FILE__, __LINE__);
	ComPtr<ID3D12ShaderReflection> pVSReflection = m_pShaderCompiler->ReflectShader(pVSBlob.Get());

	ComPtr<IDxcBlob> pPSBlob =
		m_pShaderCompiler->CompileShader(
			L"Cho/Resources/Shader/ScreenCopy/ScreenCopy.PS.hlsl",
			L"ps_6_5"
		);
	ChoAssertLog("Failed to compile pixel shader", pPSBlob.Get(), __FILE__, __LINE__);
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
		ChoLog(reinterpret_cast<const char*>(pError->GetBufferPointer()));
	}
	ChoAssertLog("Failed to serialize root signature", hr, __FILE__, __LINE__);
	// Create RootSignature
	hr = device->CreateRootSignature(
		0,
		pSignature->GetBufferPointer(),
		pSignature->GetBufferSize(),
		IID_PPV_ARGS(&m_ScreenCopyPSO.rootSignature)
	);
	ChoAssertLog("Failed to create root signature", hr, __FILE__, __LINE__);
	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[3] = {};
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
	ChoAssertLog("Failed to create pipeline state", hr, __FILE__, __LINE__);
}
