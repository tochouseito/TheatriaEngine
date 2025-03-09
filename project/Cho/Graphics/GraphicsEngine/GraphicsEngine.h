#pragma once
#include "Engine/Engine.h"
#include "Graphics/GraphicsCore/GraphicsCore.h"
#include "SDK/DirectX/DirectX12/SwapChain/SwapChain.h"
#include "SDK/DirectX/DirectX12/DescriptorHeap/DescriptorHeap.h"
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "SDK/DirectX/DirectX12/ColorBuffer/ColorBuffer.h"

class ResourceManager;

class GraphicsEngine : public Engine
{
public:
	// Constructor
	GraphicsEngine(ID3D12Device8* device) : 
		m_Device(device) 
	{
		m_GraphicsCore = std::make_unique<GraphicsCore>(device);
	}
	// Destructor
	~GraphicsEngine() = default;
	void Initialize() override;
	void Finalize() override;
	void Operation() override;
	void Update(ResourceManager& resourceManager);
	void CreateSwapChain(IDXGIFactory7* dxgiFactory, ResourceManager* resourceManager);
private:
	ID3D12Device8* m_Device = nullptr;
	std::unique_ptr<SwapChain> m_SwapChain = nullptr;
	std::unique_ptr<GraphicsCore> m_GraphicsCore = nullptr;
};

