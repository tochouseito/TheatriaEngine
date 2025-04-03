#pragma once

#include "SDK/DirectX/DirectX12/DescriptorHeap/DescriptorHeap.h"

class GpuResource
{
public:
    // Constructor
    GpuResource():
		m_UseState(D3D12_RESOURCE_STATE_COMMON)
	{
	}
	// Constructor
    GpuResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
        m_pResource(pResource), m_UseState(CurrentState)
    {

    }
    // Destructor
    virtual ~GpuResource()
    {
        Destroy();
    }
    virtual void Destroy()
    {
        m_pResource = nullptr;
		m_UseState = D3D12_RESOURCE_STATE_COMMON;
        ++m_VersionID;
    }
	// 直接リソースを取得する演算子
    ID3D12Resource* operator->() { return m_pResource.Get(); }
    const ID3D12Resource* operator->() const { return m_pResource.Get(); }
	// リソースを取得
    ID3D12Resource* GetResource() { return m_pResource.Get(); }
    const ID3D12Resource* GetResource() const { return m_pResource.Get(); }
	// リソースのポインタを取得
    ID3D12Resource** GetAddressOf() { return m_pResource.GetAddressOf(); }
	// リソースのバージョンIDを取得
    uint32_t GetVersionID() const { return m_VersionID; }
	// リソースを作成
    void CreateResource(
        ID3D12Device* device,
        D3D12_HEAP_PROPERTIES& heapProperties,
        D3D12_HEAP_FLAGS heapFlags,
        D3D12_RESOURCE_DESC& desc,
        D3D12_RESOURCE_STATES InitialState,
        D3D12_CLEAR_VALUE* pClearValue = nullptr
    );
protected:
    // リソース
    ComPtr<ID3D12Resource> m_pResource = nullptr;
    // リソースの使用状態
    D3D12_RESOURCE_STATES m_UseState;
    // このリソースのバージョンID。リソースが再利用されるとインクリメントされます。
	uint32_t m_VersionID = 0;
};

