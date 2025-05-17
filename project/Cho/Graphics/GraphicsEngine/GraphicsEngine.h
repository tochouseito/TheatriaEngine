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
	GameScreen = 0,				// ゲーム画面用描画結果
	SceneScreen,				// シーン画面用描画結果
	EffectEditScreen,			// エフェクトエディタ用描画結果
	PostProcessScreen,			// ポストプロセス用描画結果
	ScenePostProcessScreen,		// シーンポストプロセス用描画結果
	RenderTextureTypeCount,		// 種類数(使用禁止)
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

enum RenderMode
{
	Game=0,
	Debug,
	Release,
	Editor,
	RenderModeCount,
};

enum ViewportType
{
	ViewportGame = 0,
	ViewportSwapChain,
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
class EngineCommand;
class GraphicsEngine : public Engine
{
	friend class TextureManager;
	friend class EngineCommand;
	friend class ParticleInitializeSystem;
	friend class ParticleUpdateSystem;
	friend class EffectEditorUpdateSystem;
public:
	// Constructor
	GraphicsEngine(ID3D12Device8* device,ResourceManager* resourceManager,RuntimeMode mode) : 
		m_Device(device), m_ResourceManager(resourceManager), Engine(mode)
	{
		m_GraphicsCore = std::make_unique<GraphicsCore>(device);
		m_DepthManager = std::make_unique<DepthManager>();
		m_PipelineManager = std::make_unique<PipelineManager>();
	}
	// Destructor
	~GraphicsEngine() = default;
	void Finalize() override
	{
		// GPUの完了待ち
		WaitForGPU(Graphics);
		WaitForGPU(Compute);
		WaitForGPU(Copy);
	}
	void Init();
	void SetEngineCommand(EngineCommand* engineCommand) { m_EngineCommand = engineCommand; }
	// SwapChainの生成
	void CreateSwapChain(IDXGIFactory7* dxgiFactory);
	void PreRender();
	void Render(ResourceManager& resourceManager, GameCore& gameCore, RenderMode mode = RenderMode::Game);
	void PostRender(ImGuiManager* imgui, RenderMode mode);
	void PostRenderWithImGui(ImGuiManager* imgui);
	// レンダーターゲットテクスチャ、ImGui、スワップチェーンのリサイズ
	void ScreenResize();

	PipelineManager* GetPipelineManager() { return m_PipelineManager.get(); }

	// GameTextureのBufferIDを取得
	uint32_t GetGameTextureBufferID() { return m_RenderTextures[GameScreen].m_BufferIndex.value(); }
	// SceneTextureのBufferIDを取得
	uint32_t GetSceneTextureBufferID() { return m_RenderTextures[SceneScreen].m_BufferIndex.value(); }
	// EffectEditTextureのBufferIDを取得
	uint32_t GetEffectEditTextureBufferID() { return m_RenderTextures[EffectEditScreen].m_BufferIndex.value(); }
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
	void SetRenderTargets(CommandContext* context,DrawPass pass,RenderMode mode = RenderMode::Game);
	// 描画設定コマンド
	void SetRenderState(CommandContext* context,ViewportType type);
	void DrawGBuffers(ResourceManager& resourceManager, GameCore& gameCore, RenderMode mode);
	void DrawLighting(ResourceManager& resourceManager, GameCore& gameCore, RenderMode mode);
	void DrawForward(ResourceManager& resourceManager, GameCore& gameCore, RenderMode mode);
	void DrawPostProcess(ResourceManager& resourceManager, GameCore& gameCore, RenderMode mode);
	// 深度バッファの生成
	void CreateDepthBuffer();
	// オフスクリーンバッファの生成
	void CreateOffscreenBuffer();
	// デバッグ用深度バッファの生成
	void CreateDebugDepthBuffer();

	// タイプごとに描画
	void DrawParticles(CommandContext* context, ResourceManager& resourceManager, GameCore& gameCore, RenderMode mode);
	void EffectEditorDraw(CommandContext* context, ResourceManager& resourceManager, GameCore& gameCore, RenderMode mode);
	void DrawUI(CommandContext* context, ResourceManager& resourceManager, GameCore& gameCore, RenderMode mode);

	ID3D12Device8* m_Device = nullptr;
	ResourceManager* m_ResourceManager = nullptr;
	EngineCommand* m_EngineCommand = nullptr;

	std::unique_ptr<SwapChain> m_SwapChain = nullptr;
	std::unique_ptr<GraphicsCore> m_GraphicsCore = nullptr;
	std::unique_ptr<DepthManager> m_DepthManager = nullptr;
	std::unique_ptr<PipelineManager> m_PipelineManager = nullptr;

	std::array<RenderTexture, RenderTextureType::RenderTextureTypeCount> m_RenderTextures;

	UINT64 m_ResolutionWidth = 1920;
	UINT m_ResolutionHeight = 1080;
};

