#pragma once

#include "Cho/SDK/DirectX/DirectX12/CommandContext/CommandContext.h"

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
	void ExecuteCommandLists(ID3D12GraphicsCommandList6* commandList);
	void Signal();
	void WaitForFence();

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

class CommandPool
{
public:
	CommandPool(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type, size_t poolSize);
	~CommandPool();

	CommandContext* GetContext();
	void ReturnContext(CommandContext* context);
private:
	std::queue<std::unique_ptr<CommandContext>> m_CommandPool;
	std::mutex m_Mutex;
	ID3D12Device* m_Device;
	D3D12_COMMAND_LIST_TYPE m_Type;
};

class CommandManager
{
public:
    CommandManager(ID3D12Device8* device);
    ~CommandManager();

    void Initialize();
    void Shutdown();

    // コマンドキュー取得
	ID3D12CommandQueue* GetCommandQueue(QueueType type);
    // コマンドリストの実行
    uint64_t ExecuteCommandList(ID3D12GraphicsCommandList6* commandList, QueueType type);
	// シグナル
	void Signal(QueueType type);
    // GPU 完了待機
    void WaitForFence(QueueType type);
	// コマンドリスト取得
	CommandContext* GetCommandCont
		ext() { return m_CommandPool->GetContext(); }
	// コマンドリスト返却
	void ReturnCommandContext(CommandContext* context) { m_CommandPool->ReturnContext(context); }
private:
	// コマンドキューコンテキストの生成
	void CreateQueueContexts(ID3D12Device8* device);
private:
	ID3D12Device8* m_Device = nullptr;
    // コマンドキューコンテキスト
	std::array<std::unique_ptr<QueueContext>, kQueueTypeCount> m_QueueContexts;
	// コマンドプール
	std::unique_ptr<CommandPool> m_CommandPool;
};

