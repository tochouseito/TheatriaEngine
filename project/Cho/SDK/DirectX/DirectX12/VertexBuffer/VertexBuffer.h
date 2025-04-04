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
	// リソース作成
	virtual void CreateVertexBufferResource(ID3D12Device* device, const UINT& numElements) = 0;
	// View作成
	virtual bool CreateVBV() = 0;
	// SRV作成
	virtual bool CreateSRV() = 0;
	// UAV作成
	virtual bool CreateUAV() = 0;
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
	void CreateVertexBufferResource(ID3D12Device* device, const UINT& numElements)
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
			numElements, structureByteStride);
		// マッピング
		T* mappedData = nullptr;// 一時マップ用
		GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
		// マップしたデータをspanに変換
		m_MappedData = std::span<T>(mappedData, numElements);
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
	void UpdateData(const T& data,const uint32_t& index)
	{
		if (!m_MappedData.size()<=index)
		{
			m_MappedData[index] = data;
		} else
		{
			Log::Write(LogLevel::Assert, "Index out of range");
		}
	}
	void MappedDataCopy(std::vector<T>& data)
	{
		if (!m_MappedData.empty())
		{
			memcpy(m_MappedData.data(), data.data(), sizeof(T) * GetNumElements());
		} else
		{
			Log::Write(LogLevel::Assert, "MappedData is null");
		}
	}
private:
	// インデックスバッファビュー
	D3D12_VERTEX_BUFFER_VIEW m_View = {};
	// マップ用データにコピーするためのポインタ
	std::span<T> m_MappedData = nullptr;
};

