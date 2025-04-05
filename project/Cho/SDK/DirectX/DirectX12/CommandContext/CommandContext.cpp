#include "pch.h"
#include "CommandContext.h"
#include "Core/ChoLog/ChoLog.h"
using namespace Cho;

void CommandContext::Create(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
{
	HRESULT hr = {};
	// コマンドアロケータを生成する
	hr = device->CreateCommandAllocator(
		type,
		IID_PPV_ARGS(&m_CommandAllocator)
	);
	// コマンドアロケータの生成がうまくいかなかったので起動できない
	Log::Write(LogLevel::Assert, "CommandAllocator created.", hr);

	// コマンドリストを生成する
	hr = device->CreateCommandList(
		0,
		type,
		m_CommandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&m_CommandList)
	);
	// コマンドリストの生成がうまくいかなかったので起動できない
	Log::Write(LogLevel::Assert, "CommandList created.", hr);

	m_Type = type;
	m_CommandList->Close();  // 初期状態で閉じておく
}

CommandContext::~CommandContext()
{
}

void CommandContext::Reset()
{
	HRESULT hr = {};
	// コマンドアロケータをリセットする
	hr = m_CommandAllocator->Reset();
	// コマンドアロケータのリセットがうまくいかなかったので起動できない
	Log::Write(LogLevel::Assert, "CommandAllocator reset.", hr);
	// コマンドリストをリセットする
	hr = m_CommandList->Reset(m_CommandAllocator.Get(), nullptr);
	// コマンドリストのリセットがうまくいかなかったので起動できない
	Log::Write(LogLevel::Assert, "CommandList reset.", hr);
}

void CommandContext::Close()
{
	HRESULT hr = {};
	// コマンドリストを閉じる
	hr = m_CommandList->Close();
	// コマンドリストのクローズがうまくいかなかったので起動できない
	Log::Write(LogLevel::Assert, "CommandList closed.", hr);
}

void CommandContext::Flush()
{
}

void CommandContext::SetDescriptorHeap(ID3D12DescriptorHeap* heap)
{
	// ディスクリプタヒープを設定する
	ID3D12DescriptorHeap* heaps[] = { heap };
	// コマンドリストにディスクリプタヒープを設定する
	m_CommandList->SetDescriptorHeaps(_countof(heaps), heaps);
}

void CommandContext::BarrierTransition(ID3D12Resource* pResource, D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After)
{
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
	m_CommandList->ResourceBarrier(1, &barrier);
}

void CommandContext::ResourceBarrier(UINT NumBarriers, const D3D12_RESOURCE_BARRIER* pBarriers)
{
	// リソースバリアの設定
	m_CommandList->ResourceBarrier(NumBarriers, pBarriers);
}

void CommandContext::SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle)
{
	// レンダーターゲットビューの設定
	m_CommandList->OMSetRenderTargets(1, rtvHandle, false, dsvHandle);
}

void CommandContext::ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	// 指定した色で画面全体をクリアする
	m_CommandList->ClearRenderTargetView(handle, kClearColor, 0, nullptr);
}

void CommandContext::ClearDepthStencil(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	// 深度ステンシルビューのクリア
	m_CommandList->ClearDepthStencilView(handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void CommandContext::SetViewport(const D3D12_VIEWPORT& viewport)
{
	// ビューポートの設定
	m_CommandList->RSSetViewports(1, &viewport);
}

void CommandContext::SetScissorRect(const D3D12_RECT& rect)
{
	// シザーレクトの設定
	m_CommandList->RSSetScissorRects(1, &rect);
}

void CommandContext::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology)
{
	// プリミティブトポロジーの設定
	m_CommandList->IASetPrimitiveTopology(topology);
}

void CommandContext::SetGraphicsPipelineState(ID3D12PipelineState* pso)
{
	// パイプラインステートの設定
	m_CommandList->SetPipelineState(pso);
}

void CommandContext::SetGraphicsRootSignature(ID3D12RootSignature* rootSignature)
{
	// ルートシグネチャの設定
	m_CommandList->SetGraphicsRootSignature(rootSignature);
}

void CommandContext::SetVertexBuffers(UINT StartSlot, UINT Count, const D3D12_VERTEX_BUFFER_VIEW* pViews)
{
	// 頂点バッファの設定
	m_CommandList->IASetVertexBuffers(StartSlot, Count, pViews);
}

void CommandContext::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* pView)
{
	// インデックスバッファの設定
	m_CommandList->IASetIndexBuffer(pView);
}

void CommandContext::SetGraphicsRootConstantBufferView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
{
	// ルート定数バッファビューの設定
	m_CommandList->SetGraphicsRootConstantBufferView(RootParameterIndex, BufferLocation);
}

void CommandContext::SetGraphicsRootDescriptorTable(UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor)
{
	// ルートディスクリプタテーブルの設定
	m_CommandList->SetGraphicsRootDescriptorTable(RootParameterIndex, BaseDescriptor);
}

void CommandContext::DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation)
{
	// 描画
	m_CommandList->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

void CommandContext::DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
{
	// インデックス付き描画
	m_CommandList->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
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
