#include "pch.h"
#include "DXShaderCompiler.h"
#include "Cho/Core/Log/Log.h"

void DXShaderCompiler::Initialize(ID3D12Device8* device)
{
	device;
	HRESULT hr = {};
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_pUtils));
	ChoAssertLog("Failed to create DXC Utils", hr,__FILE__,__LINE__);
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_pCompiler));
	ChoAssertLog("Failed to create DXC Compiler", hr, __FILE__, __LINE__);
	hr = m_pUtils->CreateDefaultIncludeHandler(&m_pIncludeHandler);
	ChoAssertLog("Failed to create DXC Include Handler", hr, __FILE__, __LINE__);
}

IDxcBlob* DXShaderCompiler::CompileShader(const std::wstring& filePath, const wchar_t* profile)
{
	HRESULT hr = {};
	ChoLog(ConvertString(std::format(L"Begin CompilerShader,path:{},profile:{}\n", filePath, profile)));
	ComPtr<IDxcBlobEncoding> pSource = nullptr;
	hr = m_pUtils.Get()->LoadFile(filePath.c_str(), nullptr, &pSource);
	ChoAssertLog("Failed to load file", hr, __FILE__, __LINE__);
	DxcBuffer sourceBuffer;
	sourceBuffer.Ptr = pSource->GetBufferPointer();
	sourceBuffer.Size = pSource->GetBufferSize();
	sourceBuffer.Encoding = DXC_CP_UTF8;
	LPCWSTR arguments[] = {
		filePath.c_str(),       //コンパイル対象のhlslファイル名
		L"-E",L"main",          // エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,          // ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",// デバッグ用の情報を埋め込む
		L"-Od",                 // 最適化を外しておく
		L"-Zpr",                // メモリレイアウトは行優先
	};
	ComPtr<IDxcResult> pResult = nullptr;
	hr = m_pCompiler.Get()->Compile(
		&sourceBuffer,			// 読み込んだファイル
		arguments,				// コンパイルオプション
		_countof(arguments),	// コンパイル結果
		m_pIncludeHandler.Get(),// includeが含まれた諸々
		IID_PPV_ARGS(&pResult)	// コンパイル結果
	);
	ChoAssertLog("Failed to compile shader", hr, __FILE__, __LINE__);
	ComPtr<IDxcBlobUtf8> pErrors = nullptr;
	ComPtr<IDxcBlobUtf16> pErrorsUtf16;
	hr = pResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), &pErrorsUtf16);
	if (pErrors != nullptr && pErrors->GetStringLength() != 0)
	{
		ChoLog(pErrors->GetStringPointer());
		ChoAssertLog("Failed to compile shader", false, __FILE__, __LINE__);
	}
	IDxcBlob* pShader = nullptr;
	hr = pResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), &pErrorsUtf16);
	ChoAssertLog("Failed to compile shader", hr, __FILE__, __LINE__);
	ChoLog(ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
	return pShader;
}

ID3D12ShaderReflection* DXShaderCompiler::ReflectShader(IDxcBlob* shaderBlob)
{
	HRESULT hr = {};
	ComPtr<IDxcContainerReflection> pReflection = nullptr;
	hr = DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&pReflection));
	ChoAssertLog("Failed to create DXC Container Reflection", hr, __FILE__, __LINE__);
	hr = pReflection->Load(shaderBlob);
	ChoAssertLog("Failed to load shader blob", hr, __FILE__, __LINE__);
	UINT32 partIndex = 0;
	hr = pReflection->FindFirstPartKind(DXC_PART_DXIL, &partIndex);
	ChoAssertLog("Failed to find first part kind", hr, __FILE__, __LINE__);
	ID3D12ShaderReflection* pShaderReflection = nullptr;
	hr = pReflection->GetPartReflection(partIndex, IID_PPV_ARGS(&pShaderReflection));
	ChoAssertLog("Failed to get part reflection", hr, __FILE__, __LINE__);
	return pShaderReflection;
}
