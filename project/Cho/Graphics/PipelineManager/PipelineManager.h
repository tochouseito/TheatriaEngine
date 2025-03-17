#pragma once
#include "Cho/SDK/DirectX/DXC/DXShaderCompiler.h"

struct PSO
{
	ComPtr<ID3D12PipelineState> pso;
	ComPtr<ID3D12RootSignature> rootSignature;
	std::vector<std::pair<uint32_t, std::string>> rootParameters;
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
private:
	std::vector<std::pair<uint32_t, std::string>> CreateRootParameters(
		ID3D12ShaderReflection* pReflector,
		std::vector<D3D12_ROOT_PARAMETER>& rootParameters,
		std::vector<D3D12_DESCRIPTOR_RANGE>& descriptorRanges,
		D3D12_SHADER_VISIBILITY VISIBILITY
	);
	void CreatePipelineDemo(ID3D12Device8* device);
	void CreatePipelineScreenCopy(ID3D12Device8* device);

	std::unique_ptr<DXShaderCompiler> m_pShaderCompiler = nullptr;
	PSO m_DemoPSO;
	PSO m_ScreenCopyPSO;
};

