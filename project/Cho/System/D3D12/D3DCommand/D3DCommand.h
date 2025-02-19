#pragma once

#include<d3d12.h>
#include<wrl.h>
#include<vector>
#include<array>

enum QueueType {
	DIRECT = 0,
	COMPUTE,
	COPY,
	TypeCount,// カウント用。使用禁止
};

enum CommandType {
	Draw=0,
	Compute,
	Copy,
};

struct Queue {
	Microsoft::WRL::ComPtr < ID3D12CommandQueue> queue=nullptr;
	Microsoft::WRL::ComPtr <ID3D12Fence>fence=nullptr;
	HANDLE fenceEvent = 0;
	uint64_t fenceValue = 0;
};

struct Command {
	Microsoft::WRL::ComPtr < ID3D12CommandAllocator>allocator = nullptr;
	Microsoft::WRL::ComPtr < ID3D12GraphicsCommandList6>list = nullptr;
};

class D3DCommand
{
public:// メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ID3D12Device8& device);

	void Close(const QueueType& queueType, const CommandType& commandType);

	void Signal(const QueueType& type);

	void Reset(const CommandType& type);

	void Finalize();

	ID3D12CommandQueue* GetCommandQueue(const QueueType& type)const { return queues[type].queue.Get(); }

	Command GetCommand(const CommandType& type)const { return commands[type]; }

	ID3D12Fence* GetFence(const QueueType& type)const { return queues[type].fence.Get(); }
	uint64_t GetValue(const QueueType& type)const { return queues[type].fenceValue; }

	void BarrierTransition(const CommandType& type,ID3D12Resource* pResource, D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After);

private:

	/*フェンス*/
	void CreateFences(ID3D12Device8& device);
	void FenceValueUpdate(const QueueType& type);
	void WaitForSingle(const QueueType& type);

	void CreateQueues(ID3D12Device8& device);

	void CreateCommands(ID3D12Device8& device);

	void CreateCommand(ID3D12Device8& device, const D3D12_COMMAND_LIST_TYPE& type);

private:// メンバ変数
	/*フェンス*/
	std::array< Queue, TypeCount> queues;
	/*コマンド*/
	std::vector<Command> commands;
};

