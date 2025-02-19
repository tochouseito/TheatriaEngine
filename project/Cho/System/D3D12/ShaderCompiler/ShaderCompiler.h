#pragma once

#include<d3d12.h>
#include<d3d12shader.h>
#include <d3dcompiler.h>
#include<dxcapi.h>

// C++
#include<array>
#include <iostream>
#include <filesystem>
#include <vector>
#include<unordered_map>
#include <string>
#include<wrl.h>

namespace fs = std::filesystem;

class D3DDevice;
class ShaderCompiler
{
public:

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ShaderCompiler();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(D3DDevice* d3dDevice);

	/// <summary>
	/// shaderのコンパイル
	/// </summary>
	IDxcBlob* CompilerShader(
		// CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		// Compilerに使用するProfile
		const wchar_t* profile
	);

	// すでにコンパイルされたシェーダーはスキップするバージョン
	IDxcBlob* CompilerShaderWithCache(
		const std::wstring& filePath,
		const wchar_t* profile
	);

	/// <summary>
	/// シェーダーリフレクション
	/// </summary>
	/// <param name="shaderBlob"></param>
	Microsoft::WRL::ComPtr < ID3D12ShaderReflection> ReflectShader(IDxcBlob* shaderBlob);

	/// <summary>
	/// hlslファイルの数取得
	/// </summary>
	/// <returns></returns>
	uint32_t GetHLSLFilesSize()const { return static_cast<uint32_t>(hlslFiles_.size()); }

	/// <summary>
	/// 仮のルートシグネチャ生成用
	/// </summary>
	void CreateRootParm();
	ID3D12RootSignature* GetRootSig()const { return rootSignatureDemo.Get(); }
	/// <summary>
	/// ルートパラメータ生成
	/// </summary>
	std::vector<std::pair<uint32_t, std::string>> CreateRootParameters(
		ID3D12ShaderReflection* pReflector,
		std::vector<D3D12_ROOT_PARAMETER>& rootParameters,
		std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges,
		D3D12_SHADER_VISIBILITY VISIBILITY
	);
private:

	/// <summary>
	/// dxcCompilerを初期化
	/// <summary>
	void InitializeDxcCompiler();

	// 指定されたフォルダから .hlsl ファイルを探し、そのファイル名を vector に保存する関数
	std::vector<std::string> GetHLSLFilesFromFolder(const std::string& folderPath);

private:

	D3DDevice* d3dDevice_ = nullptr;

	/*dxcCompiler*/
	Microsoft::WRL::ComPtr < IDxcUtils> dxcUtils_;
	Microsoft::WRL::ComPtr < IDxcCompiler3> dxcCompiler_;
	IDxcIncludeHandler* includeHandler_;

	std::string folderPath_ = "Cho/Resources/Shader"; // 対象のフォルダパスを指定
	std::vector<std::string> hlslFiles_;

	std::vector<std::pair<uint32_t, std::string>> rootParmDemo;
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignatureDemo;

	std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<IDxcBlob>> shaderCache_;
};

