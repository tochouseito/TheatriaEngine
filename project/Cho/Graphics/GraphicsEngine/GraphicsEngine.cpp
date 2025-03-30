#include "pch.h"
#include "GraphicsEngine.h"
#include "OS/Windows/WinApp/WinApp.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Cho/SDK/ImGui/ImGuiManager/ImGuiManager.h"
#include "Cho/GameCore/GameCore.h"

void GraphicsEngine::Init()
{
	// DepthBufferの生成
	CreateDepthBuffer();
	// オフスクリーンレンダリング用のリソースを作成
	CreateOffscreenBuffer();
	// パイプラインの生成
	m_PipelineManager->Initialize(m_Device);
}

void GraphicsEngine::CreateSwapChain(IDXGIFactory7* dxgiFactory)
{
	// SwapChainの生成
	m_SwapChain = std::make_unique<SwapChain>(
		dxgiFactory,
		m_GraphicsCore->GetCommandQueue(QueueType::Graphics),
		WinApp::GetHWND(),
		WinApp::GetWindowWidth(),
		WinApp::GetWindowHeight()
	);
	// SwapChainのリソースでRTVを作成
	m_SwapChain->CreateResource(m_Device, m_ResourceManager);
}

void GraphicsEngine::PreRender()
{
	// コマンドマネージャー
	CommandManager* commandManager = m_GraphicsCore->GetCommandManager();
	// コマンドリストの取得
	CommandContext* context = commandManager->GetCommandContext();
	BeginCommandContext(context);
	// レンダーターゲットの設定
	ColorBuffer* offScreenTex = m_ResourceManager->GetBufferManager()->GetColorBuffer(m_RenderTextures[RenderTextureType::OffScreen].m_BufferID);
	context->BarrierTransition(
		offScreenTex->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	EndCommandContext(context,Graphics);
	// GPUの完了待ち
	WaitForGPU(Graphics);
}

void GraphicsEngine::Render(ResourceManager& resourceManager, GameCore& gameCore)
{
	resourceManager;
	gameCore;
	//// ディファードレンダリング
	//DrawGBuffers(resourceManager, gameCore);
	//// ライティング
	//DrawLighting(resourceManager, gameCore);
	//// フォワードレンダリング
	//DrawForward(resourceManager, gameCore);
	//// ポストプロセス
	//DrawPostProcess(resourceManager, gameCore);
}

void GraphicsEngine::PostRender()
{
	// コマンドマネージャー
	CommandManager* commandManager = m_GraphicsCore->GetCommandManager();
	// コマンドリストの取得
	CommandContext* context = commandManager->GetCommandContext();
	BeginCommandContext(context);
	// SwapChainのBackBufferIndexを取得
	UINT backBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
	// SwapChainResourceの状態遷移
	context->BarrierTransition(
		m_SwapChain->GetBuffer(backBufferIndex)->pResource.Get(),
		D3D12_RESOURCE_STATE_PRESENT, 
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	//// RTVの設定
	//SetRenderTargets(context, DrawPass::SwapChainPass);
	//SetRenderState(context);
	// レンダリング結果をスワップチェーンに描画
	ColorBuffer* offScreenTex = m_ResourceManager->GetBufferManager()->GetColorBuffer(m_RenderTextures[RenderTextureType::OffScreen].m_BufferID);
		context->BarrierTransition(
			offScreenTex->GetResource(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
	context->SetGraphicsPipelineState(m_PipelineManager->GetScreenCopyPSO().pso.Get());
	context->SetGraphicsRootSignature(m_PipelineManager->GetScreenCopyPSO().rootSignature.Get());
	context->SetGraphicsRootDescriptorTable(0, m_ResourceManager->GetSUVDHeap()->GetGpuHandle(offScreenTex->GetSUVHandleIndex()));
	context->DrawInstanced(3, 1, 0, 0);
	// SwapChainResourceの状態遷移
	context->BarrierTransition(
		m_SwapChain->GetBuffer(backBufferIndex)->pResource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	EndCommandContext(context,Graphics);
	// SwapChainのPresent
	m_SwapChain->Present();
	isOnce = true;
	// GPUの完了待ち
	WaitForGPU(Graphics);
}

void GraphicsEngine::PostRenderWithImGui(ImGuiManager* imgui)
{
	// コマンドマネージャー
	CommandManager* commandManager = m_GraphicsCore->GetCommandManager();
	// コマンドリストの取得
	CommandContext* context = commandManager->GetCommandContext();
	BeginCommandContext(context);
	// SwapChainのBackBufferIndexを取得
	UINT backBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
	// SwapChainResourceの状態遷移
	context->BarrierTransition(
		m_SwapChain->GetBuffer(backBufferIndex)->pResource.Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	// RTVの設定
	SetRenderTargets(context, DrawPass::SwapChainPass);
	SetRenderState(context);
	// ImGuiの描画
	imgui->Draw(context->GetCommandList());
	// SwapChainResourceの状態遷移
	context->BarrierTransition(
		m_SwapChain->GetBuffer(backBufferIndex)->pResource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	EndCommandContext(context,Graphics);
	// SwapChainのPresent
	m_SwapChain->Present();
	// GPUの完了待ち
	WaitForGPU(Graphics);
}

void GraphicsEngine::BeginCommandContext(CommandContext* context)
{
	// コマンドリストのリセット
	context->Reset();
	// ディスクリプタヒープの設定
	context->SetDescriptorHeap(m_ResourceManager->GetSUVDHeap()->GetDescriptorHeap());
}

void GraphicsEngine::EndCommandContext(CommandContext* context, const QueueType& queue)
{
	// コマンドリストのクローズ
	context->Close();
	// コマンドリストの実行
	m_GraphicsCore->GetCommandManager()->ExecuteCommandList(context->GetCommandList(), queue);
	// シグナル
	m_GraphicsCore->GetCommandManager()->Signal(queue);
	// コマンドリストの返却
	m_GraphicsCore->GetCommandManager()->ReturnCommandContext(context);
}

void GraphicsEngine::WaitForGPU(const QueueType& queue)
{
	// GPUの完了待ち
	m_GraphicsCore->GetCommandManager()->WaitForFence(queue);
}

void GraphicsEngine::SetRenderTargets(CommandContext* context, DrawPass pass)
{
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	switch (pass)
	{
	case GBuffers:
		// RTV,DSVの設定
		dsvHandle = m_ResourceManager->GetDSVDHeap()->GetCpuHandle(m_ResourceManager->GetBufferManager()->GetDepthBuffer(m_DepthManager->GetDepthBufferIndex())->GetDSVHandleIndex());
		rtvHandle = m_ResourceManager->GetRTVDHeap()->GetCpuHandle(m_ResourceManager->GetBufferManager()->GetColorBuffer(m_RenderTextures[OffScreen].m_BufferID)->GetRTVHandleIndex());
		context->SetRenderTarget(&rtvHandle, &dsvHandle);
		context->ClearRenderTarget(rtvHandle);
		context->ClearDepthStencil(dsvHandle);
		break;
	case Lighting:
		break;
	case Forward:
		break;
	case PostProcess:
		break;
	case SwapChainPass:
		// RTV,DSVの設定
		rtvHandle = m_ResourceManager->GetRTVDHeap()->GetCpuHandle(m_SwapChain->GetBuffer(m_SwapChain->GetCurrentBackBufferIndex())->dHIndex);
		context->SetRenderTarget(&rtvHandle);
		context->ClearRenderTarget(rtvHandle);
		break;
	case PassCount:
		// ここに来るはずがない
		ChoAssertLog("Invalid DrawPass", false, __FILE__, __LINE__);
		break;
	default:
		// ここに来るはずがない
		ChoAssertLog("Invalid DrawPass", false, __FILE__, __LINE__);
		break;
	}
}

void GraphicsEngine::SetRenderState(CommandContext* context)
{
	// ビューポートの設定
	D3D12_VIEWPORT viewport =
		D3D12_VIEWPORT(
			0.0f, 0.0f,
			static_cast<float>(WinApp::GetWindowWidth()),
			static_cast<float>(WinApp::GetWindowHeight()),
			0.0f, 1.0f
		);
	context->SetViewport(viewport);
	// シザーレクトの設定
	D3D12_RECT rect = D3D12_RECT(
		0, 0,
		WinApp::GetWindowWidth(),
		WinApp::GetWindowHeight()
	);
	context->SetScissorRect(rect);
	// プリミティブトポロジの設定
	context->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void GraphicsEngine::DrawGBuffers(ResourceManager& resourceManager, GameCore& gameCore)
{
	// コンテキスト取得
	//CommandContext* context = GetCommandContext();
	// コマンドリスト開始
	//BeginCommandContext(context);
	//// レンダーターゲットの設定
	//SetRenderTargets(context, DrawPass::GBuffers);
	//// 描画設定
	//SetRenderState(context);

	resourceManager;
	gameCore;
	// パイプラインセット
	// ルートシグネチャセット
	// 頂点バッファセット
	// インデックスバッファセット
	// シーンに含まれている頂点データを使用しているオブジェクトグループごとに
	//std::vector<bool> isUsedMesh = gameCore.GetSceneManager()->GetIntegrationBuffer()->GetUseVertexFlag();
	//for (uint32_t i = 0;i < isUsedMesh.size();i++)
	//{
	//	// 使用していないメッシュはスキップ
	//	if (!isUsedMesh[i]) { continue; }
	//	// VertexBufferを取得
	//	ModelData* modelData = resourceManager.GetModelManager()->GetModelData(i);
	//	D3D12_VERTEX_BUFFER_VIEW* vbv = resourceManager.GetBufferManager()->GetVertexBuffer(modelData->meshes[0].vertexBufferIndex)->GetVertexBufferView();
	//	D3D12_INDEX_BUFFER_VIEW* ibv = resourceManager.GetBufferManager()->GetVertexBuffer(modelData->meshes[0].vertexBufferIndex)->GetIndexBufferView();
	//	// 一旦最初のメッシュのみを使用
	//	context->SetVertexBuffers(0, 1, vbv);
	//	context->SetIndexBuffer(ibv);
	//	// 使用するトランスフォームSRVのインデックスをセット
	//	StructuredBuffer* transformIndexBuffer = resourceManager.GetBufferManager()->GetStructuredBuffer(gameCore.GetSceneManager()->GetIntegrationBuffer()->GetTransformIndexBufferIndex());
	//	context->SetGraphicsRootDescriptorTable(2, resourceManager.GetSUVDHeap()->GetGpuHandle(transformIndexBuffer->GetSUVHandleIndex()));
	//}
	//// カメラセット
	//// MainCameraを取得
	//uint32_t cameraEntity = gameCore.GetSceneManager()->GetCurrentScene()->GetMainCameraID();
	//CameraComponent* camera = gameCore.GetSceneManager()->GetECSManager()->GetComponent<CameraComponent>(cameraEntity);
	//ConstantBuffer* cameraBuffer = resourceManager.GetBufferManager()->GetConstantBuffer(camera->bufferIndex);
	//context->SetGraphicsRootConstantBufferView(0, cameraBuffer->GetResource()->GetGPUVirtualAddress());
	//// トランスフォームセット
	//StructuredBuffer* transformBuffer = resourceManager.GetBufferManager()->GetStructuredBuffer(gameCore.GetSceneManager()->GetIntegrationBuffer()->GetTransformBufferIndex());
	//context->SetGraphicsRootDescriptorTable(1, resourceManager.GetSUVDHeap()->GetGpuHandle(transformBuffer->GetSUVHandleIndex()));

	// コマンドリスト終了
	//EndCommandContext(context,Graphics);
	// GPUの完了待ち
	//WaitForGPU(Graphics);
}

void GraphicsEngine::DrawLighting(ResourceManager& resourceManager, GameCore& gameCore)
{
	resourceManager;
	gameCore;
}

void GraphicsEngine::DrawForward(ResourceManager& resourceManager, GameCore& gameCore)
{
	resourceManager;
	gameCore;
}

void GraphicsEngine::DrawPostProcess(ResourceManager& resourceManager, GameCore& gameCore)
{
	resourceManager;
	gameCore;
}

void GraphicsEngine::CreateDepthBuffer()
{
	// DepthBufferの生成
	// DepthBufferのリソースを作成
	BUFFER_DEPTH_DESC desc = {};
	desc.width = WinApp::GetWindowWidth();
	desc.height = WinApp::GetWindowHeight();
	desc.format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	// DepthBufferのリソースを作成
	m_DepthManager->SetDepthBufferIndex(m_ResourceManager->CreateDepthBuffer(desc));
}

void GraphicsEngine::CreateOffscreenBuffer()
{
	BUFFER_COLOR_DESC desc = {};
	desc.width = WinApp::GetWindowWidth();
	desc.height = WinApp::GetWindowHeight();
	desc.format = PixelFormat;
	desc.state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	m_RenderTextures[OffScreen].m_BufferID = m_ResourceManager->CreateColorBuffer(desc);
	m_RenderTextures[OffScreen].m_Width = desc.width;
	m_RenderTextures[OffScreen].m_Height = desc.height;
}
