#include "pch.h"
#include "GraphicsEngine.h"
#include "OS/Windows/WinApp/WinApp.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "EngineCommand/EngineCommand.h"
#include "SDK/ImGui/ImGuiManager/ImGuiManager.h"
#include "GameCore/GameCore.h"
#include "Core/ChoLog/ChoLog.h"
#include "SDK/ImGui/ImGuiUtlity/ImGuiUtlity.h"
using namespace Cho;

void GraphicsEngine::Init()
{
	// DepthBufferの生成
	CreateDepthBuffer();
	// オフスクリーンレンダリング用のリソースを作成
	CreateOffscreenBuffer();
	// デバッグ用のリソースを作成
	CreateDebugDepthBuffer();
	//CreateDebugOffscreenBuffer();
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
	//// コマンドマネージャー
	//CommandManager* commandManager = m_GraphicsCore->GetCommandManager();
	//// コマンドリストの取得
	//CommandContext* context = commandManager->GetCommandContext();
	//BeginCommandContext(context);
	//EndCommandContext(context,Graphics);
	//// GPUの完了待ち
	//WaitForGPU(Graphics);
}

void GraphicsEngine::Render(ResourceManager& resourceManager, GameCore& gameCore,RenderMode mode)
{
	// ディファードレンダリング
	DrawGBuffers(resourceManager, gameCore, mode);
	// ライティング
	DrawLighting(resourceManager, gameCore, mode);
	// フォワードレンダリング
	DrawForward(resourceManager, gameCore, mode);
	// ポストプロセス
	//DrawPostProcess(resourceManager, gameCore, mode);
}

void GraphicsEngine::PostRender(ImGuiManager* imgui, RenderMode mode)
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
	SetRenderTargets(context, DrawPass::SwapChainPass, mode);
	if (mode != RenderMode::Release)
	{
		// ImGuiの描画
		imgui->Draw(context->GetCommandList());
	}
	// SwapChainResourceの状態遷移
	context->BarrierTransition(
		m_SwapChain->GetBuffer(backBufferIndex)->pResource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	// コマンドリストのクローズ
	context->Close();
	// コマンドリストの実行
	m_GraphicsCore->GetCommandManager()->ExecuteCommandList(context->GetCommandList(), Graphics);
	// SwapChainのPresent
	m_SwapChain->Present();
	// シグナル
	m_GraphicsCore->GetCommandManager()->Signal(Graphics);
	// コマンドリストの返却
	m_GraphicsCore->GetCommandManager()->ReturnCommandContext(context);
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
	SetRenderState(context,ViewportSwapChain);
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

void GraphicsEngine::ScreenResize()
{
	// GPUの完了待ち
	WaitForGPU(Graphics);
	WaitForGPU(Compute);
	WaitForGPU(Copy);
	// ImGuiのリサイズ
	ImGuiUtility::DisplayResize(static_cast<float>(WinApp::GetWindowWidth()), static_cast<float>(WinApp::GetWindowHeight()));
	// SwapChainのリサイズ
	m_SwapChain->Resize(m_Device, WinApp::GetWindowWidth(), WinApp::GetWindowHeight());
	// DepthBufferのリサイズ
	m_DepthManager->ResizeDepthBuffer(m_Device, m_ResourceManager, m_ResolutionWidth, m_ResolutionHeight);
	// オフスクリーンレンダリング用のリソースをリサイズ
	// オフスクリーンレンダリング用のリソースを作成
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Width = m_ResolutionWidth;
	resourceDesc.Height = m_ResolutionHeight;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Format = PixelFormat;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	// クリア値の設定
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = PixelFormat;
	clearValue.Color[0] = kClearColor[0];
	clearValue.Color[1] = kClearColor[1];
	clearValue.Color[2] = kClearColor[2];
	clearValue.Color[3] = kClearColor[3];
	for (int i =0 ; i<static_cast<int>(RenderTextureType::RenderTextureTypeCount);i++)
	{
		RenderTextureType renderTexType = static_cast<RenderTextureType>(i);
		switch (renderTexType)
		{
		case GameScreen:
			// リサイズ
			m_ResourceManager->RemakeColorBuffer(m_RenderTextures[renderTexType].m_BufferIndex, resourceDesc, &clearValue, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			m_RenderTextures[renderTexType].m_Width = m_ResolutionWidth;
			m_RenderTextures[renderTexType].m_Height = m_ResolutionHeight;
			break;
		case SceneScreen:
			// リサイズ
			m_ResourceManager->RemakeColorBuffer(m_RenderTextures[renderTexType].m_BufferIndex, resourceDesc, &clearValue, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			m_RenderTextures[renderTexType].m_Width = m_ResolutionWidth;
			m_RenderTextures[renderTexType].m_Height = m_ResolutionHeight;
			break;
		case RenderTextureTypeCount:
			break;
		default:
			break;
		}
	}
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

void GraphicsEngine::SetRenderTargets(CommandContext* context, DrawPass pass, RenderMode mode)
{
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	RenderTextureType renderTexType = RenderTextureType::SceneScreen;
	ColorBuffer* targetTex = nullptr;
	ColorBuffer* setTex = nullptr;
	switch (pass)
	{
	case GBuffers:
		// レンダリングモードでシーン描画かどうかを判定
		if (mode == RenderMode::Game)
		{
			renderTexType = RenderTextureType::GameScreen;
		} else if(mode==RenderMode::Debug)
		{
			renderTexType = RenderTextureType::SceneScreen;
		} else if (mode == RenderMode::Editor)
		{
			renderTexType = RenderTextureType::EffectEditScreen;
		} else if (mode == RenderMode::Release)
		{
			renderTexType = RenderTextureType::GameScreen;
		} else
		{
			break;
		}
		// GBufferRenderTextureの状態遷移
		targetTex = m_ResourceManager->GetBuffer<ColorBuffer>(m_RenderTextures[renderTexType].m_BufferIndex);
		context->BarrierTransition(
			targetTex->GetResource(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		// RTV,DSVの設定
		dsvHandle = m_ResourceManager->GetBuffer<DepthBuffer>(m_DepthManager->GetDepthBufferIndex())->GetDSVCpuHandle();
		rtvHandle = m_ResourceManager->GetBuffer<ColorBuffer>(m_RenderTextures[renderTexType].m_BufferIndex)->GetRTVCpuHandle();
		context->SetRenderTarget(&rtvHandle, &dsvHandle);
		context->ClearRenderTarget(rtvHandle);
		context->ClearDepthStencil(dsvHandle);
		break;
	case Lighting:
		// レンダリングモードでシーン描画かどうかを判定
		if (mode == RenderMode::Game)
		{
			renderTexType = RenderTextureType::GameScreen;
		} else if (mode == RenderMode::Debug)
		{
			renderTexType = RenderTextureType::SceneScreen;
		} else if (mode == RenderMode::Editor)
		{
			renderTexType = RenderTextureType::EffectEditScreen;
		} else
		{
			break;
		}
		// オフスクリーンレンダリングテクスチャの状態遷移
		setTex = m_ResourceManager->GetBuffer<ColorBuffer>(m_RenderTextures[renderTexType].m_BufferIndex);
		context->BarrierTransition(
			setTex->GetResource(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
		break;
	case Forward:
		break;
	case PostProcess:
		// レンダリングモードでシーン描画かどうかを判定
		if (mode == RenderMode::Game)
		{
			renderTexType = RenderTextureType::PostProcessScreen;
		} else if (mode == RenderMode::Debug)
		{
			renderTexType = RenderTextureType::ScenePostProcessScreen;
		} else if (mode == RenderMode::Editor)
		{
			break;
		} else
		{
			break;
		}
		// ポストプロセス用のテクスチャの状態遷移
		setTex = m_ResourceManager->GetBuffer<ColorBuffer>(m_RenderTextures[renderTexType].m_BufferIndex);
		context->BarrierTransition(
			setTex->GetResource(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		// RTVの設定
		renderTexType = RenderTextureType::PostProcessScreen;
		rtvHandle = m_ResourceManager->GetBuffer<ColorBuffer>(m_RenderTextures[renderTexType].m_BufferIndex)->GetRTVCpuHandle();
		context->SetRenderTarget(&rtvHandle);
		context->ClearRenderTarget(rtvHandle);
		SetRenderState(context, ViewportSwapChain);
		break;
	case SwapChainPass:
		// RTV,DSVの設定
		rtvHandle = m_SwapChain->GetBuffer(m_SwapChain->GetCurrentBackBufferIndex())->m_RTVCpuHandle;
		context->SetRenderTarget(&rtvHandle);
		context->ClearRenderTarget(rtvHandle);
		SetRenderState(context,ViewportSwapChain);
		// リリースモードでスワップチェーンに描画
		if (mode == RenderMode::Release)
		{
			renderTexType = RenderTextureType::GameScreen;
			setTex = m_ResourceManager->GetBuffer<ColorBuffer>(m_RenderTextures[renderTexType].m_BufferIndex);
			context->BarrierTransition(
				setTex->GetResource(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);
			// パイプラインセット
			context->SetGraphicsPipelineState(m_PipelineManager->GetScreenCopyPSO().pso.Get());
			// ルートシグネチャセット
			context->SetGraphicsRootSignature(m_PipelineManager->GetScreenCopyPSO().rootSignature.Get());
			// オフスクリーンレンダリングテクスチャをセット
			context->SetGraphicsRootDescriptorTable(0, setTex->GetSRVGpuHandle());
			// DrawCall
			context->DrawInstanced(3, 1, 0, 0);
		}
		break;
	case PassCount:
		// ここに来るはずがない
		Log::Write(LogLevel::Assert, "Invalid DrawPass");
		break;
	default:
		// ここに来るはずがない
		Log::Write(LogLevel::Assert, "Invalid DrawPass");
		break;
	}
}

void GraphicsEngine::SetRenderState(CommandContext* context, ViewportType type)
{
	D3D12_VIEWPORT viewport;
	D3D12_RECT rect;
	switch (type)
	{
	case ViewportGame:
		// ビューポートの設定
		viewport = D3D12_VIEWPORT(
			0.0f, 0.0f,
			static_cast<float>(m_ResolutionWidth),
			static_cast<float>(m_ResolutionHeight),
			0.0f, 1.0f);
		// シザーレクトの設定
		rect = D3D12_RECT(
			0, 0,
			static_cast<LONG>(m_ResolutionWidth),
			static_cast<LONG>(m_ResolutionHeight));
		break;
	case ViewportSwapChain:
		// ビューポートの設定
		viewport = D3D12_VIEWPORT(
				0.0f, 0.0f,
				static_cast<float>(WinApp::GetWindowWidth()),
				static_cast<float>(WinApp::GetWindowHeight()),
				0.0f, 1.0f);
		// シザーレクトの設定
		rect = D3D12_RECT(
			0, 0,
			static_cast<LONG>(WinApp::GetWindowWidth()),
			static_cast<LONG>(WinApp::GetWindowHeight()));
		break;
	default:
		break;
	}
	context->SetViewport(viewport);
	context->SetScissorRect(rect);
	// プリミティブトポロジの設定
	context->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void GraphicsEngine::DrawGBuffers(ResourceManager& resourceManager, GameCore& gameCore, RenderMode mode)
{
	// コンテキスト取得
	CommandContext* context = GetCommandContext();
	// コマンドリスト開始
	BeginCommandContext(context);
	// レンダーターゲットの設定
	SetRenderTargets(context, DrawPass::GBuffers,mode);
	// 描画設定
	SetRenderState(context,ViewportGame);
	if (mode != RenderMode::Editor)
	{
		// パイプラインセット
		context->SetGraphicsPipelineState(m_PipelineManager->GetIntegratePSO().pso.Get());
		// ルートシグネチャセット
		context->SetGraphicsRootSignature(m_PipelineManager->GetIntegratePSO().rootSignature.Get());
		// シーンに存在するメッシュを所持しているオブジェクトを全て描画
		for (ModelData& modelData : resourceManager.GetModelManager()->GetModelDataContainer())
		{
			// シーンがないならスキップ
			if (!gameCore.GetSceneManager()->GetMainScene()) { continue; }
			IConstantBuffer* cameraBuffer = nullptr;
			// メインカメラを取得
			if (mode == RenderMode::Game||mode == RenderMode::Release)
			{
				// カメラオブジェクトのIDを取得
				std::optional<uint32_t> cameraID = gameCore.GetSceneManager()->GetMainScene()->GetMainCameraID();
				if (!cameraID) { continue; }
				// カメラオブジェクトを取得
				GameObject& cameraObject = gameCore.GetObjectContainer()->GetGameObject(cameraID.value());
				if (!cameraObject.IsActive()) { continue; }
				// カメラのバッファインデックスを取得
				CameraComponent* cameraComponent = gameCore.GetECSManager()->GetComponent<CameraComponent>(cameraObject.GetEntity());
				if (!cameraComponent) { continue; }
				// カメラのバッファを取得
				cameraBuffer = resourceManager.GetBuffer<IConstantBuffer>(cameraComponent->bufferIndex);
				// カメラがないならスキップ
				if (!cameraBuffer) { continue; }
			} else// デバッグカメラ
			{
				// カメラのバッファを取得
				cameraBuffer = resourceManager.GetDebugCameraBuffer();
				// カメラがないならスキップ
				if (!cameraBuffer) { continue; }
			}
			// 登録されているTransformがないならスキップ
			if (modelData.useTransformList.empty()) { continue; }
			// 頂点バッファビューをセット
			D3D12_VERTEX_BUFFER_VIEW* vbv = resourceManager.GetBuffer<IVertexBuffer>(modelData.meshes[0].vertexBufferIndex)->GetVertexBufferView();
			context->SetVertexBuffers(0, 1, vbv);
			// インデックスバッファビューをセット
			D3D12_INDEX_BUFFER_VIEW* ibv = resourceManager.GetBuffer<IIndexBuffer>(modelData.meshes[0].indexBufferIndex)->GetIndexBufferView();
			context->SetIndexBuffer(ibv);
			// カメラバッファをセット
			context->SetGraphicsRootConstantBufferView(0, cameraBuffer->GetResource()->GetGPUVirtualAddress());
			// トランスフォーム統合バッファをセット
			IStructuredBuffer* transformBuffer = resourceManager.GetIntegrationBuffer(IntegrationDataType::Transform);
			context->SetGraphicsRootDescriptorTable(1, transformBuffer->GetSRVGpuHandle());
			// UseTransformBufferをセット
			IStructuredBuffer* useTransformBuffer = resourceManager.GetBuffer<IStructuredBuffer>(modelData.useTransformBufferIndex);
			context->SetGraphicsRootDescriptorTable(2, useTransformBuffer->GetSRVGpuHandle());
			// モデルのボーン行列Bufferをセット
			IStructuredBuffer* boneBuffer = resourceManager.GetBuffer<IStructuredBuffer>(modelData.boneMatrixBufferIndex);
			context->SetGraphicsRootDescriptorTable(3, boneBuffer->GetSRVGpuHandle());
			// SkinningInfluenceBufferをセット
			IStructuredBuffer* skinningInfluenceBuffer = resourceManager.GetBuffer<IStructuredBuffer>(modelData.influenceBufferIndex);
			context->SetGraphicsRootDescriptorTable(4, skinningInfluenceBuffer->GetSRVGpuHandle());
			// SkinningInfoBufferをセット
			IConstantBuffer* skinningInfoBuffer = resourceManager.GetBuffer<IConstantBuffer>(modelData.skinInfoBufferIndex);
			context->SetGraphicsRootConstantBufferView(5, skinningInfoBuffer->GetResource()->GetGPUVirtualAddress());
			// ライトバッファをセット
			IConstantBuffer* lightBuffer = resourceManager.GetLightBuffer();
			context->SetGraphicsRootConstantBufferView(6, lightBuffer->GetResource()->GetGPUVirtualAddress());
			// 環境情報バッファをセット
			IConstantBuffer* envBuffer = resourceManager.GetEnvironmentBuffer();
			context->SetGraphicsRootConstantBufferView(7, envBuffer->GetResource()->GetGPUVirtualAddress());
			// PS用トランスフォーム統合バッファをセット
			context->SetGraphicsRootDescriptorTable(8, transformBuffer->GetSRVGpuHandle());
			// マテリアル統合バッファをセット
			IStructuredBuffer* materialBuffer = resourceManager.GetIntegrationBuffer(IntegrationDataType::Material);
			context->SetGraphicsRootDescriptorTable(9, materialBuffer->GetSRVGpuHandle());
			// 配列テクスチャのためヒープをセット
			context->SetGraphicsRootDescriptorTable(10, resourceManager.GetSUVDHeap()->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
			// インスタンス数を取得
			UINT numInstance = static_cast<UINT>(modelData.useTransformList.size());
			// DrawCall
			context->DrawIndexedInstanced(static_cast<UINT>(modelData.meshes[0].indices.size()), numInstance, 0, 0, 0);
		}
		// ラインの描画
		for (uint32_t i = 0;i < 1;i++)
		{
			// プリミティブトポロジの設定
			context->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
			// パイプラインセット
			context->SetGraphicsPipelineState(m_PipelineManager->GetLinePSO().pso.Get());
			// ルートシグネチャセット
			context->SetGraphicsRootSignature(m_PipelineManager->GetLinePSO().rootSignature.Get());
			// シーンがないならスキップ
			if (!gameCore.GetSceneManager()->GetMainScene()) { continue; }
			IConstantBuffer* cameraBuffer = nullptr;
			// メインカメラを取得
			if (mode == RenderMode::Game||mode==RenderMode::Release)
			{
				// カメラオブジェクトのIDを取得
				std::optional<uint32_t> cameraID = gameCore.GetSceneManager()->GetMainScene()->GetMainCameraID();
				if (!cameraID) { continue; }
				// カメラオブジェクトを取得
				GameObject& cameraObject = gameCore.GetObjectContainer()->GetGameObject(cameraID.value());
				if (!cameraObject.IsActive()) { continue; }
				// カメラのバッファインデックスを取得
				CameraComponent* cameraComponent = gameCore.GetECSManager()->GetComponent<CameraComponent>(cameraObject.GetEntity());
				if (!cameraComponent) { continue; }
				// カメラのバッファを取得
				cameraBuffer = resourceManager.GetBuffer<IConstantBuffer>(cameraComponent->bufferIndex);
				// カメラがないならスキップ
				if (!cameraBuffer) { continue; }
			} else// デバッグカメラ
			{
				// カメラのバッファを取得
				cameraBuffer = resourceManager.GetDebugCameraBuffer();
				// カメラがないならスキップ
				if (!cameraBuffer) { continue; }
			}
			// MapIDが0（使われていない）ならスキップ
			if (!resourceManager.GetIntegrationData(IntegrationDataType::Line)->GetActiveCount()) { continue; }
			// 頂点バッファビューをセット
			D3D12_VERTEX_BUFFER_VIEW* vbv = resourceManager.GetLineIntegrationBuffer()->GetVertexBufferView();
			context->SetVertexBuffers(0, 1, vbv);
			// カメラバッファをセット
			context->SetGraphicsRootConstantBufferView(0, cameraBuffer->GetResource()->GetGPUVirtualAddress());
			// DrawCall
			context->DrawInstanced(static_cast<UINT>(resourceManager.GetIntegrationData(IntegrationDataType::Line)->GetActiveCount()) * 2, 1, 0, 0);
		}
		// パーティクル
		DrawParticles(context, resourceManager, gameCore, mode);
		// EffectEditor
		EffectEditorDraw(context, resourceManager, gameCore, mode);
		// UI
		DrawUI(context, resourceManager, gameCore, mode);
	} else
	{
		// パイプラインセット
		
		// ルートシグネチャセット
		
		// カメラのバッファを取得
		IConstantBuffer* cameraBuffer = nullptr;
		// カメラのバッファを取得
		cameraBuffer = resourceManager.GetDebugCameraBuffer();
		// カメラがあったら処理する
		if (cameraBuffer)
		{
			// 頂点バッファビューをセット

			// インデックスバッファビューをセット
			
			// カメラバッファをセット

			// リソースのバインド

			// DrawCall

		}
	}
	
	// コマンドリスト終了
	EndCommandContext(context,Graphics);
	// GPUの完了待ち
	WaitForGPU(Graphics);
}

void GraphicsEngine::DrawLighting(ResourceManager& resourceManager, GameCore& gameCore, RenderMode mode)
{
	resourceManager;
	gameCore;
	if (mode == Release)
	{
		return;
	}
	// コンテキスト取得
	CommandContext* context = GetCommandContext();
	// コマンドリスト開始
	BeginCommandContext(context);
	// レンダーターゲットの設定
	SetRenderTargets(context, DrawPass::Lighting,mode);
	// 描画設定
	SetRenderState(context,ViewportGame);
	// コマンドリスト終了
	EndCommandContext(context, Graphics);
	// GPUの完了待ち
	WaitForGPU(Graphics);
}

void GraphicsEngine::DrawForward(ResourceManager& resourceManager, GameCore& gameCore, RenderMode mode)
{
	resourceManager;
	gameCore;
	mode;
}

void GraphicsEngine::DrawPostProcess(ResourceManager& resourceManager, GameCore& gameCore, RenderMode mode)
{
	// コンテキスト取得
	CommandContext* context = GetCommandContext();
	// コマンドリスト開始
	BeginCommandContext(context);
	// レンダーターゲットの設定
	SetRenderTargets(context, DrawPass::PostProcess, mode);
	// 描画設定
	SetRenderState(context, ViewportGame);
	if (mode != RenderMode::Editor)
	{
		// プリミティブトポロジの設定
		context->SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// パイプラインセット
		context->SetGraphicsPipelineState(m_PipelineManager->GetIntegratePSO().pso.Get());
		// ルートシグネチャセット
		context->SetGraphicsRootSignature(m_PipelineManager->GetIntegratePSO().rootSignature.Get());
		// シーンがないならスキップ
		if (!gameCore.GetSceneManager()->GetMainScene()) { return; }
		IConstantBuffer* cameraBuffer = nullptr;
		// メインカメラを取得
		if (mode == RenderMode::Game)
		{
			// カメラオブジェクトのIDを取得
			std::optional<uint32_t> cameraID = gameCore.GetSceneManager()->GetMainScene()->GetMainCameraID();
			if (!cameraID) { return; }
			// カメラオブジェクトを取得
			GameObject& cameraObject = gameCore.GetObjectContainer()->GetGameObject(cameraID.value());
			if (!cameraObject.IsActive()) { return; }
			// カメラのバッファインデックスを取得
			CameraComponent* cameraComponent = gameCore.GetECSManager()->GetComponent<CameraComponent>(cameraObject.GetEntity());
			if (!cameraComponent) { return; }
			// カメラのバッファを取得
			cameraBuffer = resourceManager.GetBuffer<IConstantBuffer>(cameraComponent->bufferIndex);
			// カメラがないならスキップ
			if (!cameraBuffer) { return; }
		} else// デバッグカメラ
		{
			// カメラのバッファを取得
			cameraBuffer = resourceManager.GetDebugCameraBuffer();
			// カメラがないならスキップ
			if (!cameraBuffer) { return; }
		}
		// オフスクリーンレンダリングテクスチャをセット
		//context->SetGraphicsRootDescriptorTable(0, m_ResourceManager->GetBuffer<ColorBuffer>(m_RenderTextures[renderTexType].m_BufferIndex)->GetSRVGpuHandle());
		// DrawCall
		//context->DrawInstanced(3, 1, 0, 0);
	}

	// コマンドリスト終了
	EndCommandContext(context, Graphics);
	// GPUの完了待ち
	WaitForGPU(Graphics);
}

void GraphicsEngine::CreateDepthBuffer()
{
	// DepthBufferの生成
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Width = m_ResolutionWidth;
	resourceDesc.Height = m_ResolutionHeight;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	m_DepthManager->SetDepthBufferIndex(m_ResourceManager->CreateDepthBuffer(resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE));
}

void GraphicsEngine::CreateOffscreenBuffer()
{
	// オフスクリーンレンダリング用のリソースを作成
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Width = m_ResolutionWidth;
	resourceDesc.Height = m_ResolutionHeight;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Format = PixelFormat;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	// クリア値の設定
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = PixelFormat;
	clearValue.Color[0] = kClearColor[0];
	clearValue.Color[1] = kClearColor[1];
	clearValue.Color[2] = kClearColor[2];
	clearValue.Color[3] = kClearColor[3];
	for (int i = 0; i < static_cast<int>(RenderTextureType::RenderTextureTypeCount); i++)
	{
		RenderTextureType renderTexType = static_cast<RenderTextureType>(i);
		m_RenderTextures[renderTexType].m_BufferIndex = m_ResourceManager->CreateColorBuffer(resourceDesc, &clearValue, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		m_RenderTextures[renderTexType].m_Width = resourceDesc.Width;
		m_RenderTextures[renderTexType].m_Height = resourceDesc.Height;
	}
}

void GraphicsEngine::CreateDebugDepthBuffer()
{
	// DepthBufferの生成
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Width = m_ResolutionWidth;
	resourceDesc.Height = m_ResolutionHeight;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	// デバッグ用DepthBufferの生成
	m_DepthManager->SetDebugDepthBufferIndex(m_ResourceManager->CreateDepthBuffer(resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE));
}

void GraphicsEngine::DrawParticles(CommandContext* context, ResourceManager& resourceManager, GameCore& gameCore, RenderMode mode)
{
	// プリミティブトポロジの設定
	context->SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// パイプラインセット
	context->SetGraphicsPipelineState(m_PipelineManager->GetParticlePSO().pso.Get());
	// ルートシグネチャセット
	context->SetGraphicsRootSignature(m_PipelineManager->GetParticlePSO().rootSignature.Get());

	for (auto& object : gameCore.GetObjectContainer()->GetGameObjects().GetVector())
	{
		if (!object->IsActive()) { continue; }
		if (object->GetType() != ObjectType::ParticleSystem) { continue; }
		// EmitterComponentを取得
		EmitterComponent* emitterComponent = gameCore.GetECSManager()->GetComponent<EmitterComponent>(object->GetEntity());
		if (!emitterComponent) { continue; }
		//if (!emitterComponent->particleID) { continue; }
		// ParticleComponentを取得
		ParticleComponent* particleComponent = gameCore.GetECSManager()->GetComponent<ParticleComponent>(object->GetEntity());
		if (!particleComponent) { continue; }
		MeshFilterComponent* meshFilterComponent = gameCore.GetECSManager()->GetComponent<MeshFilterComponent>(object->GetEntity());
		if (!meshFilterComponent) { continue; }
		MeshRendererComponent* meshRendererComponent = gameCore.GetECSManager()->GetComponent<MeshRendererComponent>(object->GetEntity());
		if (!meshRendererComponent) { continue; }
		// シーンがないならスキップ
		if (!gameCore.GetSceneManager()->GetMainScene()) { continue; }
		IConstantBuffer* cameraBuffer = nullptr;
		// メインカメラを取得
		if (mode == RenderMode::Game)
		{
			// カメラオブジェクトのIDを取得
			std::optional<uint32_t> cameraID = gameCore.GetSceneManager()->GetMainScene()->GetMainCameraID();
			if (!cameraID) { continue; }
			// カメラオブジェクトを取得
			GameObject& cameraObject = gameCore.GetObjectContainer()->GetGameObject(cameraID.value());
			if (!cameraObject.IsActive()) { continue; }
			// カメラのバッファインデックスを取得
			CameraComponent* cameraComponent = gameCore.GetECSManager()->GetComponent<CameraComponent>(cameraObject.GetEntity());
			if (!cameraComponent) { continue; }
			// カメラのバッファを取得
			cameraBuffer = resourceManager.GetBuffer<IConstantBuffer>(cameraComponent->bufferIndex);
			// カメラがないならスキップ
			if (!cameraBuffer) { continue; }
		} else// デバッグカメラ
		{
			// カメラのバッファを取得
			cameraBuffer = resourceManager.GetDebugCameraBuffer();
			// カメラがないならスキップ
			if (!cameraBuffer) { continue; }
		}
		// 
		ModelData* modelData = resourceManager.GetModelManager()->GetModelData(meshFilterComponent->modelName);
		if (!modelData) { break; }
		// VBVをセット
		D3D12_VERTEX_BUFFER_VIEW* vbv = resourceManager.GetBuffer<IVertexBuffer>(modelData->meshes[0].vertexBufferIndex)->GetVertexBufferView();
		context->SetVertexBuffers(0, 1, vbv);
		// IBVをセット
		D3D12_INDEX_BUFFER_VIEW* ibv = resourceManager.GetBuffer<IIndexBuffer>(modelData->meshes[0].indexBufferIndex)->GetIndexBufferView();
		context->SetIndexBuffer(ibv);
		// カメラバッファをセット
		context->SetGraphicsRootConstantBufferView(0, cameraBuffer->GetResource()->GetGPUVirtualAddress());
		// パーティクルバッファをセット
		IRWStructuredBuffer* particleBuffer = resourceManager.GetBuffer<IRWStructuredBuffer>(particleComponent->bufferIndex);
		context->SetGraphicsRootDescriptorTable(1, particleBuffer->GetUAVGpuHandle());
		// マテリアル統合バッファをセット
		IStructuredBuffer* materialBuffer = resourceManager.GetIntegrationBuffer(IntegrationDataType::Material);
		context->SetGraphicsRootDescriptorTable(2, materialBuffer->GetSRVGpuHandle());
		// 配列テクスチャのためヒープをセット
		context->SetGraphicsRootDescriptorTable(3, resourceManager.GetSUVDHeap()->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
		// DrawCall
		context->DrawIndexedInstanced(static_cast<UINT>(modelData->meshes[0].indices.size()), particleComponent->count, 0, 0, 0);
	}
}

void GraphicsEngine::EffectEditorDraw(CommandContext* context, ResourceManager& resourceManager, GameCore& gameCore, RenderMode mode)
{
	resourceManager;
	gameCore;
	mode;
	// GPU待機
	WaitForGPU(Compute);
	// レンダーターゲットの設定
	//SetRenderTargets(context, DrawPass::GBuffers, RenderMode::Editor);
	// 描画設定
	//SetRenderState(context, ViewportGame);
	// プリミティブトポロジの設定
	context->SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// パイプラインセット
	context->SetGraphicsPipelineState(m_PipelineManager->GetEffectSpritePSO().pso.Get());
	// ルートシグネチャセット
	context->SetGraphicsRootSignature(m_PipelineManager->GetEffectSpritePSO().rootSignature.Get());
	// シーンがないならスキップ
	if (!gameCore.GetSceneManager()->GetMainScene()) { return; }
	IConstantBuffer* cameraBuffer = nullptr;
	// メインカメラを取得
	if (mode == RenderMode::Game)
	{
		// カメラオブジェクトのIDを取得
		std::optional<uint32_t> cameraID = gameCore.GetSceneManager()->GetMainScene()->GetMainCameraID();
		if (!cameraID) { return; }
		// カメラオブジェクトを取得
		GameObject& cameraObject = gameCore.GetObjectContainer()->GetGameObject(cameraID.value());
		if (!cameraObject.IsActive()) { return; }
		// カメラのバッファインデックスを取得
		CameraComponent* cameraComponent = gameCore.GetECSManager()->GetComponent<CameraComponent>(cameraObject.GetEntity());
		if (!cameraComponent) { return; }
		// カメラのバッファを取得
		cameraBuffer = resourceManager.GetBuffer<IConstantBuffer>(cameraComponent->bufferIndex);
		// カメラがないならスキップ
		if (!cameraBuffer) { return; }
	} else// デバッグカメラ
	{
		// カメラのバッファを取得
		cameraBuffer = resourceManager.GetDebugCameraBuffer();
		// カメラがないならスキップ
		if (!cameraBuffer) { return; }
	}
	// 板ポリ取得
	ModelData* modelData = resourceManager.GetModelManager()->GetModelData(L"Plane");
	if (!modelData) { return; }
	// VBVをセット
	D3D12_VERTEX_BUFFER_VIEW* vbv = resourceManager.GetBuffer<IVertexBuffer>(modelData->meshes[0].vertexBufferIndex)->GetVertexBufferView();
	context->SetVertexBuffers(0, 1, vbv);
	// IBVをセット
	D3D12_INDEX_BUFFER_VIEW* ibv = resourceManager.GetBuffer<IIndexBuffer>(modelData->meshes[0].indexBufferIndex)->GetIndexBufferView();
	context->SetIndexBuffer(ibv);
	// カメラバッファをセット
	context->SetGraphicsRootConstantBufferView(0, cameraBuffer->GetResource()->GetGPUVirtualAddress());
	// RootBufferをセット
	IStructuredBuffer* rootBuffer = resourceManager.GetIntegrationBuffer(IntegrationDataType::EffectRootInt);
	context->SetGraphicsRootDescriptorTable(1, rootBuffer->GetSRVGpuHandle());
	// NodeBufferをセット
	IStructuredBuffer* nodeBuffer = resourceManager.GetIntegrationBuffer(IntegrationDataType::EffectNodeInt);
	context->SetGraphicsRootDescriptorTable(2, nodeBuffer->GetSRVGpuHandle());
	// SpriteBufferをセット
	IStructuredBuffer* spriteBuffer = resourceManager.GetIntegrationBuffer(IntegrationDataType::EffectSpriteInt);
	context->SetGraphicsRootDescriptorTable(3, spriteBuffer->GetSRVGpuHandle());
	// EffectParticleBufferをセット
	IRWStructuredBuffer* effectParticleBuffer = resourceManager.GetEffectParticleBuffer();
	context->SetGraphicsRootDescriptorTable(4, effectParticleBuffer->GetUAVGpuHandle());
	// NodeBufferをセット
	context->SetGraphicsRootDescriptorTable(5, nodeBuffer->GetSRVGpuHandle());
	// EffectParticleBufferをセット
	context->SetGraphicsRootDescriptorTable(6, effectParticleBuffer->GetUAVGpuHandle());
	// TextureBufferをセット
	context->SetGraphicsRootDescriptorTable(7, resourceManager.GetSUVDHeap()->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	// DrawCall
	context->DrawIndexedInstanced(static_cast<UINT>(modelData->meshes[0].indices.size()), 128 * 1024, 0, 0, 0);
}

void GraphicsEngine::DrawUI(CommandContext* context, ResourceManager& resourceManager, GameCore& gameCore, RenderMode mode)
{
	if (resourceManager.GetUIContainer()->GetUseList().empty())
	{
		return;
	}

	// プリミティブトポロジの設定
	context->SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// パイプラインセット
	context->SetGraphicsPipelineState(m_PipelineManager->GetUIPSO().pso.Get());
	// ルートシグネチャセット
	context->SetGraphicsRootSignature(m_PipelineManager->GetUIPSO().rootSignature.Get());

	// UIComponentを取得
	// シーンがないならスキップ
	if (!gameCore.GetSceneManager()->GetMainScene()) { return; }
	IConstantBuffer* cameraBuffer = nullptr;
	// メインカメラを取得
	if (mode == RenderMode::Game)
	{
		// カメラオブジェクトのIDを取得
		std::optional<uint32_t> cameraID = gameCore.GetSceneManager()->GetMainScene()->GetMainCameraID();
		if (!cameraID) { return; }
		// カメラオブジェクトを取得
		GameObject& cameraObject = gameCore.GetObjectContainer()->GetGameObject(cameraID.value());
		if (!cameraObject.IsActive()) { return; }
		// カメラのバッファインデックスを取得
		CameraComponent* cameraComponent = gameCore.GetECSManager()->GetComponent<CameraComponent>(cameraObject.GetEntity());
		if (!cameraComponent) { return; }
		// カメラのバッファを取得
		cameraBuffer = resourceManager.GetBuffer<IConstantBuffer>(cameraComponent->bufferIndex);
		// カメラがないならスキップ
		if (!cameraBuffer) { return; }
	} else// デバッグカメラ
	{
		// カメラのバッファを取得
		cameraBuffer = resourceManager.GetDebugCameraBuffer();
		// カメラがないならスキップ
		if (!cameraBuffer) { return; }
	}
	// UIの頂点バッファを取得
	UIData& uiData = resourceManager.GetUIContainer()->GetUIData(0);
	// VBVをセット
	D3D12_VERTEX_BUFFER_VIEW* vbv = resourceManager.GetBuffer<IVertexBuffer>(uiData.vertexBufferIndex)->GetVertexBufferView();
	context->SetVertexBuffers(0, 1, vbv);
	// IBVをセット
	D3D12_INDEX_BUFFER_VIEW* ibv = resourceManager.GetBuffer<IIndexBuffer>(uiData.indexBufferIndex)->GetIndexBufferView();
	context->SetIndexBuffer(ibv);
	// UI統合バッファをセット
	IStructuredBuffer* uiBuffer = resourceManager.GetIntegrationBuffer(IntegrationDataType::UISprite);
	context->SetGraphicsRootDescriptorTable(0, uiBuffer->GetSRVGpuHandle());
	// UIUseListをセット
	IStructuredBuffer* useUIBuffer = resourceManager.GetBuffer<IStructuredBuffer>(resourceManager.GetUIContainer()->GetUseListBufferIndex());
	context->SetGraphicsRootDescriptorTable(1, useUIBuffer->GetSRVGpuHandle());
	// マテリアル統合バッファをセット
	IStructuredBuffer* materialBuffer = resourceManager.GetIntegrationBuffer(IntegrationDataType::Material);
	context->SetGraphicsRootDescriptorTable(2, materialBuffer->GetSRVGpuHandle());
	// 配列テクスチャのためヒープをセット
	context->SetGraphicsRootDescriptorTable(3, resourceManager.GetSUVDHeap()->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	// DrawCall
	context->DrawIndexedInstanced(6, static_cast<UINT>(resourceManager.GetUIContainer()->GetUseList().size()), 0, 0, 0);

}
