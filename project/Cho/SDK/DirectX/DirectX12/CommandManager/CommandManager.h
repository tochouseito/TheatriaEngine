#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

using namespace Microsoft::WRL;

class CommandManager
{
public:
    CommandManager();
    ~CommandManager();

    void Initialize(ID3D12Device* device);
    void Shutdown();

    // コマンドキュー取得
    ID3D12CommandQueue* GetGraphicsQueue() { return m_GraphicsQueue.Get(); }
    ID3D12CommandQueue* GetComputeQueue() { return m_ComputeQueue.Get(); }
    ID3D12CommandQueue* GetCopyQueue() { return m_CopyQueue.Get(); }

    // コマンドアロケータの取得
    ID3D12CommandAllocator* GetAllocator(const D3D12_COMMAND_LIST_TYPE& type);

    // コマンドリストの実行
    uint64_t ExecuteCommandList(ID3D12GraphicsCommandList6* commandList, const D3D12_COMMAND_LIST_TYPE& type);

    // GPU 完了待機
    void WaitForFence(const uint64_t& fenceValue, const D3D12_COMMAND_LIST_TYPE& type);

private:
    ComPtr<ID3D12Device> m_Device;

    // コマンドキュー
    ComPtr<ID3D12CommandQueue> m_GraphicsQueue;
    ComPtr<ID3D12CommandQueue> m_ComputeQueue;
    ComPtr<ID3D12CommandQueue> m_CopyQueue;

    // フェンス
    ComPtr<ID3D12Fence> m_GraphicsFence;
    ComPtr<ID3D12Fence> m_ComputeFence;
    ComPtr<ID3D12Fence> m_CopyFence;
    UINT64 m_GraphicsFenceValue;
    UINT64 m_ComputeFenceValue;
    UINT64 m_CopyFenceValue;

    // コマンドアロケータ管理
    std::queue<ComPtr<ID3D12CommandAllocator>> m_GraphicsAllocators;
    std::queue<ComPtr<ID3D12CommandAllocator>> m_ComputeAllocators;
    std::queue<ComPtr<ID3D12CommandAllocator>> m_CopyAllocators;

    std::mutex m_AllocatorMutex;
    std::condition_variable m_AllocatorCV;
};

