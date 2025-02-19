#include "PrecompiledHeader.h"
#include "ShaderCompiler.h"
#include"D3D12/D3DDevice/D3DDevice.h"
#include<assert.h>

ShaderCompiler::~ShaderCompiler()
{
	includeHandler_->Release();
}

void ShaderCompiler::Initialize(D3DDevice* d3dDevice)
{
	d3dDevice_ = d3dDevice;

	// DXコンパイラー初期化
	InitializeDxcCompiler();

	// HLSLファイルの名前を読み込み
	hlslFiles_ = GetHLSLFilesFromFolder(folderPath_);

	//CreateRootParm();
}

IDxcBlob* ShaderCompiler::CompilerShader(
	const std::wstring& filePath,
	const wchar_t* profile
)
{
	// hlslファイルを読む
	// これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompilerShader,path:{},profile:{}\n", filePath, profile)));
	// hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils_.Get()->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;// UTF8の文字コードであることを通知
	LPCWSTR arguments[] = {
		filePath.c_str(),       //コンパイル対象のhlslファイル名
		L"-E",L"main",          // エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,          // ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",// デバッグ用の情報を埋め込む
		L"-Od",                 // 最適化を外しておく
		L"-Zpr",                // メモリレイアウトは行優先
	};
	// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler_.Get()->Compile(
		&shaderSourceBuffer,       // 読み込んだファイル
		arguments,                 // コンパイルオプション
		_countof(arguments),       // コンパイル結果
		includeHandler_,     // includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)// コンパイル結果
	);
	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	// 警告、エラーがでてたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	IDxcBlobUtf16* shaderOutputName;// 出力名用の変数
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), &shaderOutputName);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		// エラー警告ダメゼッタイ
		assert(false&&shaderError->GetStringPointer());
	}

	// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), &shaderOutputName);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
	// もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	shaderError->Release();
	// 実行用のバイナリを返却
	return shaderBlob;
}

IDxcBlob* ShaderCompiler::CompilerShaderWithCache(const std::wstring& filePath, const wchar_t* profile)
{
	// キャッシュに存在するか確認
	auto it = shaderCache_.find(filePath);
	if (it != shaderCache_.end()) {
		return it->second.Get(); // キャッシュから取得
	}

	// シェーダーをコンパイル
	IDxcBlob* shaderBlob = CompilerShader(filePath, profile);
	if (shaderBlob) {
		// コンパイル結果をキャッシュに保存
		shaderCache_[filePath] = shaderBlob;
	}
	return shaderBlob;
}

Microsoft::WRL::ComPtr < ID3D12ShaderReflection> ShaderCompiler::ReflectShader(IDxcBlob* shaderBlob)
{
	Microsoft::WRL::ComPtr<IDxcContainerReflection> pContainerReflection;
	HRESULT hr = DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&pContainerReflection));
	assert(SUCCEEDED(hr));

	// コンテナにシェーダーブロブをロード
	hr = pContainerReflection->Load(shaderBlob);
	assert(SUCCEEDED(hr));

	// DXIL部分のインデックスを取得
	UINT32 shaderIdx;
	hr = pContainerReflection->FindFirstPartKind(DXC_PART_DXIL, &shaderIdx);
	assert(SUCCEEDED(hr));

	// シェーダーリフレクションインターフェースを取得
	Microsoft::WRL::ComPtr<ID3D12ShaderReflection> pReflector;
	hr = pContainerReflection->GetPartReflection(shaderIdx, IID_PPV_ARGS(&pReflector));
	assert(SUCCEEDED(hr));

	return pReflector;
}

void ShaderCompiler::CreateRootParm()
{
	HRESULT hr;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob =
		CompilerShader(
			L"Engine/Resources/Shader/DemoOBJ.VS.hlsl",
			L"vs_6_5");
	assert(vertexShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> vertexShaderReflection = nullptr;
	vertexShaderReflection = ReflectShader(vertexShaderBlob.Get());

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob =
		CompilerShader(
			L"Engine/Resources/Shader/DemoOBJ.PS.hlsl",
			L"ps_6_5");
	assert(pixelShaderBlob);

	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> pixelShaderReflection = nullptr;
	pixelShaderReflection = ReflectShader(pixelShaderBlob.Get());

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
		CreateRootParameters(vertexShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_VERTEX);

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParmDemo.push_back(rootParm);
	}

	vertexShaderReflection->Release();

	rootParms =
		CreateRootParameters(pixelShaderReflection.Get(), rootParameter, descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	uint32_t offset = static_cast<uint32_t>(rootParms.size());

	for (std::pair<uint32_t, std::string> rootParm : rootParms) {
		rootParm.first += offset;
		rootParmDemo.push_back(rootParm);
	}

	pixelShaderReflection->Release();

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
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatureDemo));
	assert(SUCCEEDED(hr));
}

void ShaderCompiler::InitializeDxcCompiler()
{
	HRESULT hr;
	// dxcCompilerを初期化
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	// 現時点でincludeはしないが、includeに対応するための設定を作っておく
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

std::vector<std::pair<uint32_t, std::string>> ShaderCompiler::CreateRootParameters(
	ID3D12ShaderReflection* pReflector,
	std::vector<D3D12_ROOT_PARAMETER>& rootParameters,
	std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges,
	D3D12_SHADER_VISIBILITY VISIBILITY
)
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
				if (resourceDesc.Type == D3D_SIT_SAMPLER) {// サンプラーだったら抜ける
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
				}
				else if (resourceDesc.Type == D3D_SIT_UAV_RWSTRUCTURED)  // 書き込み可能なリソースの場合
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

std::vector<std::string> ShaderCompiler::GetHLSLFilesFromFolder(const std::string& folderPath)
{
	std::vector<std::string> hlslFiles;

	// ディレクトリの存在を確認
	if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
		//std::cerr << "フォルダが存在しないか、ディレクトリではありません: " << folderPath << std::endl;
		return hlslFiles;
	}

	// ディレクトリを走査
	for (const auto& entry : fs::directory_iterator(folderPath)) {
		if (entry.is_regular_file() && entry.path().extension() == ".hlsl") {
			// ファイル名のみを保存 (拡張子を含むファイル名)
			hlslFiles.push_back(entry.path().filename().string());
		}
	}

	return hlslFiles;
}
