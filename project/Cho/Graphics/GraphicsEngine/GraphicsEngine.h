#pragma once
#include "Engine/Engine.h"
#include "Graphics/GraphicsCore/GraphicsCore.h"
#include "Graphics/DepthManager/DepthManager.h"
#include "Graphics/PipelineManager/PipelineManager.h"
#include "SDK/DirectX/DirectX12/SwapChain/SwapChain.h"
#include "SDK/DirectX/DirectX12/DescriptorHeap/DescriptorHeap.h"
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "SDK/DirectX/DirectX12/ColorBuffer/ColorBuffer.h"
#include <array>

enum RenderTextureType
{
	OffScreen = 0,// ゲーム画面用描画結果
	TypeCount,// 種類数(使用禁止)
};

enum DrawPass
{
	GBuffers = 0,
	Lighting,
	Forward,
	PostProcess,// ポストプロセス、最終描画
	SwapChainPass,// SwapChainのバックバッファへの描画
	PassCount,
};

struct RenderTexture
{
	std::optional<uint32_t> m_BufferIndex = std::nullopt;
	UINT64 m_Width = 0;
	UINT m_Height = 0;
};

class ResourceManager;
class ImGuiManager;
class GameCore;
class GraphicsEngine : public Engine
{
	friend class TextureManager;
public:
	// Constructor
	GraphicsEngine(ID3D12Device8* device,ResourceManager* resourceManager) : 
		m_Device(device), m_ResourceManager(resourceManager)
	{
		m_GraphicsCore = std::make_unique<GraphicsCore>(device);
		m_DepthManager = std::make_unique<DepthManager>();
		m_PipelineManager = std::make_unique<PipelineManager>();
	}
	// Destructor
	~GraphicsEngine() = default;
	void Init();
	void CreateSwapChain(IDXGIFactory7* dxgiFactory);
	void PreRender();
	void Render(ResourceManager& resourceManager, GameCore& gameCore);
	void PostRender(ImGuiManager* imgui);
	void PostRenderWithImGui(ImGuiManager* imgui);

	// SceneTextureのBufferIDを取得
	uint32_t GetSceneTextureBufferID() { return m_RenderTextures[OffScreen].m_BufferIndex.value(); }
private:
	// コマンドコンテキストの取得
	CommandContext* GetCommandContext() { return m_GraphicsCore->GetCommandManager()->GetCommandContext(); }
	// 記録開始
	void BeginCommandContext(CommandContext* context);
	// 記録終了
	void EndCommandContext(CommandContext* context,const QueueType& queue);
	// GPU待機
	void WaitForGPU(const QueueType& queue);
	// レンダーターゲットの設定
	void SetRenderTargets(CommandContext* context,DrawPass pass);
	// 描画設定コマンド
	void SetRenderState(CommandContext* context);
	void DrawGBuffers(ResourceManager& resourceManager, GameCore& gameCore);
	void DrawLighting(ResourceManager& resourceManager, GameCore& gameCore);
	void DrawForward(ResourceManager& resourceManager, GameCore& gameCore);
	void DrawPostProcess(ResourceManager& resourceManager, GameCore& gameCore);
	void CreateDepthBuffer();
	void CreateOffscreenBuffer();

	ID3D12Device8* m_Device = nullptr;
	ResourceManager* m_ResourceManager = nullptr;
	std::unique_ptr<SwapChain> m_SwapChain = nullptr;
	std::unique_ptr<GraphicsCore> m_GraphicsCore = nullptr;
	std::unique_ptr<DepthManager> m_DepthManager = nullptr;
	std::unique_ptr<PipelineManager> m_PipelineManager = nullptr;

	std::array<RenderTexture, TypeCount> m_RenderTextures;
};

