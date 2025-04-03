#pragma once

/*--------------------------------------------
リソースマネージャクラス
--------------------------------------------*/

#include "SDK/DirectX/DirectX12/DescriptorHeap/DescriptorHeap.h"
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "SDK/DirectX/DirectX12/VertexBuffer/VertexBuffer.h"
#include "SDK/DirectX/DirectX12/IndexBuffer/IndexBuffer.h"
#include "SDK/DirectX/DirectX12/ColorBuffer/ColorBuffer.h"
#include "SDK/DirectX/DirectX12/DepthBuffer/DepthBuffer.h"
#include "Resources/ModelManager/ModelManager.h"
#include "Resources/TextureManager/TextureManager.h"
#include "Core/Utility/CompBufferData.h"
#include <optional>

class GraphicsEngine;
class SwapChain;
class IntegrationBuffer;
class GraphicsEngine;
class ResourceManager
{
	friend class GraphicsEngine;
public:
	// コンストラクタ
	ResourceManager(ID3D12Device8* device);
	// デストラクタ
	~ResourceManager();
	// 初期化
	void Initialize();
	// 終了処理
	void Finalize();
	// 更新
	void Update();
	// 解放
	void Release();

	void GenerateManager(IntegrationBuffer* integrationBuffer,GraphicsEngine* graphicsEngine)
	{
		m_ModelManager = std::make_unique<ModelManager>(this, integrationBuffer);
		m_TextureManager = std::make_unique<TextureManager>(this, graphicsEngine, m_Device);
	}

	// BufferMethod
	uint32_t CreateColorBuffer(BUFFER_COLOR_DESC& desc);
	uint32_t CreateDepthBuffer(BUFFER_DEPTH_DESC& desc);
	uint32_t CreateVertexBuffer(BUFFER_VERTEX_DESC& desc);
	uint32_t CreateConstantBuffer(BUFFER_CONSTANT_DESC& desc);
	uint32_t CreateStructuredBuffer(BUFFER_STRUCTURED_DESC& desc);
	uint32_t CreateTextureBuffer(BUFFER_TEXTURE_DESC& desc);

	// RemakeMethod
	void RemakeColorBuffer(const uint32_t& index, BUFFER_COLOR_DESC& desc);
	void RemakeDepthBuffer(const uint32_t& index, BUFFER_DEPTH_DESC& desc);
	void RemakeVertexBuffer(const uint32_t& index, BUFFER_VERTEX_DESC& desc);

	// ReleaseMethod
	void ReleaseColorBuffer(const uint32_t& index);
	void ReleaseDepthBuffer(const uint32_t& index);
	void ReleaseVertexBuffer(const uint32_t& index);

	// MapMethod
	/*uint32_t CreateMappedTF() { m_MappedTF.push_back(nullptr); }
	BUFFER_DATA_TF* GetMappedTF(const uint32_t& index) { return m_MappedTF[index]; }
	uint32_t CreateMappedViewProjection(const uint32_t& bufferIndex);
	BUFFER_DATA_VIEWPROJECTION* GetMappedViewProjection(const uint32_t& index) { return m_MappedViewProjection[index]; }*/

	//Getters
	DescriptorHeap* GetSUVDHeap() const { return m_SUVDescriptorHeap.get(); }
	DescriptorHeap* GetRTVDHeap() const { return m_RTVDescriptorHeap.get(); }
	DescriptorHeap* GetDSVDHeap() const { return m_DSVDescriptorHeap.get(); }
	TextureManager* GetTextureManager() const { return m_TextureManager.get(); }
	ModelManager* GetModelManager() const { return m_ModelManager.get(); }
private:
	// Heap生成
	void CreateHeap(ID3D12Device8* device);
	static const uint32_t kMaxSUVDescriptorHeapSize = 1024;
	static const uint32_t kMaxRTVDescriptorHeapSize = 16;
	static const uint32_t kMaxDSVDescriptorHeapSize = 1;

	// Device
	ID3D12Device8* m_Device = nullptr;
	// SUVディスクリプタヒープ
	std::unique_ptr<DescriptorHeap> m_SUVDescriptorHeap = nullptr;
	// RTVディスクリプタヒープ
	std::unique_ptr<DescriptorHeap> m_RTVDescriptorHeap = nullptr;
	// DSVディスクリプタヒープ
	std::unique_ptr<DescriptorHeap> m_DSVDescriptorHeap = nullptr;
	// テクスチャマネージャ
	std::unique_ptr<TextureManager> m_TextureManager = nullptr;
	// モデルマネージャ
	std::unique_ptr<ModelManager> m_ModelManager = nullptr;
	//// GPUResourceUpdate用のマッピングデータ
	//FVector<BUFFER_DATA_TF*> m_MappedTF;
	//FVector<BUFFER_DATA_VIEWPROJECTION*> m_MappedViewProjection;
	// 定数バッファ
	FVector<std::unique_ptr<IConstantBuffer>> m_ConstantBuffers;
	// 構造化バッファ
	FVector<std::unique_ptr<IStructuredBuffer>> m_StructuredBuffers;
	// 頂点バッファ
	FVector<std::unique_ptr<IVertexBuffer>> m_VertexBuffers;
	// インデックスバッファ
	FVector<std::unique_ptr<IIndexBuffer>> m_IndexBuffers;
	// カラーバッファ
	FVector<std::unique_ptr<ColorBuffer>> m_ColorBuffers;
	// 深度バッファ
	FVector<std::unique_ptr<DepthBuffer>> m_DepthBuffers;
	// テクスチャバッファ
	FVector<std::unique_ptr<PixelBuffer>> m_TextureBuffers;
};

