#include "pch.h"
#include "CommandContext.h"

void CommandContext::Create(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
{
	HRESULT hr = {};
	// コマンドアロケータを生成する
	hr = device->CreateCommandAllocator(
		type,
		IID_PPV_ARGS(&m_CommandAllocator)
	);
	// コマンドアロケータの生成がうまくいかなかったので起動できない
	ChoAssertLog("Failed to create command allocator.", hr, __FILE__, __LINE__);

	// コマンドリストを生成する
	hr = device->CreateCommandList(
		0,
		type,
		m_CommandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&m_CommandList)
	);
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	m_Type = type;
	m_CommandList->Close();  // 初期状態で閉じておく
}

CommandContext::~CommandContext()
{
}

void CommandContext::Reset()
{
}

void CommandContext::Close()
{
}

void CommandContext::Flush()
{
}

GraphicsContext::GraphicsContext(ID3D12Device* device)
{
	Create(device, D3D12_COMMAND_LIST_TYPE_DIRECT);	
}

GraphicsContext::~GraphicsContext()
{
}

ComputeContext::ComputeContext(ID3D12Device* device)
{
	Create(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
}

ComputeContext::~ComputeContext()
{
}

CopyContext::CopyContext(ID3D12Device* device)
{
	Create(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
}

CopyContext::~CopyContext()
{
}
