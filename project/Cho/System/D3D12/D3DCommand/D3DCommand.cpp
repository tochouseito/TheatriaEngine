#include "PrecompiledHeader.h"
#include "D3DCommand.h"
#include<assert.h>

void D3DCommand::Initialize(ID3D12Device8& device)
{
	CreateFences(device);

	// コマンドキューを生成する
	CreateQueues(device);

	CreateCommands(device);
}

void D3DCommand::Close(const QueueType& queueType, const CommandType& commandType)
{
	HRESULT hr;
	// コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	hr = commands[commandType].list->Close();
	assert(SUCCEEDED(hr));

	// GPUにコマンドリストの実行を行わせる
	Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = { commands[commandType].list};
	queues[queueType].queue->ExecuteCommandLists(1, commandLists->GetAddressOf());
}

void D3DCommand::Signal(const QueueType& type)
{
	FenceValueUpdate(type);

	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	queues[type].queue->Signal(GetFence(type), GetValue(type));

	WaitForSingle(type);
}

void D3DCommand::Reset(const CommandType& type)
{
	HRESULT hr;
	// 次のフレーム用のコマンドリストを準備
	hr = commands[type].allocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commands[type].list->Reset(commands[type].allocator.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

void D3DCommand::Finalize()
{
	for (int queue = QueueType::DIRECT; queue < QueueType::TypeCount; ++queue) {
		QueueType currentQueue = static_cast<QueueType>(queue);
		switch (currentQueue)
		{
		case DIRECT:
			CloseHandle(queues[queue].fenceEvent);
			break;
		case COMPUTE:
			CloseHandle(queues[queue].fenceEvent);
			break;
		case COPY:
			CloseHandle(queues[queue].fenceEvent);
			break;
		case TypeCount:
			break;
		default:
			break;
		}
	}
}

void D3DCommand::BarrierTransition(const CommandType& type, ID3D12Resource* pResource, D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After)
{
	ID3D12GraphicsCommandList6* commandList = GetCommand(type).list.Get();

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};

	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = pResource;

	// 遷移前（現在）のResourceState
	barrier.Transition.StateBefore = Before;

	// 遷移後のResourceState
	barrier.Transition.StateAfter = After;

	// TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);
}

void D3DCommand::CreateFences(ID3D12Device8& device)
{
	HRESULT hr;
	for (int queue = QueueType::DIRECT; queue < QueueType::TypeCount; ++queue) {
		QueueType currentQueue = static_cast<QueueType>(queue);
		switch (currentQueue)
		{
		case DIRECT:
			// 初期値0でFenceを作る
			queues[queue].fence = nullptr;
			hr = device.CreateFence(queues[queue].fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&queues[queue].fence));
			assert(SUCCEEDED(hr));

			// FenceのSignalを持つためのイベントを作成する
			queues[queue].fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			assert(queues[queue].fenceEvent != nullptr);
			break;
		case COMPUTE:
			// 初期値0でFenceを作る
			queues[queue].fence = nullptr;
			hr = device.CreateFence(queues[queue].fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&queues[queue].fence));
			assert(SUCCEEDED(hr));

			// FenceのSignalを持つためのイベントを作成する
			queues[queue].fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			assert(queues[queue].fenceEvent != nullptr);
			break;
		case COPY:
			// 初期値0でFenceを作る
			queues[queue].fence = nullptr;
			hr = device.CreateFence(queues[queue].fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&queues[queue].fence));
			assert(SUCCEEDED(hr));

			// FenceのSignalを持つためのイベントを作成する
			queues[queue].fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			assert(queues[queue].fenceEvent != nullptr);
			break;
		case TypeCount:
			break;
		default:
			break;
		}
	}
}

void D3DCommand::FenceValueUpdate(const QueueType& type)
{
	queues[type].fenceValue++;
}

void D3DCommand::WaitForSingle(const QueueType& type)
{
	// Fenceの値が指定したSignal値にたどり着いているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (queues[type].fence->GetCompletedValue() < queues[type].fenceValue) {

		// 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		queues[type].fence->SetEventOnCompletion(queues[type].fenceValue, queues[type].fenceEvent);

		// イベント待つ
		WaitForSingleObject(queues[type].fenceEvent, INFINITE);
	}
}

void D3DCommand::CreateQueues(ID3D12Device8& device)
{
	HRESULT hr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	for (int queue = QueueType::DIRECT; queue < QueueType::TypeCount; ++queue) {
		QueueType currentQueue = static_cast<QueueType>(queue);
		switch (currentQueue)
		{
		case DIRECT:
			commandQueueDesc.Type= D3D12_COMMAND_LIST_TYPE_DIRECT;
			hr = device.CreateCommandQueue(
				&commandQueueDesc,
				IID_PPV_ARGS(&queues[queue].queue)
			);
			assert(SUCCEEDED(hr));
			break;
		case COMPUTE:
			commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
			hr = device.CreateCommandQueue(
				&commandQueueDesc,
				IID_PPV_ARGS(&queues[queue].queue)
			);
			break;
		case COPY:
			commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			hr = device.CreateCommandQueue(
				&commandQueueDesc,
				IID_PPV_ARGS(&queues[queue].queue)
			);
			break;
		case TypeCount:
		default:
			break;
		}
	}
}

void D3DCommand::CreateCommands(ID3D12Device8& device)
{
	for (int type = CommandType::Draw; type <= CommandType::Copy; ++type) {
		CommandType currentType = static_cast<CommandType>(type);

		switch (currentType)
		{
		case Draw:
			CreateCommand(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
			break;
		case Compute:
			CreateCommand(device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
			break;
		case Copy:
			CreateCommand(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
			break;
		default:
			break;
		}
	}
}

void D3DCommand::CreateCommand(ID3D12Device8& device, const D3D12_COMMAND_LIST_TYPE& type)
{
	HRESULT hr;
	Command command;
	// コマンドアロケータを生成する
	hr = device.CreateCommandAllocator(
		type,
		IID_PPV_ARGS(&command.allocator)
	);
	// コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドリストを生成する
	hr = device.CreateCommandList(
		0,
		type,
		command.allocator.Get(),
		nullptr,
		IID_PPV_ARGS(&command.list)
	);
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	command.list->Close();  // 初期状態で閉じておく
	commands.push_back(command);
}
