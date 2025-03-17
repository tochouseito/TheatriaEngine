#pragma once
#include <d3d12.h>
class ResourceManager;
class ImGuiManager
{
public:

	void Initialize(ID3D12Device8* device,ResourceManager* resourceManager);
	void Finalize();
	void Begin();
	void End();
	void Draw(ID3D12GraphicsCommandList6* commandList);
private:
	uint32_t m_DHandleIndex = {};
};

