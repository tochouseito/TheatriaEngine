#include "PrecompiledHeader.h"
#include "DrawExecution.h"
#include"Base/Format.h"
#include"WinApp/WinApp.h"
#include"D3D12/D3DCommand/D3DCommand.h"
#include"D3D12/D3DSwapChain/D3DSwapChain.h"
#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"D3D12/RTVManager/RTVManager.h"
#include"D3D12/DSVManager/DSVManager.h"
#include"Graphics/GraphicsSystem/GraphicsSystem.h"

static const float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
//static const float clearColor[] = { 0.0f,0.0f,0.0f,1.0f };

void DrawExecution::Initialize(
	D3DCommand* d3dCommand,
	D3DSwapChain* d3dSwapChain,
	ResourceViewManager* resourceViewManager,
	RTVManager* rtvManager,
	DSVManager* dsvManager,
	GraphicsSystem* graphicsSystem
)
{
	d3dCommand_ = d3dCommand;
	d3dSwapChain_ = d3dSwapChain;
	resourceViewManager_ = resourceViewManager;
	rtvManager_ = rtvManager;
	dsvManager_ = dsvManager;
	graphicsSystem_ = graphicsSystem;

	uint32_t w = WindowWidth();
	uint32_t h = WindowHeight();

	// オフスクリーンレンダーテクスチャの作成
	sceneRenderHandle.texIndex = resourceViewManager_->GetNewHandle();
	resourceViewManager_->CreateRenderTextureResource(sceneRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
		);
	sceneRenderHandle.rtvIndex = 
		rtvManager_->CreateRTV(
			resourceViewManager_->GetHandle(
				sceneRenderHandle.texIndex).resource.Get()
		);

	// 最終レンダーテクスチャの作成
	finalRenderHandle.texIndex = resourceViewManager_->GetNewHandle();
	resourceViewManager_->CreateRenderTextureResource(finalRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	finalRenderHandle.rtvIndex =
		rtvManager_->CreateRTV(
			resourceViewManager_->GetHandle(
				finalRenderHandle.texIndex).resource.Get()
		);

	// デバッグオフスクリーンレンダーテクスチャの作成
	debugRenderHandle.texIndex = resourceViewManager_->GetNewHandle();
	resourceViewManager_->CreateRenderTextureResource(debugRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	debugRenderHandle.rtvIndex =
		rtvManager_->CreateRTV(
			resourceViewManager_->GetHandle(
				debugRenderHandle.texIndex).resource.Get()
		);

	// 最終デバッグレンダーテクスチャの作成
	debugFinalRenderHandle.texIndex = resourceViewManager_->GetNewHandle();
	resourceViewManager_->CreateRenderTextureResource(debugFinalRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	debugFinalRenderHandle.rtvIndex =
		rtvManager_->CreateRTV(
			resourceViewManager_->GetHandle(
				debugFinalRenderHandle.texIndex).resource.Get()
		);

	// アルベドカラーのRTVを作成
	albedoRenderHandle.texIndex = resourceViewManager_->GetNewHandle();
	resourceViewManager_->CreateRenderTextureResource(albedoRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	albedoRenderHandle.rtvIndex =
		rtvManager_->CreateRTV(
			resourceViewManager_->GetHandle(
				albedoRenderHandle.texIndex).resource.Get()
		);

	// 法線のRTVを作成
	normalRenderHandle.texIndex = resourceViewManager_->GetNewHandle();
	resourceViewManager_->CreateRenderTextureResource(normalRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	normalRenderHandle.rtvIndex =
		rtvManager_->CreateRTV(
			resourceViewManager_->GetHandle(
				normalRenderHandle.texIndex).resource.Get()
		);

	// 位置情報のRTVを作成
	positionRenderHandle.texIndex = resourceViewManager_->GetNewHandle();
	resourceViewManager_->CreateRenderTextureResource(positionRenderHandle.texIndex,
		w, h, DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	positionRenderHandle.rtvIndex =
		rtvManager_->CreateRTV(
			resourceViewManager_->GetHandle(
				positionRenderHandle.texIndex).resource.Get(),
			DXGI_FORMAT_R32G32B32A32_FLOAT
		);

	// マテリアルのRTVを作成
	materialRenderHandle.texIndex = resourceViewManager_->GetNewHandle();
	resourceViewManager_->CreateRenderTextureResource(materialRenderHandle.texIndex,
		w, h, DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	materialRenderHandle.rtvIndex =
		rtvManager_->CreateRTV(
			resourceViewManager_->GetHandle(
				materialRenderHandle.texIndex).resource.Get(),
			DXGI_FORMAT_R32G32B32A32_FLOAT
		);

	// デバッグ用
	// アルベドカラーのRTVを作成
	debugAlbedoRenderHandle.texIndex = resourceViewManager_->GetNewHandle();
	resourceViewManager_->CreateRenderTextureResource(debugAlbedoRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	debugAlbedoRenderHandle.rtvIndex =
		rtvManager_->CreateRTV(
			resourceViewManager_->GetHandle(
				debugAlbedoRenderHandle.texIndex).resource.Get()
		);

	// 法線のRTVを作成
	debugNormalRenderHandle.texIndex = resourceViewManager_->GetNewHandle();
	resourceViewManager_->CreateRenderTextureResource(debugNormalRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	debugNormalRenderHandle.rtvIndex =
		rtvManager_->CreateRTV(
			resourceViewManager_->GetHandle(
				debugNormalRenderHandle.texIndex).resource.Get()
		);

	// 位置情報のRTVを作成
	debugPositionRenderHandle.texIndex = resourceViewManager_->GetNewHandle();
	resourceViewManager_->CreateRenderTextureResource(debugPositionRenderHandle.texIndex,
		w, h, DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	debugPositionRenderHandle.rtvIndex =
		rtvManager_->CreateRTV(
			resourceViewManager_->GetHandle(
				debugPositionRenderHandle.texIndex).resource.Get(),
			DXGI_FORMAT_R32G32B32A32_FLOAT
		);

	// マテリアルのRTVを作成
	debugMaterialRenderHandle.texIndex = resourceViewManager_->GetNewHandle();
	resourceViewManager_->CreateRenderTextureResource(debugMaterialRenderHandle.texIndex,
		w, h, DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	debugMaterialRenderHandle.rtvIndex =
		rtvManager_->CreateRTV(
			resourceViewManager_->GetHandle(
				debugMaterialRenderHandle.texIndex).resource.Get(),
			DXGI_FORMAT_R32G32B32A32_FLOAT
		);

	// シャドウマップのRTVを作成
	shadowMapRenderHandle.texIndex = resourceViewManager_->GetNewHandle();
	resourceViewManager_->CreateRenderTextureResource(shadowMapRenderHandle.texIndex,
		1024, 1024, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	shadowMapRenderHandle.rtvIndex =
		rtvManager_->CreateRTV(
			resourceViewManager_->GetHandle(
				shadowMapRenderHandle.texIndex).resource.Get(),
			dxgiFormat
		);

}

void DrawExecution::PreDraw()
{
	ID3D12GraphicsCommandList6* commandList = d3dCommand_->GetCommand(CommandType::Draw).list.Get();

	BarrierTransition(
		resourceViewManager_->GetHandle(dsvManager_->GetSRVIndex()).resource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);

	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvManager_->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetHandle(sceneRenderHandle.rtvIndex).CPUHandle;
	commandList->OMSetRenderTargets(
		1,
		&rtvHandle,
		false,
		&dsvHandle
	);

	//// 指定した色で画面全体をクリアする
	//commandList->ClearRenderTargetView(
	//	rtvHandle,
	//	clearColor,
	//	0,
	//	nullptr
	//);

	// 指定した深度で画面全体をクリアする
	commandList->ClearDepthStencilView(
		dsvHandle,
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,0, 0,
		nullptr
	);

	// ビューポートの設定
	D3D12_VIEWPORT viewport =
		D3D12_VIEWPORT(
			0.0f, 0.0f,
			static_cast<float>((WindowWidth())),
			static_cast<float>((WindowHeight())),
			0.0f, 1.0f
		);
	commandList->RSSetViewports(1, &viewport);// Viewportを設定

	// シザリング矩形の設定
	D3D12_RECT rect = D3D12_RECT(
		0, 0,
		WindowWidth(),
		WindowHeight()
	);
	commandList->RSSetScissorRects(1, &rect);// Scissorを設定

	// 形状を設定。
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DrawExecution::PreDrawDebugGBuffer()
{
	ID3D12GraphicsCommandList6* commandList = d3dCommand_->GetCommand(CommandType::Draw).list.Get();

	BarrierTransition(
		resourceViewManager_->GetHandle(debugAlbedoRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	BarrierTransition(
		resourceViewManager_->GetHandle(debugNormalRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	BarrierTransition(
		resourceViewManager_->GetHandle(debugPositionRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvManager_->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE albedoHandle = rtvManager_->GetHandle(albedoRenderHandle.rtvIndex).CPUHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE normalHandle = rtvManager_->GetHandle(normalRenderHandle.rtvIndex).CPUHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE positionHandle = rtvManager_->GetHandle(positionRenderHandle.rtvIndex).CPUHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[] = {
		albedoHandle,
		normalHandle,
		positionHandle
	};
	commandList->OMSetRenderTargets(
		2,
		rtvHandle,
		false,
		&dsvHandle
	);

	// 指定した色で画面全体をクリアする
	commandList->ClearRenderTargetView(albedoHandle, clearColor, 0, nullptr);
	commandList->ClearRenderTargetView(normalHandle, clearColor, 0, nullptr);
	commandList->ClearRenderTargetView(positionHandle, clearColor, 0, nullptr);

	// 指定した深度で画面全体をクリアする
	commandList->ClearDepthStencilView(
		dsvHandle,
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f, 0, 0,
		nullptr
	);

	// ビューポートの設定
	D3D12_VIEWPORT viewport =
		D3D12_VIEWPORT(
			0.0f, 0.0f,
			static_cast<float>((WindowWidth())),
			static_cast<float>((WindowHeight())),
			0.0f, 1.0f
		);
	commandList->RSSetViewports(1, &viewport);// Viewportを設定

	// シザリング矩形の設定
	D3D12_RECT rect = D3D12_RECT(
		0, 0,
		WindowWidth(),
		WindowHeight()
	);
	commandList->RSSetScissorRects(1, &rect);// Scissorを設定

	// 形状を設定。
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DrawExecution::DebugPreDrawGBufferMix()
{
	ID3D12GraphicsCommandList6* commandList = d3dCommand_->GetCommand(CommandType::Draw).list.Get();

	/*BarrierTransition(
		resourceViewManager_->GetHandle(debugRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);*/
	// 描画先のRTVとDSVを設定する
	//D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvManager_->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetHandle(debugRenderHandle.rtvIndex).CPUHandle;
	commandList->OMSetRenderTargets(
		1,
		&rtvHandle,
		false,
		nullptr//&dsvHandle
	);

	// 指定した色で画面全体をクリアする
	commandList->ClearRenderTargetView(
		rtvHandle,
		clearColor,
		0,
		nullptr
	);

	//// 指定した深度で画面全体をクリアする
	//commandList->ClearDepthStencilView(
	//	dsvHandle,
	//	D3D12_CLEAR_FLAG_DEPTH,
	//	1.0f, 0, 0,
	//	nullptr
	//);

	// ビューポートの設定
	D3D12_VIEWPORT viewport =
		D3D12_VIEWPORT(
			0.0f, 0.0f,
			static_cast<float>((WindowWidth())),
			static_cast<float>((WindowHeight())),
			0.0f, 1.0f
		);
	commandList->RSSetViewports(1, &viewport);// Viewportを設定

	// シザリング矩形の設定
	D3D12_RECT rect = D3D12_RECT(
		0, 0,
		WindowWidth(),
		WindowHeight()
	);
	commandList->RSSetScissorRects(1, &rect);// Scissorを設定

	// 形状を設定。
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DrawExecution::DebugPreDraw()
{
	ID3D12GraphicsCommandList6* commandList = d3dCommand_->GetCommand(CommandType::Draw).list.Get();

	BarrierTransition(
		resourceViewManager_->GetHandle(debugRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvManager_->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetHandle(debugRenderHandle.rtvIndex).CPUHandle;
	commandList->OMSetRenderTargets(
		1,
		&rtvHandle,
		false,
		&dsvHandle
	);

	// 指定した色で画面全体をクリアする
	commandList->ClearRenderTargetView(
		rtvHandle,
		clearColor,
		0,
		nullptr
	);

	// 指定した深度で画面全体をクリアする
	commandList->ClearDepthStencilView(
		dsvHandle,
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f, 0, 0,
		nullptr
	);

	// ビューポートの設定
	D3D12_VIEWPORT viewport =
		D3D12_VIEWPORT(
			0.0f, 0.0f,
			static_cast<float>((WindowWidth())),
			static_cast<float>((WindowHeight())),
			0.0f, 1.0f
		);
	commandList->RSSetViewports(1, &viewport);// Viewportを設定

	// シザリング矩形の設定
	D3D12_RECT rect = D3D12_RECT(
		0, 0,
		WindowWidth(),
		WindowHeight()
	);
	commandList->RSSetScissorRects(1, &rect);// Scissorを設定

	// 形状を設定。
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DrawExecution::Draw()
{
}

void DrawExecution::PostDraw()
{
	ID3D12GraphicsCommandList6* commandList = d3dCommand_->GetCommand(CommandType::Draw).list.Get();

	UINT backBufferIndex = d3dSwapChain_->GetSwapChain()->GetCurrentBackBufferIndex();

	// スワップチェーンリソースの状態遷移
	BarrierTransition(rtvManager_->GetHandle(backBufferIndex).resource.Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	// レンダーテクスチャリソースの状態遷移
	BarrierTransition(resourceViewManager_->GetHandle(sceneRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

	// デバッグレンダーテクスチャリソースの状態遷移
	BarrierTransition(resourceViewManager_->GetHandle(debugRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

	/*BarrierTransition(
		resourceViewManager_->GetHandle(albedoRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

	BarrierTransition(
		resourceViewManager_->GetHandle(normalRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);*/

#ifdef _DEBUG
	{
		BarrierTransition(
			resourceViewManager_->GetHandle(finalRenderHandle.texIndex).resource.Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetHandle(finalRenderHandle.rtvIndex).CPUHandle;

		// 描画先のRTVを設定
		commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		// 指定した色で画面全体をクリアする
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		// ビューポートの設定
		D3D12_VIEWPORT viewport =
			D3D12_VIEWPORT(
				0.0f, 0.0f,
				static_cast<float>((WindowWidth())),
				static_cast<float>((WindowHeight())),
				0.0f, 1.0f
			);
		commandList->RSSetViewports(1, &viewport);// Viewportを設定

		// シザリング矩形の設定
		D3D12_RECT rect = D3D12_RECT(
			0, 0,
			WindowWidth(),
			WindowHeight()
		);
		commandList->RSSetScissorRects(1, &rect);// Scissorを設定

		// 形状を設定。
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		commandList->SetGraphicsRootSignature(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::ScreenCopy).rootSignature.Get());

		commandList->SetPipelineState(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::ScreenCopy).Blend[0].Get());

		commandList->SetGraphicsRootDescriptorTable(0, resourceViewManager_->GetHandle(sceneRenderHandle.texIndex).GPUHandle);

		commandList->DrawInstanced(3, 1, 0, 0);

		// 最終レンダーテクスチャリソースの状態遷移
		BarrierTransition(resourceViewManager_->GetHandle(finalRenderHandle.texIndex).resource.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
	}
	{
		BarrierTransition(
			resourceViewManager_->GetHandle(debugFinalRenderHandle.texIndex).resource.Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetHandle(debugFinalRenderHandle.rtvIndex).CPUHandle;

		// 描画先のRTVを設定
		commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		// 指定した色で画面全体をクリアする
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		// ビューポートの設定
		D3D12_VIEWPORT viewport =
			D3D12_VIEWPORT(
				0.0f, 0.0f,
				static_cast<float>((WindowWidth())),
				static_cast<float>((WindowHeight())),
				0.0f, 1.0f
			);
		commandList->RSSetViewports(1, &viewport);// Viewportを設定

		// シザリング矩形の設定
		D3D12_RECT rect = D3D12_RECT(
			0, 0,
			WindowWidth(),
			WindowHeight()
		);
		commandList->RSSetScissorRects(1, &rect);// Scissorを設定

		// 形状を設定。
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		commandList->SetGraphicsRootSignature(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::ScreenCopy).rootSignature.Get());

		commandList->SetPipelineState(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::ScreenCopy).Blend[0].Get());

		commandList->SetGraphicsRootDescriptorTable(0, resourceViewManager_->GetHandle(debugRenderHandle.texIndex).GPUHandle);

		commandList->DrawInstanced(3, 1, 0, 0);

		// デバッグ最終レンダーテクスチャリソースの状態遷移
		BarrierTransition(resourceViewManager_->GetHandle(debugFinalRenderHandle.texIndex).resource.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
	}
#endif // _DEBUG

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetHandle(backBufferIndex).CPUHandle;

		// 描画先のRTVを設定
		commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		// 指定した色で画面全体をクリアする
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		// ビューポートの設定
		D3D12_VIEWPORT viewport =
			D3D12_VIEWPORT(
				0.0f, 0.0f,
				static_cast<float>((WindowWidth())),
				static_cast<float>((WindowHeight())),
				0.0f, 1.0f
			);
		commandList->RSSetViewports(1, &viewport);// Viewportを設定

		// シザリング矩形の設定
		D3D12_RECT rect = D3D12_RECT(
			0, 0,
			WindowWidth(),
			WindowHeight()
		);
		commandList->RSSetScissorRects(1, &rect);// Scissorを設定

		// 形状を設定。
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		commandList->SetGraphicsRootSignature(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::ScreenCopy).rootSignature.Get());

		commandList->SetPipelineState(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::ScreenCopy).Blend[0].Get());

#ifdef NDEBUG

		{

			commandList->SetGraphicsRootDescriptorTable(0, resourceViewManager_->GetHandle(sceneRenderHandle.texIndex).GPUHandle);

			commandList->DrawInstanced(3, 1, 0, 0);
		}
#endif // Release
}

void DrawExecution::End()
{
	UINT backBufferIndex = d3dSwapChain_->GetSwapChain()->GetCurrentBackBufferIndex();

	//// レンダーテクスチャリソースの状態遷移
	//BarrierTransition(resourceViewManager_->GetHandle(offscreenRenderTextureIndex).resource.Get(),
	//	)

	// スワップチェーンリソースの状態遷移
	BarrierTransition(rtvManager_->GetHandle(backBufferIndex).resource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);

	//// 最終レンダーテクスチャリソースの状態遷移
	//BarrierTransition(resourceViewManager_->GetHandle(finalRenderHandle.texIndex).resource.Get(),
	//	D3D12_RESOURCE_STATE_RENDER_TARGET,
	//	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	//);

	//// 最終デバッグレンダーテクスチャリソースの状態遷移
	//BarrierTransition(resourceViewManager_->GetHandle(debugFinalRenderHandle.texIndex).resource.Get(),
	//	D3D12_RESOURCE_STATE_RENDER_TARGET,
	//	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	//);

	d3dCommand_->Close(DIRECT,CommandType::Draw);

	d3dSwapChain_->Present();

	d3dCommand_->Signal(DIRECT);
}

void DrawExecution::ResizeOffscreenRenderTex()
{
	// オフスクリーンレンダリング用のTextureを解放
	resourceViewManager_->GetHandle(sceneRenderHandle.texIndex).resource.Reset();
	resourceViewManager_->GetHandle(sceneRenderHandle.texIndex).resource = nullptr;

	// 最終レンダリング用のTextureを解放
	resourceViewManager_->GetHandle(finalRenderHandle.texIndex).resource.Reset();
	resourceViewManager_->GetHandle(finalRenderHandle.texIndex).resource = nullptr;

	// デバッグ用
	resourceViewManager_->GetHandle(debugRenderHandle.texIndex).resource.Reset();
	resourceViewManager_->GetHandle(debugRenderHandle.texIndex).resource = nullptr;

	// デバッグ最終レンダリング用のTextureを解放
	resourceViewManager_->GetHandle(debugFinalRenderHandle.texIndex).resource.Reset();
	resourceViewManager_->GetHandle(debugFinalRenderHandle.texIndex).resource = nullptr;

	// アルベドカラーのRTVを解放
	resourceViewManager_->GetHandle(albedoRenderHandle.texIndex).resource.Reset();
	resourceViewManager_->GetHandle(albedoRenderHandle.texIndex).resource = nullptr;

	// 法線のRTVを解放
	resourceViewManager_->GetHandle(normalRenderHandle.texIndex).resource.Reset();
	resourceViewManager_->GetHandle(normalRenderHandle.texIndex).resource = nullptr;

	// 位置情報のRTVを解放
	resourceViewManager_->GetHandle(positionRenderHandle.texIndex).resource.Reset();
	resourceViewManager_->GetHandle(positionRenderHandle.texIndex).resource = nullptr;

	// マテリアルのRTVを解放
	resourceViewManager_->GetHandle(materialRenderHandle.texIndex).resource.Reset();
	resourceViewManager_->GetHandle(materialRenderHandle.texIndex).resource = nullptr;

	// デバッグ用
	resourceViewManager_->GetHandle(debugAlbedoRenderHandle.texIndex).resource.Reset();
	resourceViewManager_->GetHandle(debugAlbedoRenderHandle.texIndex).resource = nullptr;

	resourceViewManager_->GetHandle(debugNormalRenderHandle.texIndex).resource.Reset();
	resourceViewManager_->GetHandle(debugNormalRenderHandle.texIndex).resource = nullptr;

	resourceViewManager_->GetHandle(debugPositionRenderHandle.texIndex).resource.Reset();
	resourceViewManager_->GetHandle(debugPositionRenderHandle.texIndex).resource = nullptr;

	resourceViewManager_->GetHandle(debugMaterialRenderHandle.texIndex).resource.Reset();
	resourceViewManager_->GetHandle(debugMaterialRenderHandle.texIndex).resource = nullptr;

	// シャドウマップのRTVを解放
	resourceViewManager_->GetHandle(shadowMapRenderHandle.texIndex).resource.Reset();
	resourceViewManager_->GetHandle(shadowMapRenderHandle.texIndex).resource = nullptr;

	uint32_t w = WindowWidth();
	uint32_t h = WindowHeight();

	// オフスクリーンレンダーテクスチャの作成
	resourceViewManager_->CreateRenderTextureResource(sceneRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	rtvManager_->RemakeRTV(
		sceneRenderHandle.rtvIndex,
		resourceViewManager_->GetHandle(sceneRenderHandle.texIndex).resource.Get()
	);

	// 最終レンダーテクスチャの作成
	resourceViewManager_->CreateRenderTextureResource(finalRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	rtvManager_->RemakeRTV(
		finalRenderHandle.rtvIndex,
		resourceViewManager_->GetHandle(finalRenderHandle.texIndex).resource.Get()
	);

	// デバッグオフスクリーンレンダーテクスチャの作成
	resourceViewManager_->CreateRenderTextureResource(debugRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	rtvManager_->RemakeRTV(
		debugRenderHandle.rtvIndex,
		resourceViewManager_->GetHandle(debugRenderHandle.texIndex).resource.Get()
	);

	// 最終デバッグレンダーテクスチャの作成
	resourceViewManager_->CreateRenderTextureResource(debugFinalRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	rtvManager_->RemakeRTV(
		debugFinalRenderHandle.rtvIndex,
		resourceViewManager_->GetHandle(debugFinalRenderHandle.texIndex).resource.Get()
	);

	// アルベドカラーのRTVを作成
	resourceViewManager_->CreateRenderTextureResource(albedoRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	rtvManager_->RemakeRTV(
		albedoRenderHandle.rtvIndex,
		resourceViewManager_->GetHandle(albedoRenderHandle.texIndex).resource.Get()
	);

	// 法線のRTVを作成
	resourceViewManager_->CreateRenderTextureResource(normalRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	rtvManager_->RemakeRTV(
		normalRenderHandle.rtvIndex,
		resourceViewManager_->GetHandle(normalRenderHandle.texIndex).resource.Get()
	);

	// 位置情報のRTVを作成
	resourceViewManager_->CreateRenderTextureResource(positionRenderHandle.texIndex,
		w, h, DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	rtvManager_->RemakeRTV(
		positionRenderHandle.rtvIndex,
		resourceViewManager_->GetHandle(positionRenderHandle.texIndex).resource.Get(),
		DXGI_FORMAT_R32G32B32A32_FLOAT
	);

	// マテリアルのRTVを作成
	resourceViewManager_->CreateRenderTextureResource(materialRenderHandle.texIndex,
		w, h, DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	rtvManager_->RemakeRTV(
		materialRenderHandle.rtvIndex,
		resourceViewManager_->GetHandle(materialRenderHandle.texIndex).resource.Get(),
		DXGI_FORMAT_R32G32B32A32_FLOAT
	);

	// デバッグ用
	resourceViewManager_->CreateRenderTextureResource(debugAlbedoRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	rtvManager_->RemakeRTV(
		debugAlbedoRenderHandle.rtvIndex,
		resourceViewManager_->GetHandle(debugAlbedoRenderHandle.texIndex).resource.Get()
	);

	resourceViewManager_->CreateRenderTextureResource(debugNormalRenderHandle.texIndex,
		w, h, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	rtvManager_->RemakeRTV(
		debugNormalRenderHandle.rtvIndex,
		resourceViewManager_->GetHandle(debugNormalRenderHandle.texIndex).resource.Get()
	);

	resourceViewManager_->CreateRenderTextureResource(debugPositionRenderHandle.texIndex,
		w, h, DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	rtvManager_->RemakeRTV(
		debugPositionRenderHandle.rtvIndex,
		resourceViewManager_->GetHandle(debugPositionRenderHandle.texIndex).resource.Get(),
		DXGI_FORMAT_R32G32B32A32_FLOAT
	);

	resourceViewManager_->CreateRenderTextureResource(debugMaterialRenderHandle.texIndex,
		w, h, DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	rtvManager_->RemakeRTV(
		debugMaterialRenderHandle.rtvIndex,
		resourceViewManager_->GetHandle(debugMaterialRenderHandle.texIndex).resource.Get(),
		DXGI_FORMAT_R32G32B32A32_FLOAT
	);

	// シャドウマップのRTVを作成
	resourceViewManager_->CreateRenderTextureResource(shadowMapRenderHandle.texIndex,
		1024, 1024, dxgiFormat,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		Color(clearColor[0], clearColor[1], clearColor[2], clearColor[3])
	);
	rtvManager_->RemakeRTV(
		shadowMapRenderHandle.rtvIndex,
		resourceViewManager_->GetHandle(shadowMapRenderHandle.texIndex).resource.Get(),
		dxgiFormat
	);
}

void DrawExecution::BarrierTransition(ID3D12Resource* pResource, D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After)
{
	ID3D12GraphicsCommandList6* commandList = d3dCommand_->GetCommand(CommandType::Draw).list.Get();

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};

	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = pResource;

	// 遷移前（現在）のResourceState
	barrier.Transition.StateBefore = Before;

	// 遷移後のResourceState
	barrier.Transition.StateAfter = After;

	// TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);
}

void DrawExecution::PreDrawGBuffer()
{
	ID3D12GraphicsCommandList6* commandList = d3dCommand_->GetCommand(CommandType::Draw).list.Get();

	BarrierTransition(
		resourceViewManager_->GetHandle(albedoRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	BarrierTransition(
		resourceViewManager_->GetHandle(normalRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	BarrierTransition(
		resourceViewManager_->GetHandle(positionRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	BarrierTransition(
		resourceViewManager_->GetHandle(materialRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvManager_->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE albedoHandle = rtvManager_->GetHandle(albedoRenderHandle.rtvIndex).CPUHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE normalHandle = rtvManager_->GetHandle(normalRenderHandle.rtvIndex).CPUHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE positionHandle = rtvManager_->GetHandle(positionRenderHandle.rtvIndex).CPUHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE materialHandle = rtvManager_->GetHandle(materialRenderHandle.rtvIndex).CPUHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[] = {
		albedoHandle,
		normalHandle,
		positionHandle,
		materialHandle
	};
	commandList->OMSetRenderTargets(
		4,
		rtvHandle,
		false,
		&dsvHandle
	);

	// 指定した色で画面全体をクリアする
	commandList->ClearRenderTargetView(albedoHandle,clearColor,0,nullptr);
	commandList->ClearRenderTargetView(normalHandle,clearColor,0,nullptr);
	commandList->ClearRenderTargetView(positionHandle, clearColor, 0, nullptr);
	commandList->ClearRenderTargetView(materialHandle, clearColor, 0, nullptr);

	// 指定した深度で画面全体をクリアする
	commandList->ClearDepthStencilView(
		dsvHandle,
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f, 0, 0,
		nullptr
	);

	// ビューポートの設定
	D3D12_VIEWPORT viewport =
		D3D12_VIEWPORT(
			0.0f, 0.0f,
			static_cast<float>((WindowWidth())),
			static_cast<float>((WindowHeight())),
			0.0f, 1.0f
		);
	commandList->RSSetViewports(1, &viewport);// Viewportを設定

	// シザリング矩形の設定
	D3D12_RECT rect = D3D12_RECT(
		0, 0,
		WindowWidth(),
		WindowHeight()
	);
	commandList->RSSetScissorRects(1, &rect);// Scissorを設定

	// 形状を設定。
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DrawExecution::PreDrawGBufferMix()
{
	ID3D12GraphicsCommandList6* commandList = d3dCommand_->GetCommand(CommandType::Draw).list.Get();

	BarrierTransition(
		resourceViewManager_->GetHandle(sceneRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	BarrierTransition(
		resourceViewManager_->GetHandle(albedoRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

	BarrierTransition(
		resourceViewManager_->GetHandle(normalRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

	BarrierTransition(
		resourceViewManager_->GetHandle(positionRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

	BarrierTransition(
		resourceViewManager_->GetHandle(materialRenderHandle.texIndex).resource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

	BarrierTransition(
		resourceViewManager_->GetHandle(dsvManager_->GetSRVIndex()).resource.Get(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

	// 描画先のRTVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetHandle(sceneRenderHandle.rtvIndex).CPUHandle;
	commandList->OMSetRenderTargets(
		1,
		&rtvHandle,
		false,
		nullptr
	);

	// 指定した色で画面全体をクリアする
	commandList->ClearRenderTargetView(
		rtvHandle,
		clearColor,
		0,
		nullptr
	);

	// ビューポートの設定
	D3D12_VIEWPORT viewport =
		D3D12_VIEWPORT(
			0.0f, 0.0f,
			static_cast<float>((WindowWidth())),
			static_cast<float>((WindowHeight())),
			0.0f, 1.0f
		);
	commandList->RSSetViewports(1, &viewport);// Viewportを設定

	// シザリング矩形の設定
	D3D12_RECT rect = D3D12_RECT(
		0, 0,
		WindowWidth(),
		WindowHeight()
	);
	commandList->RSSetScissorRects(1, &rect);// Scissorを設定

	// 形状を設定。
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

