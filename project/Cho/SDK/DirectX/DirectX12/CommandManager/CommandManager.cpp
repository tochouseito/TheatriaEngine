#include "pch.h"
#include "CommandManager.h"

CommandManager::CommandManager(ID3D12Device8* device)
{
	CreateQueueContexts(device);// デバイスを受け取り、キューコンテキストを作成する
}

CommandManager::~CommandManager()
{
}

void CommandManager::Initialize()
{
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
	ChoAssertLog("Failed to create fence event.", m_FenceEvent != nullptr, __FILE__, __LINE__);

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