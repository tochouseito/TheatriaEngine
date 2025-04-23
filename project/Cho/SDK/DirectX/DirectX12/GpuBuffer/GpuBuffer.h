#pragma once

#include "SDK/DirectX/DirectX12/GpuResource/GpuResource.h"
#include <type_traits>

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
		const UINT& numElements,
		const UINT& structureByteStride);
	// Getters
	UINT64 GetBufferSize() const { return m_BufferSize; }
	UINT GetNumElements() const { return m_NumElements; }
	UINT GetStructureByteStride() const { return m_StructureByteStride; }
protected:
    UINT64 m_BufferSize = {};
    UINT m_NumElements = {};
    UINT m_StructureByteStride = {};
};

// 定数バッファのインターフェース
class IConstantBuffer : public GpuBuffer 
{
public:
	// Constructor
	IConstantBuffer() : GpuBuffer()
	{
	}
	// Constructor
	IConstantBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
		GpuBuffer(pResource, CurrentState)
	{
	}
	// Destructor
	virtual ~IConstantBuffer() = default;
	// リソース作成
	virtual void CreateConstantBufferResource(ID3D12Device* device) = 0;
	// マッピング
	virtual void Unmap() = 0;
};

template<typename T>
class ConstantBuffer : public IConstantBuffer
{
public:
	// Constructor
	ConstantBuffer() : IConstantBuffer()
	{
	}
	// Constructor
	ConstantBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
		IConstantBuffer(pResource, CurrentState)
	{
	}
	// Destructor
	~ConstantBuffer()
	{
		m_MappedData = std::span<T>{};
	}
	// リソースを作成
	void CreateConstantBufferResource(ID3D12Device* device) override
	{
		// 構造体のサイズを確認
		if constexpr (std::is_class_v<T>)// クラス、構造体用
		{
			size_t size = sizeof(T);
			bool isValid = size % 16 != 0;
			if (isValid)
			{
				Log::Write(LogLevel::Assert, "Structure size must be multiple of 16 bytes");
			}
		}
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
		size_t bufferSize = 1;
		GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
		// マップしたデータをspanに変換
		m_MappedData = std::span<T>(mappedData, bufferSize);
	}
	void Unmap() override
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
	std::span<T> m_MappedData;
};

// 構造化バッファのインターフェース
class IStructuredBuffer : public GpuBuffer
{
public:
	// Constructor
	IStructuredBuffer() : GpuBuffer()
	{
	}
	// Constructor
	IStructuredBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
		GpuBuffer(pResource, CurrentState)
	{
	}
	// Destructor
	virtual ~IStructuredBuffer() = default;
	// リソース作成
	virtual void CreateStructuredBufferResource(ID3D12Device* device, const UINT& numElements) = 0;
	// SRV作成
	virtual bool CreateSRV(ID3D12Device8* device, D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, DescriptorHeap* pDescriptorHeap) = 0;
	// ディスクリプタハンドルを取得
	virtual D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCpuHandle() const = 0;
	virtual D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGpuHandle() const = 0;
	// ディスクリプタハンドルインデックスを取得
	virtual std::optional<uint32_t> GetSRVHandleIndex() const = 0;
};

template<typename T>
class StructuredBuffer : public IStructuredBuffer
{
public:
	// Constructor
	StructuredBuffer() : IStructuredBuffer()
	{
	}
	// Constructor
	StructuredBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
		IStructuredBuffer(pResource, CurrentState)
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
	void CreateStructuredBufferResource(ID3D12Device* device, const UINT& numElements) override
	{
		// 構造体のサイズを確認
		if constexpr (std::is_class_v<T>)// クラス、構造体用
		{
			size_t size = sizeof(T);
			bool isValid = size % 16 != 0;
			if (isValid)
			{
				Log::Write(LogLevel::Assert, "Structure size must be multiple of 16 bytes");
			}
		}
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
		m_MappedData = std::span<T>(mappedData, static_cast<size_t>(numElements));
	}
	bool CreateSRV(ID3D12Device8* device, D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, DescriptorHeap* pDescriptorHeap) override
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
	// ディスクリプタハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCpuHandle() const override { return m_SRVCpuHandle; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGpuHandle() const override { return m_SRVGpuHandle; }
	// ディスクリプタハンドルインデックスを取得
	std::optional<uint32_t> GetSRVHandleIndex() const override { return m_SRVHandleIndex; }
private:
	// マッピングデータ
	std::span<T> m_MappedData;
	// ディスクリプタハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE m_SRVCpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE m_SRVGpuHandle = {};
	// ディスクリプタハンドルインデックス
	std::optional<uint32_t> m_SRVHandleIndex = std::nullopt;
};

// UAVのインターフェース
class IRWStructuredBuffer : public GpuBuffer
{
public:
	// Constructor
	IRWStructuredBuffer() : GpuBuffer()
	{
	}
	// Constructor
	IRWStructuredBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
		GpuBuffer(pResource, CurrentState)
	{
	}
	// Destructor
	virtual ~IRWStructuredBuffer() = default;
	// リソース作成
	virtual void CreateRWStructuredBufferResource(ID3D12Device* device, const UINT& numElements) = 0;
	// SRV作成
	virtual bool CreateUAV(ID3D12Device8* device, D3D12_UNORDERED_ACCESS_VIEW_DESC& uavDesc, DescriptorHeap* pDescriptorHeap) = 0;
	// ディスクリプタハンドルを取得
	virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUAVCpuHandle() const = 0;
	virtual D3D12_GPU_DESCRIPTOR_HANDLE GetUAVGpuHandle() const = 0;
	// ディスクリプタハンドルインデックスを取得
	virtual std::optional<uint32_t> GetUAVHandleIndex() const = 0;
};

// UAVのクラス
template<typename T>
class RWStructuredBuffer : public IRWStructuredBuffer
{
public:
	// Constructor
	RWStructuredBuffer() : IRWStructuredBuffer()
	{
	}
	// Constructor
	RWStructuredBuffer(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
		IRWStructuredBuffer(pResource, CurrentState)
	{
	}
	// Destructor
	~RWStructuredBuffer()
	{
		m_UAVCpuHandle = {};
		m_UAVGpuHandle = {};
		m_UAVHandleIndex = std::nullopt;
	}
	void CreateRWStructuredBufferResource(ID3D12Device8* device, const UINT& numElements) override
	{
		// 構造体のサイズを確認
		if constexpr (std::is_class_v<T>)// クラス、構造体用
		{
			size_t size = sizeof(T);
			bool isValid = size % 16 != 0;
			if (isValid)
			{
				Log::Write(LogLevel::Assert, "Structure size must be multiple of 16 bytes");
			}
		}
		UINT structureByteStride = static_cast<UINT>(sizeof(T));
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// DefaultHeapを使う
		GpuBuffer::CreateBuffer(
			device, heapProperties, D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			numElements, structureByteStride);
	}
	bool CreateUAV(ID3D12Device8* device, D3D12_UNORDERED_ACCESS_VIEW_DESC& uavDesc, DescriptorHeap* pDescriptorHeap) override
	{
		// ヒープがUAVタイプかどうか確認
		if (pDescriptorHeap->GetType() != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		{
			Log::Write(LogLevel::Assert, "DescriptorHeap is not UAV type");
			return false;
		}
		// リソースがあるかどうか確認
		if (!GetResource())
		{
			Log::Write(LogLevel::Assert, "Resource is null");
			return false;
		}
		// 新しいディスクリプタのインデッ���スとハンドルを取得
		if (!m_UAVHandleIndex.has_value())
		{
			m_UAVHandleIndex = pDescriptorHeap->Allocate();
		}
		if (!m_UAVHandleIndex.has_value())
		{
			Log::Write(LogLevel::Warn, "DescriptorHeap is full");
			return false;
		}
		m_UAVCpuHandle = pDescriptorHeap->GetCPUDescriptorHandle(m_UAVHandleIndex.value());
		m_UAVGpuHandle = pDescriptorHeap->GetGPUDescriptorHandle(m_UAVHandleIndex.value());
		device->CreateUnorderedAccessView(
			GetResource(),
			nullptr,
			&uavDesc,
			m_UAVCpuHandle
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
	// ディスクリプタハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetUAVCpuHandle() const override { return m_UAVCpuHandle; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetUAVGpuHandle() const override { return m_UAVGpuHandle; }
	// ディスクリプタハンドルインデックスを取得
	std::optional<uint32_t> GetUAVHandleIndex() const override { return m_UAVHandleIndex; }
private:
	// ディスクリプタハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE m_UAVCpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE m_UAVGpuHandle = {};
	// ディスクリプタハンドルインデックス
	std::optional<uint32_t> m_UAVHandleIndex = std::nullopt;
};
