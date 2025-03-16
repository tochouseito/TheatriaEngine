#include "pch.h"
#include "CommandManager.h"
#include "Cho/Core/ThreadManager/ThreadManager.h"

CommandManager::CommandManager(ID3D12Device8* device)
{
	m_Device = device;
	CreateQueueContexts(device);// デバイスを受け取り、キューコンテキストを作成する
	Initialize();// 初期化
}

CommandManager::~CommandManager()
{
}

void CommandManager::Initialize()
{
	m_CommandPool = std::make_unique<CommandPool>(m_Device, D3D12_COMMAND_LIST_TYPE_DIRECT, ThreadManager::GetInstance().GetThreadCount());
}

void CommandManager::Shutdown()
{
}

ID3D12CommandQueue* CommandManager::GetCommandQueue(QueueType type)
{
	return m_QueueContexts[type]->GetCommandQueue();
}

uint64_t CommandManager::ExecuteCommandList(ID3D12GraphicsCommandList6* commandList, const D3D12_COMMAND_LIST_TYPE& type)
{
	commandList;
	switch (type)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
		break;
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		break;
	case D3D12_COMMAND_LIST_TYPE_COPY:
		break;
	default:
		break;
	}

	return 0;
}

void CommandManager::WaitForFence(const uint64_t& fenceValue, const D3D12_COMMAND_LIST_TYPE& type)
{
	fenceValue;
	switch (type)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
		break;
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		break;
	case D3D12_COMMAND_LIST_TYPE_COPY:
		break;
	default:
		break;
	}
}

void CommandManager::CreateQueueContexts(ID3D12Device8* device)
{
	for (uint32_t i = 0;i < kQueueTypeCount;++i)
	{
		m_QueueContexts[i] = std::make_unique<QueueContext>(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	}
}

QueueContext::QueueContext(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
{
	HRESULT hr = {};
	/*FenceCreate*/
	// 初期値0でFenceを作る
	m_Fence = nullptr;
	hr = device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
	ChoAssertLog("Failed to create fence.", hr, __FILE__, __LINE__);
	// FenceのSignalを持つためのイベントを作成する
	m_FenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	ChoAssertLog("Failed to create fence event.", static_cast<bool>(m_FenceEvent != nullptr), __FILE__, __LINE__);

	// コマンドキューの作成
	D3D12_COMMAND_QUEUE_DESC desc{};
	desc.Type = type;
	hr = device->CreateCommandQueue(
		&desc,
		IID_PPV_ARGS(&m_CommandQueue)
	);
	ChoAssertLog("Failed to create command queue.", hr, __FILE__, __LINE__);
}

QueueContext::~QueueContext()
{
	CloseHandle(m_FenceEvent);
}

void QueueContext::Initialize()
{
}

void QueueContext::Reset()
{
}

CommandPool::CommandPool(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type, size_t poolSize)
	: m_Device(device), m_Type(type)
{
	for (size_t i = 0; i < poolSize; ++i)
	{
		auto context = std::make_unique<CommandContext>();
		context->Create(device, type);
		m_CommandPool.push(std::move(context));
	}
}

CommandPool::~CommandPool()
{
}

CommandContext* CommandPool::GetContext()
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	if (m_CommandPool.empty())
	{
		auto context = std::make_unique<CommandContext>();
		context->Create(m_Device, m_Type);
		return context.release();
	}
	auto context = std::move(m_CommandPool.front());
	m_CommandPool.pop();
	return context.release();
}

void CommandPool::ReturnContext(CommandContext* context)
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	context->Reset();
	m_CommandPool.push(std::unique_ptr<CommandContext>(context));
}
