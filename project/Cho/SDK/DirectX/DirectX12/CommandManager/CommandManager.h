#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <array>
#include <queue>
#include <mutex>
#include <condition_variable>

using namespace Microsoft::WRL;

enum QueueType {
	Graphics,
	Compute,
	Copy,
	kQueueTypeCount,
};

class QueueContext {
public:
	QueueContext(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
	~QueueContext();

	void Initialize();
	void Reset();
	ID3D12CommandQueue* GetCommandQueue() { return m_CommandQueue.Get(); };
	ID3D12Fence* GetFence() { return m_Fence.Get(); };
	uint64_t GetFenceValue() { return m_FenceValue; };

private:
    ComPtr<ID3D12CommandQueue> m_CommandQueue = nullptr;
    ComPtr<ID3D12Fence> m_Fence = nullptr;
    HANDLE m_FenceEvent = {};
    uint64_t m_FenceValue = {};
	std::mutex m_FenceMutex;
	std::condition_variable m_FenceCV;
};

class CommandManager
{
public:
    CommandManager(ID3D12Device8* device);
    ~CommandManager();

    void Initialize();
    void Shutdown();

    // コマンドキュー取得
	ID3D12CommandQueue* GetCommandQueue(const D3D12_COMMAND_LIST_TYPE& type);

 //   // コマンドアロケータの取得
 //   ID3D12CommandAllocator* GetAllocator(const D3D12_COMMAND_LIST_TYPE& type);

	//// コマンドリストの取得
	//ID3D12GraphicsCommandList6* GetCommandList(const D3D12_COMMAND_LIST_TYPE& type);

    // コマンドリストの実行
    uint64_t ExecuteCommandList(ID3D12GraphicsCommandList6* commandList, const D3D12_COMMAND_LIST_TYPE& type);

    // GPU 完了待機
    void WaitForFence(const uint64_t& fenceValue, const D3D12_COMMAND_LIST_TYPE& type);

private:
	// コマンドキューコンテキストの生成
	void CreateQueueContexts(ID3D12Device8* device);
private:
    // コマンドキューコンテキスト
	std::array<std::unique_ptr<QueueContext>, kQueueTypeCount> m_QueueContexts;

	// コマンドコンテキスト
	//std::vector<std::unique_ptr<CommandContext>> m_CommandContexts;
};

