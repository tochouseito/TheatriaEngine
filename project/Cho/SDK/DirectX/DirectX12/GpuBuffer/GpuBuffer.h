#pragma once

#include "Cho/SDK/DirectX/DirectX12/GpuResource/GpuResource.h"

class GpuBuffer : public GpuResource
{
public:
	// Constructor
	GpuBuffer() 
	{
	}
	// Constructor
	GpuBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
		GpuResource(pResource, CurrentState)
	{
	}
	// Destructor
	virtual ~GpuBuffer()
	{
		
	}
	// CreateBuffer
	void CreateBuffer(
		ID3D12Device* device,
		D3D12_HEAP_PROPERTIES& heapProperties,
		D3D12_HEAP_FLAGS heapFlags,
		D3D12_RESOURCE_STATES InitialState,
		D3D12_RESOURCE_FLAGS resourceFlags,
		const UINT& numElementes,
		const UINT& structureByteStride);
	// Getters
	UINT64 GetBufferSize() const { return m_BufferSize; }
	UINT GetNumElements() const { return m_NumElementes; }
	UINT GetStructureByteStride() const { return m_StructureByteStride; }
protected:
    UINT64 m_BufferSize = {};
    UINT m_NumElementes = {};
    UINT m_StructureByteStride = {};
};

template<typename T>
class ConstantBuffer : public GpuBuffer
{
public:
	// Constructor
	ConstantBuffer() : GpuBuffer()
	{
	}
	// Constructor
	ConstantBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
		GpuBuffer(pResource, CurrentState)
	{
	}
	// Destructor
	~ConstantBuffer()
	{
		m_MappedData = std::span<T>{};
	}
	// リソースを作成
	void CreateConstantBufferResource(ID3D12Device* device)
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
			1, structureByteStride);
		// マッピング
		T* mappedData = nullptr;// 一時マップ用
		GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
		// マップしたデータをspanに変換
		m_MappedData = std::span<T>(mappedData, 1);
	}
	void Unmap()
	{
		if (GetResource())
		{
			GetResource()->Unmap(0, nullptr);
		}
	}
	void UpdateData(const T& data)
	{
		if (m_MappedData.size() > 0)
		{
			m_MappedData[0] = data;
		} else
		{
			Log::Write(LogLevel::Assert, "Index out of range");
		}
	}
private:
	// マッピングデータ
	std::span<T> m_MappedData = nullptr;
};

template<typename T>
class StructuredBuffer : public GpuBuffer
{
public:
	// Constructor
	StructuredBuffer() : GpuBuffer()
	{
	}
	// Constructor
	StructuredBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
		GpuBuffer(pResource, CurrentState)
	{
	}
	// Destructor
	~StructuredBuffer()
	{
		m_MappedData = std::span<T>{};
		m_SRVCpuHandle = {};
		m_SRVGpuHandle = {};
		m_SRVHandleIndex = std::nullopt;
	}
	void CreateStructuredBufferResource(ID3D12Device* device, const UINT& numElementes)
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
	bool CreateSRV(ID3D12Device8* device, D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, DescriptorHeap* pDescriptorHeap)
	{
		// ヒープがSRVタイプかどうか確認
		if (pDescriptorHeap->GetType() != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		{
			Log::Write(LogLevel::Assert, "DescriptorHeap is not SRV type");
			return false;
		}
		// リソースがあるかどうか確認
		if (!GetResource())
		{
			Log::Write(LogLevel::Assert, "Resource is null");
			return false;
		}
		// 新しいディスクリプタのインデックスとハンドルを取得
		// すでにSRVが作成されている場合はインデックスを取得しない
		if (!m_SRVHandleIndex.has_value())
		{
			m_SRVHandleIndex = pDescriptorHeap->Allocate();
		}
		// ディスクリプタヒープがいっぱいの場合は警告を出して終了
		if (!m_SRVHandleIndex.has_value())
		{
			Log::Write(LogLevel::Warn, "DescriptorHeap is full");
			return false;
		}
		// CPUハンドルを取得
		m_SRVCpuHandle = pDescriptorHeap->GetCPUDescriptorHandle(m_SRVHandleIndex.value());
		// GPUハンドルを取得
		m_SRVGpuHandle = pDescriptorHeap->GetGPUDescriptorHandle(m_SRVHandleIndex.value());
		// Viewの生成
		device->CreateShaderResourceView(
			GetResource(),
			&srvDesc,
			m_SRVCpuHandle
		);
		return true;
	}
	void Unmap()
	{
		if (GetResource())
		{
			GetResource()->Unmap(0, nullptr);
		}
	}
	void UpdateData(const T& data,const uint32_t& index)
	{
		if (m_MappedData.size() > index)
		{
			m_MappedData[index] = data;
		} else
		{
			Log::Write(LogLevel::Assert, "Index out of range");
		}
	}
private:
	// マッピングデータ
	std::span<T> m_MappedData = nullptr;
	// ディスクリプタハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE m_SRVCpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE m_SRVGpuHandle = {};
	// ディスクリプタハンドルインデックス
	std::optional<uint32_t> m_SRVHandleIndex = std::nullopt;
};


