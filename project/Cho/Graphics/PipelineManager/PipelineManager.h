#pragma once
#include "SDK/DirectX/DXC/DXShaderCompiler.h"

struct PSO
{
	ComPtr<ID3D12PipelineState> pso;
	ComPtr<ID3D12RootSignature> rootSignature;
	std::vector<std::pair<uint32_t, std::string>> rootParameters;
};
struct PipelineStateObject
{
	ComPtr<ID3D12PipelineState> pipelineState;// パイプラインステートオブジェクト
	ComPtr<ID3D12RootSignature> rootSignature;// ルートシグネチャ
	std::string vsPath;// VertexShader
	std::string psPath;// PixelShader
	std::string gsPath;// GeometryShader
	std::string csPath;// ComputeShader
	std::string hsPath;// HullShader
	std::string dsPath;// DomainShader
	// メッシュシェーダー、増幅シェーダー追加予定
};
class PipelineManager
{
public:
	// Constructor
	PipelineManager()
	{
		m_pShaderCompiler = std::make_unique<DXShaderCompiler>();
	}
	// Destructor
	~PipelineManager()
	{

	}
	// Initialize
	void Initialize(ID3D12Device8* device);

	// Get PSO
	PSO GetDemoPSO() { return m_DemoPSO; }
	PSO GetScreenCopyPSO() { return m_ScreenCopyPSO; }
	PSO GetIntegratePSO() { return m_IntegratePSO; }
	PSO GetLinePSO() { return m_LinePSO; }
	// Particle
	PSO GetParticlePSO() { return m_ParticlePSO; }
	PSO GetParticleInitPSO() { return m_ParticleInitPSO; }
	PSO GetParticleUpdatePSO() { return m_ParticleUpdatePSO; }
	PSO GetParticleEmitPSO() { return m_ParticleEmitPSO; }
	// EffectEditor
	PSO GetEffectEditorPSO() { return m_EffectEditorPSO; }
	PSO GetEffectEditorInitPSO() { return m_EffectEditorInitPSO; }
	PSO GetEffectEditorUpdatePSO() { return m_EffectEditorUpdatePSO; }
	PSO GetEffectEditorEmitPSO() { return m_EffectEditorEmitPSO; }
	// UI
	PSO GetUIPSO() { return m_UIPSO; }
	// SkinningCS
	PSO GetSkinningPSO() { return m_Skinning; }
private:
	std::vector<std::pair<uint32_t, std::string>> CreateRootParameters(
		ID3D12ShaderReflection* pReflector,
		std::vector<D3D12_ROOT_PARAMETER>& rootParameters,
		std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges,
		D3D12_SHADER_VISIBILITY VISIBILITY
	);
	void CreatePipelineDemo(ID3D12Device8* device);
	void CreatePipelineScreenCopy(ID3D12Device8* device);
	void CreatePipelineIntegrate(ID3D12Device8* device);
	void CreatePipelineLine(ID3D12Device8* device);

	// Particle
	void CreatePipelineParticle(ID3D12Device8* device);
	void CreatePipelineParticleInit(ID3D12Device8* device);
	void CreatePipelineParticleUpdate(ID3D12Device8* device);
	void CreatePipelineParticleEmit(ID3D12Device8* device);

	// EffectEditor
	void CreatePipelineEffectEditor(ID3D12Device8* device);
	void CreatePipelineEffectEditorInit(ID3D12Device8* device);
	void CreatePipelineEffectEditorUpdate(ID3D12Device8* device);
	void CreatePipelineEffectEditorEmit(ID3D12Device8* device);

	// UI
	void CreatePipelineUI(ID3D12Device8* device);

	// SkinningCS
	void CreatePipelineSkinningCS(ID3D12Device8* device);

	std::unique_ptr<DXShaderCompiler> m_pShaderCompiler = nullptr;
	PSO m_DemoPSO;
	PSO m_ScreenCopyPSO;
	PSO m_IntegratePSO;
	PSO m_LinePSO;

	// Particle
	PSO m_ParticlePSO;
	PSO m_ParticleInitPSO;
	PSO m_ParticleUpdatePSO;
	PSO m_ParticleEmitPSO;

	// EffectEditor
	PSO m_EffectEditorPSO;
	PSO m_EffectEditorInitPSO;
	PSO m_EffectEditorUpdatePSO;
	PSO m_EffectEditorEmitPSO;

	// UI
	PSO m_UIPSO;

	// SkinningCS
	PSO m_Skinning;
};

