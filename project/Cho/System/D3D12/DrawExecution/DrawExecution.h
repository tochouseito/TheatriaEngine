#pragma once

#include<d3d12.h>
#include<wrl.h>
#include<cstdint>

struct RTVIndex {
	uint32_t rtvIndex = 0;
	uint32_t texIndex = 0;
};

class D3DCommand;
class D3DSwapChain;
class ResourceViewManager;
class RTVManager;
class DSVManager;
class GraphicsSystem;
class DrawExecution
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(
		D3DCommand* d3dCommand,
		D3DSwapChain* d3dSwapChain,
		ResourceViewManager* resourceViewManager,
		RTVManager* rtvManager,
		DSVManager* dsvManager,
		GraphicsSystem* graphicsSystem
	);

	/*/// <summary>
	/// ディファードレンダリング
	/// </summary>
	void DrawGBuffer();*/

	void PreDrawGBuffer();

	void PreDrawGBufferMix();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	void PreDrawDebugGBuffer();

	void DebugPreDrawGBufferMix();

	/// <summary>
	/// デバッグ用描画前処理
	/// </summary>
	void DebugPreDraw();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	void End();

	uint32_t GetRenderTexIndex()const { return sceneRenderHandle.texIndex; }

	uint32_t GetRendTexRTVHandInd()const { return sceneRenderHandle.rtvIndex; }

	uint32_t GetFinalRenderTexIndex()const { return finalRenderHandle.texIndex; }

	uint32_t GetFinalRTVHandInd()const { return finalRenderHandle.rtvIndex; }

	uint32_t GetDebugRenTexInd()const { return debugRenderHandle.texIndex; }

	uint32_t GetDebugRTVHandInd()const { return debugRenderHandle.rtvIndex; }

	uint32_t GetDebugFinalRenTexInd()const { return debugFinalRenderHandle.texIndex; }

	uint32_t GetDebugFinalRTVHandInd()const { return debugFinalRenderHandle.rtvIndex; }

	RTVIndex GetAlbedoRTVIndex()const { return albedoRenderHandle; }

	RTVIndex GetNormalRTVIndex()const { return normalRenderHandle; }

	RTVIndex GetPositionRTVIndex()const { return positionRenderHandle; }

	RTVIndex GetMaterialRTVIndex()const { return materialRenderHandle; }

	RTVIndex GetDebugAlbedoRTVIndex()const { return debugAlbedoRenderHandle; }

	RTVIndex GetDebugNormalRTVIndex()const { return debugNormalRenderHandle; }

	RTVIndex GetDebugPositionRTVIndex()const { return debugPositionRenderHandle; }

	RTVIndex GetDebugMaterialRTVIndex()const { return debugMaterialRenderHandle; }

	RTVIndex GetShadowMapRTVIndex()const { return shadowMapRenderHandle; }
 
	void ResizeOffscreenRenderTex();

private:
	/// <summary>
	/// リソースバリア遷移
	/// </summary>
	void BarrierTransition(
		ID3D12Resource* pResource,
		D3D12_RESOURCE_STATES Before,
		D3D12_RESOURCE_STATES After
	);

private:
	/*借りインスタンス*/
	D3DCommand* d3dCommand_ = nullptr;
	D3DSwapChain* d3dSwapChain_ = nullptr;
	ResourceViewManager* resourceViewManager_ = nullptr;
	RTVManager* rtvManager_ = nullptr;
	DSVManager* dsvManager_ = nullptr;
	GraphicsSystem* graphicsSystem_ = nullptr;

	// オフスクリーンレンダーテクスチャ
	RTVIndex sceneRenderHandle;

	// デバッグ用オフスクリーンレンダーテクスチャ
	RTVIndex debugRenderHandle;

	// オフスクリーン最終テクスチャ(エディターのスワップチェーンみたいな)
	RTVIndex finalRenderHandle;

	// デバッグ用オフスクリーン最終テクスチャ
	RTVIndex debugFinalRenderHandle;

	// アルベドカラー
	RTVIndex albedoRenderHandle;

	// 法線書き込み用
	RTVIndex normalRenderHandle;

	// 位置情報書き込み用
	RTVIndex positionRenderHandle;

	// マテリアル情報書き込み用
	RTVIndex materialRenderHandle;

	// デバッグ用
	RTVIndex debugAlbedoRenderHandle;
	RTVIndex debugNormalRenderHandle;
	RTVIndex debugPositionRenderHandle;
	RTVIndex debugMaterialRenderHandle;

	// シャドウマップ
	RTVIndex shadowMapRenderHandle;
};

