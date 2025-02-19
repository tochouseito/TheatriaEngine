#pragma once

#include<wrl.h>
#include<d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

// C++
#include<array>
#include <vector>

// Utility
#include"PSOMode.h"
#include"BlendMode.h"

class D3DDevice;
class ShaderCompiler;
class Pipeline
{
public:
	struct PSOBlend {
		std::array<Microsoft::WRL::ComPtr <ID3D12PipelineState>,
			BlendMode::kCountOfBlendMode> Blend;
		Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
		std::vector<std::pair<uint32_t, std::string>> rootParm;
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(D3DDevice* d3dDevice,ShaderCompiler* shaderCompiler);

	PSOBlend& GetPSO(const PSOMode& index) { return psoBlends[index]; }

private:

	void CreatePSODemo();

	void CreatePSOScreenCopy();

	void CreatePSOSprite();

	void CreatePSOParticle();

	void CreatePSOParticleInit();

	void CreatePSOEmit();

	void CreatePSOUpdate();

	void CreatePSOWireframe();

	void CreatePSOSkinningCS();

	void CreatePSODemoMS();

	void CreatePSOGBuffer();

	void CreatePSOWireframeGB();

	void CreatePSOGBufferMix();

	void CreatePSOWireframeMix();

	void CreatePSODrawLine();

	void CreatePSOMapChip();

	void CreatePSOEffectSprite();

	void CreatePSOEffectInit();

	void CreatePSOEffectEmit();

	void CreatePSOEffectUpdate();

	void CreatePSOMapChipGBuffer();

	void CreatePSODif_ToonLighting();
private:

	D3DDevice* d3dDevice_ = nullptr;

	std::vector<PSOBlend> psoBlends;
	
	ShaderCompiler* shaderCompiler_ = nullptr;

	std::wstring psPath = L"Cho/Resources/Shader/PixelShader/";
	std::wstring vsPath = L"Cho/Resources/Shader/VertexShader/";
	std::wstring csPath = L"Cho/Resources/Shader/ComputeShader/";
	std::wstring msPath = L"Cho/Resources/Shader/MeshShader/";
};

