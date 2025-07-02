#pragma once
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "SDK/DirectX/DXC/DXShaderCompiler.h"
#include <memory>

// test
struct IndirectArgs
{
	D3D12_GPU_VIRTUAL_ADDRESS cbv_ViewProjection;
	D3D12_GPU_VIRTUAL_ADDRESS srv_IntegrationTF;
	D3D12_GPU_VIRTUAL_ADDRESS srv_UseTransformList;
	D3D12_GPU_VIRTUAL_ADDRESS srv_BoneMatrix;
	D3D12_GPU_VIRTUAL_ADDRESS srv_SkinningInfluence;
	D3D12_GPU_VIRTUAL_ADDRESS cbv_SkinningInfo;
	D3D12_GPU_VIRTUAL_ADDRESS cbv_Lights;
	D3D12_GPU_VIRTUAL_ADDRESS cbv_Environment;
	D3D12_GPU_VIRTUAL_ADDRESS srv_IntegrationTF2;
	D3D12_GPU_VIRTUAL_ADDRESS srv_IntegrationMTL;
	D3D12_DRAW_INDEXED_ARGUMENTS drawIndexedArgs;// DrawIndexedIndirect用引数
};

struct PSO
{
	ComPtr<ID3D12PipelineState> pso;
	ComPtr<ID3D12RootSignature> rootSignature;
	std::vector<std::pair<uint32_t, std::string>> rootParameters;
	ComPtr<ID3D12CommandSignature> commandSignature;// コマンドシグネチャ
	std::unique_ptr<ConstantBuffer<IndirectArgs>> indirectArgsBuffer;// IndirectArgs用バッファ
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
class ResourceManager;
class GraphicsEngine;
class PipelineManager
{
public:
	// Constructor
	PipelineManager(ResourceManager* resourceManager,GraphicsEngine* graphicsEngine)
	{
		m_pShaderCompiler = std::make_unique<DXShaderCompiler>();
		m_pResourceManager = resourceManager;
		m_pGraphicsEngine = graphicsEngine;
	}
	// Destructor
	~PipelineManager()
	{

	}
	// Initialize
	void Initialize(ID3D12Device8* device);

	// Get PSO
	const PSO& GetDemoPSO() { return m_DemoPSO; }
	const PSO& GetScreenCopyPSO() { return m_ScreenCopyPSO; }
	const PSO& GetIntegratePSO() { return m_IntegratePSO; }
	const PSO& GetLinePSO() { return m_LinePSO; }
	// Particle
	const PSO& GetParticlePSO() { return m_ParticlePSO; }
	const PSO& GetParticleInitPSO() { return m_ParticleInitPSO; }
	const PSO& GetParticleUpdatePSO() { return m_ParticleUpdatePSO; }
	const PSO& GetParticleEmitPSO() { return m_ParticleEmitPSO; }
	// EffectEditor
	const PSO& GetEffectEditorPSO() { return m_EffectEditorPSO; }
	const PSO& GetEffectEditorInitPSO() { return m_EffectEditorInitPSO; }
	const PSO& GetEffectEditorUpdatePSO() { return m_EffectEditorUpdatePSO; }
	const PSO& GetEffectEditorEmitPSO() { return m_EffectEditorEmitPSO; }
	// Effect
	const PSO& GetEffectInitPSO() { return m_EffectInitPSO; }
	// EffectTimeBase
	const PSO& GetEffectTimeBaseEmitPSO() { return m_EffectTimeBaseEmitPSO; }
	const PSO& GetEffectTimeBaseUpdatePSO() { return m_EffectTimeBaseUpdatePSO; }
	// EffectSprite
	const PSO& GetEffectSpritePSO() { return m_EffectSpritePSO; }

	// UI
	const PSO& GetUIPSO() { return m_UIPSO; }
	// SkinningCS
	const PSO& GetSkinningPSO() { return m_Skinning; }
	// Skybox
	const PSO& GetSkyboxPSO() { return m_SkyboxPSO; }
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

	// Effect
	void CreatePipelineEffectInit(ID3D12Device8* device);
	// EffectTimeBase
	void CreatePipelineEffectTimeBaseEmit(ID3D12Device8* device);
	void CreatePipelineEffectTimeBaseUpdate(ID3D12Device8* device);
	// EffectSprite
	void CreatePipelineEffectSprite(ID3D12Device8* device);

	// UI
	void CreatePipelineUI(ID3D12Device8* device);

	// SkinningCS
	void CreatePipelineSkinningCS(ID3D12Device8* device);

	// Skybox
	void CreatePipelineSkybox(ID3D12Device8* device);

	std::unique_ptr<DXShaderCompiler> m_pShaderCompiler = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	GraphicsEngine* m_pGraphicsEngine = nullptr;
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

	// Effect
	PSO m_EffectInitPSO;

	// EffectTimeBase
	PSO m_EffectTimeBaseEmitPSO;
	PSO m_EffectTimeBaseUpdatePSO;

	// EffectSprite
	PSO m_EffectSpritePSO;

	// UI
	PSO m_UIPSO;

	// SkinningCS
	PSO m_Skinning;

	// Skybox
	PSO m_SkyboxPSO;
};

