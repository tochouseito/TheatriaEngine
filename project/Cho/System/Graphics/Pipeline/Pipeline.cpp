#include "PrecompiledHeader.h"
#pragma warning(push)
#pragma warning(disable : 4324)
#include "Pipeline.h"
#include"D3D12/ShaderCompiler/ShaderCompiler.h"
#include"D3D12/D3DDevice/D3DDevice.h"
#include<assert.h>
#include"ConvertString/ConvertString.h"
#include "Base/Format.h"
#include <windows.h>
#include <sstream>

namespace /* anonymous */ {
	///////////////////////////////////////////////////////////////////////////////
	// StateParam structure
	///////////////////////////////////////////////////////////////////////////////
	template<typename ValueType, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE ObjectType>
	class alignas(void*) StateParam
	{
	public:
		StateParam()
			: Type(ObjectType)
			, Value(ValueType())
		{ /* DO_NOTHING */
		}

		StateParam(const ValueType& value)
			: Value(value)
			, Type(ObjectType)
		{ /* DO_NOTHING */
		}

		StateParam& operator = (const ValueType& value)
		{
			Type = ObjectType;
			Value = value;
			return *this;
		}

	private:
		D3D12_PIPELINE_STATE_SUBOBJECT_TYPE     Type;
		ValueType                               Value;
	};

	// 長いから省略形を作る.
#define PSST(x) D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_##x

	using SP_ROOT_SIGNATURE = StateParam<ID3D12RootSignature*, PSST(ROOT_SIGNATURE)>;
	using SP_AS = StateParam<D3D12_SHADER_BYTECODE, PSST(AS)>;
	using SP_MS = StateParam<D3D12_SHADER_BYTECODE, PSST(MS)>;
	using SP_PS = StateParam<D3D12_SHADER_BYTECODE, PSST(PS)>;
	using SP_BLEND = StateParam<D3D12_BLEND_DESC, PSST(BLEND)>;
	using SP_RASTERIZER = StateParam<D3D12_RASTERIZER_DESC, PSST(RASTERIZER)>;
	using SP_DEPTH_STENCIL = StateParam<D3D12_DEPTH_STENCIL_DESC, PSST(DEPTH_STENCIL)>;
	using SP_SAMPLE_MASK = StateParam<UINT, PSST(SAMPLE_MASK)>;
	using SP_SAMPLE_DESC = StateParam<DXGI_SAMPLE_DESC, PSST(SAMPLE_DESC)>;
	using SP_RT_FORMAT = StateParam<D3D12_RT_FORMAT_ARRAY, PSST(RENDER_TARGET_FORMATS)>;
	using SP_DS_FORMAT = StateParam<DXGI_FORMAT, PSST(DEPTH_STENCIL_FORMAT)>;
	using SP_FLAGS = StateParam<D3D12_PIPELINE_STATE_FLAGS, PSST(FLAGS)>;

	// 宣言し終わったら要らないので無効化.
#undef PSST

///////////////////////////////////////////////////////////////////////////////
// MeshShaderPipelineStateDesc structure
///////////////////////////////////////////////////////////////////////////////
	struct MeshShaderPipelineStateDesc
	{
		SP_ROOT_SIGNATURE  RootSignature;           // ルートシグニチャ.
		SP_AS              AS;                      // 増幅シェーダ.
		SP_MS              MS;                      // メッシュシェーダ.
		SP_PS              PS;                      // ピクセルシェーダ.
		SP_BLEND           Blend;                   // ブレンドステート.
		SP_RASTERIZER      Rasterizer;              // ラスタライザーステート.
		SP_DEPTH_STENCIL   DepthStencil;            // 深度ステンシルステート.
		SP_SAMPLE_MASK     SampleMask;              // サンプルマスク.
		SP_SAMPLE_DESC     SampleDesc;              // サンプル設定.
		SP_RT_FORMAT       RTFormats;               // レンダーゲットフォーマット.
		SP_DS_FORMAT       DSFormat;                // 深度ステンシルフォーマット.
		SP_FLAGS           Flags;                   // フラグです.
	};
}// namespace /* anonymous */
#pragma warning(pop)
void Pipeline::Initialize(D3DDevice* d3dDevice, ShaderCompiler* shaderCompiler)
{
	d3dDevice_ = d3dDevice;

    shaderCompiler_ = shaderCompiler;

	CreatePSODemo();
	CreatePSOScreenCopy();
	CreatePSOSprite();
	CreatePSOParticle();
	CreatePSOParticleInit();
	CreatePSOEmit();
	CreatePSOUpdate();
	CreatePSOWireframe();
	CreatePSOSkinningCS();
	CreatePSODemoMS();
	CreatePSOGBuffer();
	CreatePSOWireframeGB();
	CreatePSOGBufferMix();
	CreatePSOWireframeMix();
	CreatePSODrawLine();
	CreatePSOMapChip();
	CreatePSOEffectSprite();
	CreatePSOEffectInit();
	CreatePSOEffectEmit();
	CreatePSOEffectUpdate();
	CreatePSOMapChipGBuffer();
	CreatePSODif_ToonLighting();
}

void Pipeline::CreatePSODemo()
{
	HRESULT hr;
	PSOBlend psoBlend;
	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob =
		shaderCompiler_->CompilerShader(
			vsPath+L"DemoOBJ.VS.hlsl",
			L"vs_6_5");
	assert(vertexShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> vertexShaderReflection = nullptr;
	vertexShaderReflection = shaderCompiler_->ReflectShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		shaderCompiler_->CompilerShader(
			psPath+L"DemoOBJ.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = shaderCompiler_->ReflectShader(pixelShaderBlob.Get());

	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	std::vector< D3D12_ROOT_PARAMETER>rootParameter;

	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;

	std::vector<std::pair<uint32_t, std::string>> rootParms;

	rootParms =
		shaderCompiler_->CreateRootParameters(vertexShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		psoBlend.rootParm.push_back(rootParm);
	}

	//vertexShaderReflection->Release();

	rootParms =
		shaderCompiler_->CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		psoBlend.rootParm.push_back(rootParm);
	}

	//pixelShaderReflection->Release();

	for (int32_t i = 0; i < rootParameter.size(); ++i) {
		rootParameter[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}


	descriptionRooTSignature.pParameters = rootParameter.data();// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = static_cast<UINT>(rootParameter.size());// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = dxgiFormat;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeScreen]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOScreenCopy()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob =
		shaderCompiler_->CompilerShader(
			vsPath+L"FullScreen.VS.hlsl",
			L"vs_6_5");
	assert(vertexShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> vertexShaderReflection = nullptr;
	vertexShaderReflection = shaderCompiler_->ReflectShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		shaderCompiler_->CompilerShader(
			psPath+L"FullScreen.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = shaderCompiler_->ReflectShader(pixelShaderBlob.Get());

	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	std::vector< D3D12_ROOT_PARAMETER>rootParameter;

	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;

	std::vector<std::pair<uint32_t, std::string>> rootParms;

	rootParms =
		shaderCompiler_->CreateRootParameters(vertexShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		psoBlend.rootParm.push_back(rootParm);
	}

	//vertexShaderReflection->Release();

	rootParms =
		shaderCompiler_->CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		psoBlend.rootParm.push_back(rootParm);
	}

	//pixelShaderReflection->Release();

	for (int32_t i = 0; i < rootParameter.size(); ++i) {
		rootParameter[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}


	descriptionRooTSignature.pParameters = rootParameter.data();// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = static_cast<UINT>(rootParameter.size());// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;




	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を無効化
	depthStencilDesc.DepthEnable = false;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = dxgiFormat;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeScreen]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOSprite()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob =
		shaderCompiler_->CompilerShader(
			vsPath+L"Sprite.VS.hlsl",
			L"vs_6_5");
	assert(vertexShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> vertexShaderReflection = nullptr;
	vertexShaderReflection = shaderCompiler_->ReflectShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		shaderCompiler_->CompilerShader(
			psPath+L"Sprite.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = shaderCompiler_->ReflectShader(pixelShaderBlob.Get());

	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	std::vector< D3D12_ROOT_PARAMETER>rootParameter;

	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;

	std::vector<std::pair<uint32_t, std::string>> rootParms;

	rootParms =
		shaderCompiler_->CreateRootParameters(vertexShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		psoBlend.rootParm.push_back(rootParm);
	}

	//vertexShaderReflection->Release();

	rootParms =
		shaderCompiler_->CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		psoBlend.rootParm.push_back(rootParm);
	}

	//pixelShaderReflection->Release();

	for (int32_t i = 0; i < rootParameter.size(); ++i) {
		rootParameter[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}


	descriptionRooTSignature.pParameters = rootParameter.data();// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = static_cast<UINT>(rootParameter.size());// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = dxgiFormat;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeScreen]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOParticle()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob =
		shaderCompiler_->CompilerShader(
			vsPath+L"Particle.VS.hlsl",
			L"vs_6_5");
	assert(vertexShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> vertexShaderReflection = nullptr;
	vertexShaderReflection = shaderCompiler_->ReflectShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		shaderCompiler_->CompilerShader(
			psPath+L"Particle.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = shaderCompiler_->ReflectShader(pixelShaderBlob.Get());

	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	std::vector< D3D12_ROOT_PARAMETER>rootParameter;

	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;

	std::vector<std::pair<uint32_t, std::string>> rootParms;

	rootParms =
		shaderCompiler_->CreateRootParameters(vertexShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		psoBlend.rootParm.push_back(rootParm);
	}

	//vertexShaderReflection->Release();

	rootParms =
		shaderCompiler_->CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		psoBlend.rootParm.push_back(rootParm);
	}

	//pixelShaderReflection->Release();

	for (int32_t i = 0; i < rootParameter.size(); ++i) {
		rootParameter[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}


	descriptionRooTSignature.pParameters = rootParameter.data();// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = static_cast<UINT>(rootParameter.size());// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[4] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[3].SemanticName = "COLOR";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = dxgiFormat;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeScreen]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOParticleInit()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob =
		shaderCompiler_->CompilerShader(
			csPath+L"InitParticle.CS.hlsl",
			L"cs_6_5");
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

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange1 = {};
	descriptorUAVRange1.BaseShaderRegister = 1;
	descriptorUAVRange1.NumDescriptors = 1;
	descriptorUAVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &descriptorUAVRange1;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange2 = {};
	descriptorUAVRange2.BaseShaderRegister = 2;
	descriptorUAVRange2.NumDescriptors = 1;
	descriptorUAVRange2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorUAVRange2;// Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };
	computePipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOEmit()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob =
		shaderCompiler_->CompilerShader(
			csPath+L"EmitParticle.CS.hlsl",
			L"cs_6_5");
	assert(computeShaderBlob);

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[5] = {};

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange = {};
	descriptorUAVRange.BaseShaderRegister = 0;
	descriptorUAVRange.NumDescriptors = 1;
	descriptorUAVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// PixelShaderを使う
	rootParameter[1].Descriptor.ShaderRegister = 0;// レジスタ番号0とバインド

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

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange2 = {};
	descriptorUAVRange2.BaseShaderRegister = 2;
	descriptorUAVRange2.NumDescriptors = 1;
	descriptorUAVRange2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[4].DescriptorTable.pDescriptorRanges = &descriptorUAVRange2;// Tableの中身の配列を指定
	rootParameter[4].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };
	computePipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOUpdate()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob =
		shaderCompiler_->CompilerShader(
			csPath+L"UpdateParticle.CS.hlsl",
			L"cs_6_5");
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

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// PixelShaderを使う
	rootParameter[1].Descriptor.ShaderRegister = 0;// レジスタ番号0とバインド

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange1 = {};
	descriptorUAVRange1.BaseShaderRegister = 1;
	descriptorUAVRange1.NumDescriptors = 1;
	descriptorUAVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorUAVRange1;// Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange2 = {};
	descriptorUAVRange2.BaseShaderRegister = 2;
	descriptorUAVRange2.NumDescriptors = 1;
	descriptorUAVRange2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[3].DescriptorTable.pDescriptorRanges = &descriptorUAVRange2;// Tableの中身の配列を指定
	rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };
	computePipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOWireframe()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob =
		shaderCompiler_->CompilerShader(
			vsPath+L"Wireframe.VS.hlsl",
			L"vs_6_5");
	assert(vertexShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> vertexShaderReflection = nullptr;
	vertexShaderReflection = shaderCompiler_->ReflectShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		shaderCompiler_->CompilerShader(
			psPath+L"Wireframe.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = shaderCompiler_->ReflectShader(pixelShaderBlob.Get());

	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	std::vector< D3D12_ROOT_PARAMETER>rootParameter;

	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;

	std::vector<std::pair<uint32_t, std::string>> rootParms;

	rootParms =
		shaderCompiler_->CreateRootParameters(vertexShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		psoBlend.rootParm.push_back(rootParm);
	}

	//vertexShaderReflection->Release();

	rootParms =
		shaderCompiler_->CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		psoBlend.rootParm.push_back(rootParm);
	}

	//pixelShaderReflection->Release();

	for (int32_t i = 0; i < rootParameter.size(); ++i) {
		rootParameter[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}


	descriptionRooTSignature.pParameters = rootParameter.data();// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = static_cast<UINT>(rootParameter.size());// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// ワイヤーフレーム
	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = dxgiFormat;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOSkinningCS()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob =
		shaderCompiler_->CompilerShader(
			csPath+L"Skinning.CS.hlsl",
			L"cs_6_5");
	assert(computeShaderBlob);

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[5] = {};

	D3D12_DESCRIPTOR_RANGE descriptorSRVRange = {};
	descriptorSRVRange.BaseShaderRegister = 0;
	descriptorSRVRange.NumDescriptors = 1;
	descriptorSRVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorSRVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &descriptorSRVRange;// Tableの中身の配列を指定
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorSRVRange1 = {};
	descriptorSRVRange1.BaseShaderRegister = 1;
	descriptorSRVRange1.NumDescriptors = 1;
	descriptorSRVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorSRVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &descriptorSRVRange1;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorSRVRange2 = {};
	descriptorSRVRange2.BaseShaderRegister = 2;
	descriptorSRVRange2.NumDescriptors = 1;
	descriptorSRVRange2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorSRVRange2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorSRVRange2;// Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange = {};
	descriptorUAVRange.BaseShaderRegister = 0;
	descriptorUAVRange.NumDescriptors = 1;
	descriptorUAVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[3].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[4].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };
	computePipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSODemoMS()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> msShaderBlob =
		shaderCompiler_->CompilerShader(
			msPath+L"Basic.MS.hlsl",
			L"ms_6_5");
	assert(msShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> msShaderReflection = nullptr;
	msShaderReflection = shaderCompiler_->ReflectShader(msShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		shaderCompiler_->CompilerShader(
			psPath+L"Demo.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = shaderCompiler_->ReflectShader(pixelShaderBlob.Get());

	// RootSignature作成
	auto flag = D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags = flag;
		//D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	std::vector< D3D12_ROOT_PARAMETER>rootParameter;

	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;

	std::vector<std::pair<uint32_t, std::string>> rootParms;

	rootParms =
		shaderCompiler_->CreateRootParameters(msShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_MESH);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		psoBlend.rootParm.push_back(rootParm);
	}

	//vertexShaderReflection->Release();

	rootParms =
		shaderCompiler_->CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		psoBlend.rootParm.push_back(rootParm);
	}

	//pixelShaderReflection->Release();

	for (int32_t i = 0; i < rootParameter.size(); ++i) {
		rootParameter[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}


	descriptionRooTSignature.pParameters = rootParameter.data();// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = static_cast<UINT>(rootParameter.size());// 配列の長さ

	//D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	//staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	//staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	//staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	//staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	//staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	//descriptionRooTSignature.pStaticSamplers = staticSamplers;
	//descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(
		&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob,
		&errorBlob
	);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));


	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	DXGI_SAMPLE_DESC descSample;
	descSample.Count = 1;
	descSample.Quality = 0;

	D3D12_RT_FORMAT_ARRAY rtFormat = {};
	rtFormat.NumRenderTargets = 1;
	rtFormat.RTFormats[0] = dxgiFormat;

	ID3D12RootSignature* pRootSig = psoBlend.rootSignature.Get();

	MeshShaderPipelineStateDesc descState = {};
	descState.RootSignature = pRootSig;
	descState.MS = { msShaderBlob->GetBufferPointer(), msShaderBlob->GetBufferSize() };
	descState.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };
	descState.Rasterizer = rasterizerDesc;
	descState.Blend = blendDesc;
	descState.DepthStencil = depthStencilDesc;
	descState.SampleMask = UINT_MAX;
	descState.SampleDesc = descSample;
	descState.RTFormats = rtFormat;
	descState.DSFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	D3D12_PIPELINE_STATE_STREAM_DESC descStream = {};
	descStream.SizeInBytes = sizeof(descState);
	descStream.pPipelineStateSubobjectStream = &descState;

	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreatePipelineState(
		&descStream,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone])
	);
	assert(SUCCEEDED(hr));

	//// ここからブレンドPSOの各設定
	//blendDesc.RenderTarget[0].BlendEnable = TRUE;
	//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	//blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	////!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	//blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//meshShaderPipelineStateDesc.BlendState = blendDesc;          // BlendState
	//hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&meshShaderPipelineStateDesc,
	//	IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNormal]));
	//assert(SUCCEEDED(hr));

	////!< 加算。Src * SrcA + Dest * 1
	//blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	//meshShaderPipelineStateDesc.BlendState = blendDesc;          // BlendState
	//hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&meshShaderPipelineStateDesc,
	//	IID_PPV_ARGS(&psoBlend.Blend[kBlendModeAdd]));
	//assert(SUCCEEDED(hr));

	////!< 減算。Dest * 1 - Src * SrcA
	//blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	//blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	//meshShaderPipelineStateDesc.BlendState = blendDesc;          // BlendState
	//hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&meshShaderPipelineStateDesc,
	//	IID_PPV_ARGS(&psoBlend.Blend[kBlendModeSubtract]));
	//assert(SUCCEEDED(hr));

	////!< 乗算。Src * 0 + Dest * Src
	//blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	//blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	//meshShaderPipelineStateDesc.BlendState = blendDesc;          // BlendState
	//hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&meshShaderPipelineStateDesc,
	//	IID_PPV_ARGS(&psoBlend.Blend[kBlendModeMultily]));
	//assert(SUCCEEDED(hr));

	////!< スクリーン。Src * (1 - Dest) *Dest * 1
	//blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	//blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	//meshShaderPipelineStateDesc.BlendState = blendDesc;          // BlendState
	//hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&meshShaderPipelineStateDesc,
	//	IID_PPV_ARGS(&psoBlend.Blend[kBlendModeScreen]));
	//assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOGBuffer()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob =
		shaderCompiler_->CompilerShader(
			vsPath+L"DemoOBJ.VS.hlsl",
			L"vs_6_5");
	assert(vertexShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> vertexShaderReflection = nullptr;
	vertexShaderReflection = shaderCompiler_->ReflectShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		shaderCompiler_->CompilerShader(
			psPath+L"GBuffer.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = shaderCompiler_->ReflectShader(pixelShaderBlob.Get());

	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	std::vector< D3D12_ROOT_PARAMETER>rootParameter;

	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;

	std::vector<std::pair<uint32_t, std::string>> rootParms;

	rootParms =
		shaderCompiler_->CreateRootParameters(vertexShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		psoBlend.rootParm.push_back(rootParm);
	}

	//vertexShaderReflection->Release();

	rootParms =
		shaderCompiler_->CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		psoBlend.rootParm.push_back(rootParm);
	}

	//pixelShaderReflection->Release();

	for (int32_t i = 0; i < rootParameter.size(); ++i) {
		rootParameter[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}


	descriptionRooTSignature.pParameters = rootParameter.data();// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = static_cast<UINT>(rootParameter.size());// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 4;// 三つ
	graphicsPipelineStateDesc.RTVFormats[0] = dxgiFormat;
	graphicsPipelineStateDesc.RTVFormats[1] = dxgiFormat;
	graphicsPipelineStateDesc.RTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	graphicsPipelineStateDesc.RTVFormats[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeScreen]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOWireframeGB()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob =
		shaderCompiler_->CompilerShader(
			vsPath+L"DemoOBJ.VS.hlsl",
			L"vs_6_5");
	assert(vertexShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> vertexShaderReflection = nullptr;
	vertexShaderReflection = shaderCompiler_->ReflectShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		shaderCompiler_->CompilerShader(
			psPath+L"GBuffer.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = shaderCompiler_->ReflectShader(pixelShaderBlob.Get());

	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	std::vector< D3D12_ROOT_PARAMETER>rootParameter;

	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;

	std::vector<std::pair<uint32_t, std::string>> rootParms;

	rootParms =
		shaderCompiler_->CreateRootParameters(vertexShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		psoBlend.rootParm.push_back(rootParm);
	}

	//vertexShaderReflection->Release();

	rootParms =
		shaderCompiler_->CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		psoBlend.rootParm.push_back(rootParm);
	}

	//pixelShaderReflection->Release();

	for (int32_t i = 0; i < rootParameter.size(); ++i) {
		rootParameter[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}


	descriptionRooTSignature.pParameters = rootParameter.data();// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = static_cast<UINT>(rootParameter.size());// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 4;// 三つ
	graphicsPipelineStateDesc.RTVFormats[0] = dxgiFormat;
	graphicsPipelineStateDesc.RTVFormats[1] = dxgiFormat;
	graphicsPipelineStateDesc.RTVFormats[2] = dxgiFormat;
	graphicsPipelineStateDesc.RTVFormats[3] = dxgiFormat;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeScreen]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOGBufferMix()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob =
		shaderCompiler_->CompilerShader(
			vsPath+L"FullScreen.VS.hlsl",
			L"vs_6_5");
	assert(vertexShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> vertexShaderReflection = nullptr;
	vertexShaderReflection = shaderCompiler_->ReflectShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		shaderCompiler_->CompilerShader(
			psPath+L"GBufferMIX.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = shaderCompiler_->ReflectShader(pixelShaderBlob.Get());

	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	std::vector< D3D12_ROOT_PARAMETER>rootParameter;

	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;

	std::vector<std::pair<uint32_t, std::string>> rootParms;

	rootParms =
		shaderCompiler_->CreateRootParameters(vertexShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		psoBlend.rootParm.push_back(rootParm);
	}

	//vertexShaderReflection->Release();

	rootParms =
		shaderCompiler_->CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		psoBlend.rootParm.push_back(rootParm);
	}

	//pixelShaderReflection->Release();

	for (int32_t i = 0; i < rootParameter.size(); ++i) {
		rootParameter[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}


	descriptionRooTSignature.pParameters = rootParameter.data();// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = static_cast<UINT>(rootParameter.size());// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	/*inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;*/

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を無効化
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	depthStencilDesc.StencilEnable = false;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = dxgiFormat;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeScreen]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOWireframeMix()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob =
		shaderCompiler_->CompilerShader(
			vsPath+L"DemoOBJ.VS.hlsl",
			L"vs_6_5");
	assert(vertexShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> vertexShaderReflection = nullptr;
	vertexShaderReflection = shaderCompiler_->ReflectShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		shaderCompiler_->CompilerShader(
			psPath+L"GBufferMIX.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = shaderCompiler_->ReflectShader(pixelShaderBlob.Get());

	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	std::vector< D3D12_ROOT_PARAMETER>rootParameter;

	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;

	std::vector<std::pair<uint32_t, std::string>> rootParms;

	rootParms =
		shaderCompiler_->CreateRootParameters(vertexShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		psoBlend.rootParm.push_back(rootParm);
	}

	//vertexShaderReflection->Release();

	rootParms =
		shaderCompiler_->CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		psoBlend.rootParm.push_back(rootParm);
	}

	//pixelShaderReflection->Release();

	for (int32_t i = 0; i < rootParameter.size(); ++i) {
		rootParameter[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}


	descriptionRooTSignature.pParameters = rootParameter.data();// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = static_cast<UINT>(rootParameter.size());// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = dxgiFormat;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeScreen]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSODrawLine()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob =
		shaderCompiler_->CompilerShader(
			vsPath+L"Line.VS.hlsl",
			L"vs_6_5");
	assert(vertexShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> vertexShaderReflection = nullptr;
	vertexShaderReflection = shaderCompiler_->ReflectShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		shaderCompiler_->CompilerShader(
			psPath+L"Line.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = shaderCompiler_->ReflectShader(pixelShaderBlob.Get());

	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	std::vector< D3D12_ROOT_PARAMETER>rootParameter;

	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;

	std::vector<std::pair<uint32_t, std::string>> rootParms;

	rootParms =
		shaderCompiler_->CreateRootParameters(vertexShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		psoBlend.rootParm.push_back(rootParm);
	}

	//vertexShaderReflection->Release();

	rootParms =
		shaderCompiler_->CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		psoBlend.rootParm.push_back(rootParm);
	}

	//pixelShaderReflection->Release();

	for (int32_t i = 0; i < rootParameter.size(); ++i) {
		rootParameter[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}


	descriptionRooTSignature.pParameters = rootParameter.data();// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = static_cast<UINT>(rootParameter.size());// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "COLOR";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = dxgiFormat;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOMapChip()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob =
		shaderCompiler_->CompilerShader(
			vsPath+L"MapChipBlocks.VS.hlsl",
			L"vs_6_5");
	assert(vertexShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> vertexShaderReflection = nullptr;
	vertexShaderReflection = shaderCompiler_->ReflectShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		shaderCompiler_->CompilerShader(
			psPath+L"DemoOBJ.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = shaderCompiler_->ReflectShader(pixelShaderBlob.Get());

	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	std::vector< D3D12_ROOT_PARAMETER>rootParameter;

	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;

	std::vector<std::pair<uint32_t, std::string>> rootParms;

	rootParms =
		shaderCompiler_->CreateRootParameters(vertexShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		psoBlend.rootParm.push_back(rootParm);
	}

	//vertexShaderReflection->Release();

	rootParms =
		shaderCompiler_->CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		psoBlend.rootParm.push_back(rootParm);
	}

	//pixelShaderReflection->Release();

	for (int32_t i = 0; i < rootParameter.size(); ++i) {
		rootParameter[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}


	descriptionRooTSignature.pParameters = rootParameter.data();// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = static_cast<UINT>(rootParameter.size());// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = dxgiFormat;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeScreen]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOEffectSprite()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob =
		shaderCompiler_->CompilerShader(
			vsPath+L"Effect_Sprite.VS.hlsl",
			L"vs_6_5");
	assert(vertexShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> vertexShaderReflection = nullptr;
	vertexShaderReflection = shaderCompiler_->ReflectShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		shaderCompiler_->CompilerShader(
			psPath+L"Effect_Sprite.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = shaderCompiler_->ReflectShader(pixelShaderBlob.Get());

	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	std::vector< D3D12_ROOT_PARAMETER>rootParameter;

	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;

	std::vector<std::pair<uint32_t, std::string>> rootParms;

	rootParms =
		shaderCompiler_->CreateRootParameters(vertexShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		psoBlend.rootParm.push_back(rootParm);
	}

	//vertexShaderReflection->Release();

	rootParms =
		shaderCompiler_->CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		psoBlend.rootParm.push_back(rootParm);
	}

	//pixelShaderReflection->Release();

	for (int32_t i = 0; i < rootParameter.size(); ++i) {
		rootParameter[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}


	descriptionRooTSignature.pParameters = rootParameter.data();// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = static_cast<UINT>(rootParameter.size());// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[4] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[3].SemanticName = "COLOR";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を無効化
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	depthStencilDesc.StencilEnable = false;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = dxgiFormat;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeScreen]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOEffectInit()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob =
		shaderCompiler_->CompilerShader(
			csPath+L"EffectParticleInit.CS.hlsl",
			L"cs_6_5");
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

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange1 = {};
	descriptorUAVRange1.BaseShaderRegister = 1;
	descriptorUAVRange1.NumDescriptors = 1;
	descriptorUAVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &descriptorUAVRange1;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange2 = {};
	descriptorUAVRange2.BaseShaderRegister = 2;
	descriptorUAVRange2.NumDescriptors = 1;
	descriptorUAVRange2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorUAVRange2;// Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };
	computePipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOEffectEmit()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob =
		shaderCompiler_->CompilerShader(
			csPath+L"EffectParticleEmit.CS.hlsl",
			L"cs_6_5");
	assert(computeShaderBlob);

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[5] = {};

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// PixelShaderを使う
	rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号0とバインド

	D3D12_DESCRIPTOR_RANGE descriptorSRVRange = {};
	descriptorSRVRange.BaseShaderRegister = 0;
	descriptorSRVRange.NumDescriptors = 1;
	descriptorSRVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorSRVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &descriptorSRVRange;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange = {};
	descriptorUAVRange.BaseShaderRegister = 0;
	descriptorUAVRange.NumDescriptors = 1;
	descriptorUAVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange1 = {};
	descriptorUAVRange1.BaseShaderRegister = 1;
	descriptorUAVRange1.NumDescriptors = 1;
	descriptorUAVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[3].DescriptorTable.pDescriptorRanges = &descriptorUAVRange1;// Tableの中身の配列を指定
	rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange2 = {};
	descriptorUAVRange2.BaseShaderRegister = 2;
	descriptorUAVRange2.NumDescriptors = 1;
	descriptorUAVRange2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[4].DescriptorTable.pDescriptorRanges = &descriptorUAVRange2;// Tableの中身の配列を指定
	rootParameter[4].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };
	computePipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOEffectUpdate()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob =
		shaderCompiler_->CompilerShader(
			csPath+L"EffectParticleUpdate.CS.hlsl",
			L"cs_6_5");
	assert(computeShaderBlob);

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[5] = {};

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// PixelShaderを使う
	rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号0とバインド

	D3D12_DESCRIPTOR_RANGE descriptorSRVRange = {};
	descriptorSRVRange.BaseShaderRegister = 0;
	descriptorSRVRange.NumDescriptors = 1;
	descriptorSRVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorSRVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &descriptorSRVRange;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange = {};
	descriptorUAVRange.BaseShaderRegister = 0;
	descriptorUAVRange.NumDescriptors = 1;
	descriptorUAVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange1 = {};
	descriptorUAVRange1.BaseShaderRegister = 1;
	descriptorUAVRange1.NumDescriptors = 1;
	descriptorUAVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[3].DescriptorTable.pDescriptorRanges = &descriptorUAVRange1;// Tableの中身の配列を指定
	rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange2 = {};
	descriptorUAVRange2.BaseShaderRegister = 2;
	descriptorUAVRange2.NumDescriptors = 1;
	descriptorUAVRange2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[4].DescriptorTable.pDescriptorRanges = &descriptorUAVRange2;// Tableの中身の配列を指定
	rootParameter[4].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };
	computePipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSOMapChipGBuffer()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob =
		shaderCompiler_->CompilerShader(
			vsPath+L"MapChipBlocks.VS.hlsl",
			L"vs_6_5");
	assert(vertexShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> vertexShaderReflection = nullptr;
	vertexShaderReflection = shaderCompiler_->ReflectShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		shaderCompiler_->CompilerShader(
			psPath+L"GBuffer.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = shaderCompiler_->ReflectShader(pixelShaderBlob.Get());

	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	std::vector< D3D12_ROOT_PARAMETER>rootParameter;

	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;

	std::vector<std::pair<uint32_t, std::string>> rootParms;

	rootParms =
		shaderCompiler_->CreateRootParameters(vertexShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		psoBlend.rootParm.push_back(rootParm);
	}

	//vertexShaderReflection->Release();

	rootParms =
		shaderCompiler_->CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		psoBlend.rootParm.push_back(rootParm);
	}

	//pixelShaderReflection->Release();

	for (int32_t i = 0; i < rootParameter.size(); ++i) {
		rootParameter[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}


	descriptionRooTSignature.pParameters = rootParameter.data();// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = static_cast<UINT>(rootParameter.size());// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 4;// 三つ
	graphicsPipelineStateDesc.RTVFormats[0] = dxgiFormat;
	graphicsPipelineStateDesc.RTVFormats[1] = dxgiFormat;
	graphicsPipelineStateDesc.RTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	graphicsPipelineStateDesc.RTVFormats[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeScreen]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}

void Pipeline::CreatePSODif_ToonLighting()
{
	HRESULT hr;
	PSOBlend psoBlend;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob =
		shaderCompiler_->CompilerShader(
			vsPath+L"FullScreen.VS.hlsl",
			L"vs_6_5");
	assert(vertexShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> vertexShaderReflection = nullptr;
	vertexShaderReflection = shaderCompiler_->ReflectShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		shaderCompiler_->CompilerShader(
			psPath+L"Dfe_ToonLighting.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = shaderCompiler_->ReflectShader(pixelShaderBlob.Get());

	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	std::vector< D3D12_ROOT_PARAMETER>rootParameter;

	std::vector<D3D12_DESCRIPTOR_RANGE>descriptorRange;

	std::vector<std::pair<uint32_t, std::string>> rootParms;

	rootParms =
		shaderCompiler_->CreateRootParameters(vertexShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		psoBlend.rootParm.push_back(rootParm);
	}

	//vertexShaderReflection->Release();

	rootParms =
		shaderCompiler_->CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		psoBlend.rootParm.push_back(rootParm);
	}

	//pixelShaderReflection->Release();

	for (int32_t i = 0; i < rootParameter.size(); ++i) {
		rootParameter[i].DescriptorTable.pDescriptorRanges = &descriptorRange[i];
	}


	descriptionRooTSignature.pParameters = rootParameter.data();// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = static_cast<UINT>(rootParameter.size());// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = d3dDevice_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&psoBlend.rootSignature));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	/*inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;*/

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を無効化
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	depthStencilDesc.StencilEnable = false;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = psoBlend.rootSignature.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = dxgiFormat;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = d3dDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoBlend.Blend[kBlendModeScreen]));
	assert(SUCCEEDED(hr));

	psoBlends.push_back(psoBlend);
}


