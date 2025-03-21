#pragma once

#include "SDK/DirectX/DirectX12/stdafx/stdafx.h"

class GpuResource
{
public:
    // Constructor
    GpuResource() :
        m_GpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS_NULL),
        m_UsageState(D3D12_RESOURCE_STATE_COMMON),
        m_TransitioningState((D3D12_RESOURCE_STATES)-1)
    {
    }

    // Constructor
    GpuResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) :
        m_GpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS_NULL),
        m_pResource(pResource),
        m_UsageState(CurrentState),
        m_TransitioningState((D3D12_RESOURCE_STATES)-1)
    {
    }

    // Destructor
    ~GpuResource() { Destroy(); }

    virtual void Destroy()
    {
        m_pResource = nullptr;
        m_GpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
        ++m_VersionID;
        m_DHandleIndex = UINT32_MAX;
    }

    ID3D12Resource* operator->() { return m_pResource.Get(); }
    const ID3D12Resource* operator->() const { return m_pResource.Get(); }

    ID3D12Resource* GetResource() { return m_pResource.Get(); }
    const ID3D12Resource* GetResource() const { return m_pResource.Get(); }

    ID3D12Resource** GetAddressOf() { return m_pResource.GetAddressOf(); }

    D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return m_GpuVirtualAddress; }

    uint32_t GetVersionID() const { return m_VersionID; }

    void Create(
        ID3D12Device* device,
        D3D12_HEAP_PROPERTIES& heapProperties,
        D3D12_HEAP_FLAGS heapFlags,
        D3D12_RESOURCE_DESC& desc,
        D3D12_RESOURCE_STATES InitialState,
        D3D12_CLEAR_VALUE* pClearValue = nullptr
    );
	void UnMap() { m_pResource->Unmap(0, nullptr); }
	uint32_t GetDHandleIndex() const { return m_DHandleIndex; }
	void SetDHandleIndex(const uint32_t& index) { m_DHandleIndex = index; }
	D3D12_RESOURCE_STATES GetUsageState() const { return m_UsageState; }
	void SetResourceState(const D3D12_RESOURCE_STATES& state) { m_UsageState = state; }

protected:

    ComPtr<ID3D12Resource> m_pResource;
	D3D12_RESOURCE_STATES m_UsageState;// リソースの使用状態
	D3D12_RESOURCE_STATES m_TransitioningState;// 遷移直前の状態
    D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;
	uint32_t m_VersionID = 0;// このリソースのバージョンID。リソースが再利用されるとインクリメントされます。
	uint32_t m_DHandleIndex = UINT32_MAX;// このリソースがディスクリプタヒープに割り当てられたインデックス
};

