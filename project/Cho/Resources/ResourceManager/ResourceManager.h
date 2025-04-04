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
#include "Resources/IntegrationData/IntegrationData.h"
#include "Resources/ModelManager/ModelManager.h"
#include "Resources/TextureManager/TextureManager.h"
#include "Core/Utility/CompBufferData.h"

enum IntegrationDataType
{
	Transform=0,
	kCount,
};

class GraphicsEngine;
class SwapChain;
class GraphicsEngine;
class ResourceManager
{
	friend class GraphicsEngine;
public:
	// コンストラクタ
	ResourceManager(ID3D12Device8* device,GraphicsEngine* graphicsEngine);
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

	// CreateBuffer
	template<typename T>
	uint32_t CreateConstantBuffer()
	{
		// 定数バッファの生成
		std::unique_ptr<ConstantBuffer<T>> buffer = std::make_unique<ConstantBuffer<T>>();
		buffer->CreateConstantBufferResource(m_Device);
		uint32_t index = m_ConstantBuffers.push_back(std::move(buffer));
		return index;
	}
	template<typename T>
	uint32_t CreateStructuredBuffer(const UINT& numElements)
	{
		// 構造化バッファの生成
		std::unique_ptr<StructuredBuffer<T>> buffer = std::make_unique<StructuredBuffer<T>>();
		buffer->CreateStructuredBufferResource(m_Device,numElements);
		// SRVの生成
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.NumElements = buffer->GetNumElements();
		srvDesc.Buffer.StructureByteStride = buffer->GetStructureByteStride();
		buffer->CreateSRV(m_Device, srvDesc, m_SUVDescriptorHeap.get());
		uint32_t index = m_StructuredBuffers.push_back(std::move(buffer));
		return index;
	}
	template<typename T>
	uint32_t CreateVertexBuffer(const UINT& numElements)
	{
		// 頂点バッファの生成
		std::unique_ptr<VertexBuffer<T>> buffer = std::make_unique<VertexBuffer<T>>();
		buffer->CreateVertexBufferResource(m_Device, numElements);
		// CreateVBV
		buffer->CreateVBV(numElements);
		uint32_t index = m_VertexBuffers.push_back(std::move(buffer));
		return index;
	}
	template<typename T>
	uint32_t CreateIndexBuffer(const UINT& numElements)
	{
		// インデックスバッファの生成
		std::unique_ptr<IndexBuffer<T>> buffer = std::make_unique<IndexBuffer<T>>();
		buffer->CreateIndexBufferResource(m_Device, numElements);
		// CreateIBV
		buffer->CreateIBV();
		uint32_t index = m_IndexBuffers.push_back(std::move(buffer));
		return index;
	}
	uint32_t CreateColorBuffer(D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE* clearValue, D3D12_RESOURCE_STATES& state);
	uint32_t CreateDepthBuffer(D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES& state);
	uint32_t CreateTextureBuffer(D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE* clearValue, D3D12_RESOURCE_STATES& state);


	// RemakeBuffer

	// ReleaseBuffer

	// 統合バッファ
	void CreateIntegrationBuffers();

	// GetBuffer
	template<typename T>
	T* GetBuffer(const std::optional<uint32_t>& index) const
	{
		if (!index.has_value())
		{
			return nullptr;
		}
		if constexpr (std::is_same_v<T, ColorBuffer>)
		{
			return m_ColorBuffers[index].get();
		} else if constexpr (std::is_same_v<T, DepthBuffer>)
		{
			return m_DepthBuffers[index].get();
		} else if constexpr (std::is_same_v<T, IVertexBuffer>)

		{
			return m_VertexBuffers[index].get();
		} else if constexpr (std::is_same_v<T, IConstantBuffer>)
		{
			return m_ConstantBuffers[index].get();
		} else if constexpr (std::is_same_v<T, IStructuredBuffer>)
		{
			return m_StructuredBuffers[index].get();
		} else if constexpr (std::is_same_v<T, PixelBuffer>)
		{
			return m_TextureBuffers[index].get();
		} else
			assert(false && "Invalid buffer type");
	}

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
	// 統合バッファ
	std::array<std::unique_ptr<IIntegrationData>, IntegrationDataType::kCount> m_IntegrationData;
};

