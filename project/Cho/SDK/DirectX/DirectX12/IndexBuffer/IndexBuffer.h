#pragma once
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"

// インデックスバッファのインタフェース
class IIndexBuffer : public GpuBuffer
{
public:
	// Constructor
	IIndexBuffer() : GpuBuffer()
	{
	}
	// Constructor
	IIndexBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
		GpuBuffer(pResource, CurrentState)
	{
	}
	// Destructor
	virtual ~IIndexBuffer() = default;
};

template<typename T>
class IndexBuffer : public IIndexBuffer
{
public:
	// Constructor
	IndexBuffer() : IIndexBuffer()
	{
	}
	// Constructor
	IndexBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
		IIndexBuffer(pResource, CurrentState)
	{
	}
	// Destructor
	~IndexBuffer()
	{
		m_View = {};
		m_MappedData = std::span<T>{};
	}
	// リソースを作成
	void CreateIndexBufferResource(ID3D12Device8* device,const UINT& numElementes)
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
	bool CreateIBV()
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
		//m_View.StrideInBytes = GetStructureByteStride();
		// インデックスの形式
		if constexpr (std::is_same_v<T, uint16_t>)
		{
			m_View.Format = DXGI_FORMAT_R16_UINT;
		} else if constexpr (std::is_same_v<T, uint32_t>)
		{
			m_View.Format = DXGI_FORMAT_R32_UINT;
		} else
		{
			Log::Write(LogLevel::Assert, "IndexBuffer is not uint16_t or uint32_t");
			return false;
		}
		return true;
	}
	// スキニング用SRV
	bool CreateSRV(ID3D12Device8* device, D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, DescriptorHeap* pDescriptorHeap)
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
	D3D12_INDEX_BUFFER_VIEW m_View = {};
	// マップ用データにコピーするためのポインタ
	std::span<T> m_MappedData = nullptr;
};
