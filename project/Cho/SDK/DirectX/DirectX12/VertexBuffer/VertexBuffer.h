#pragma once
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"

// 頂点バッファのインタフェース
class IVertexBuffer : public GpuBuffer
{
public:
	// Constructor
	IVertexBuffer() : GpuBuffer()
	{
	}
	// Constructor
	IVertexBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
		GpuBuffer(pResource, CurrentState)
	{
	}
	// Destructor
	virtual ~IVertexBuffer() = default;
};

template<typename T>
class VertexBuffer : public IVertexBuffer
{
public:
	// Constructor
	VertexBuffer() : IVertexBuffer()
	{
	}
	// Constructor
	VertexBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
		IVertexBuffer(pResource, CurrentState)
	{
	}
	// Destructor
	~VertexBuffer()
	{
		m_View = {};
		m_MappedData = std::span<T>{};
	}
	// リソースを作成
	void CreateVertexBufferResource(ID3D12Device* device, const UINT& numElementes)
	{
		// リソースのサイズ
		UINT structureByteStride = static_cast<UINT>(sizeof(T));
		// リソース用のヒープの設定
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;// UploadHeapを使う
		GpuBuffer::CreateBuffer(
			device, heapProperties, D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_FLAG_NONE,
			numElementes, structureByteStride);
		// マッピング
		T* mappedData = nullptr;// 一時マップ用
		GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
		// マップしたデータをspanに変換
		m_MappedData = std::span<T>(mappedData, numElementes);
	}
	bool CreateVBV()
	{
		// Viewの作成
		// リソースがあるかどうか確認
		if (!GetResource())
		{
			Log::Write(LogLevel::Assert, "Resource is null");
			return false;
		}
		// Resourceの先頭のアドレスから使う
		m_View.BufferLocation = GetResource()->GetGPUVirtualAddress();
		// 使用するResourceのサイズ
		m_View.SizeInBytes = static_cast<UINT>(GetBufferSize());
		// 要素一つ分のサイズ
		m_View.StrideInBytes = GetStructureByteStride();
		return true;
	}
	// スキニング用SRV
	bool CreateSRV()
	{
		return true;
	}
	// スキニング用UAV
	bool CreateUAV()
	{
		return true;
	}
private:
	// インデックスバッファビュー
	D3D12_VERTEX_BUFFER_VIEW m_View = {};
	// マップ用データにコピーするためのポインタ
	std::span<T> m_MappedData = nullptr;
};

