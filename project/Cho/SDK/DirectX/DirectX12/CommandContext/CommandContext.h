#pragma once

#include "SDK/DirectX/DirectX12/stdafx/stdafx.h"
#include <vector>
#include <array>
#include <queue>
#include <mutex>
#include <condition_variable>

class CommandContext {
public:
	virtual void Create(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
	virtual ~CommandContext();
	virtual void Reset();
	virtual void Close();
	virtual void Flush();
	virtual ID3D12GraphicsCommandList6* GetCommandList() { return m_CommandList.Get(); };
	virtual D3D12_COMMAND_LIST_TYPE GetType() { return m_Type; };

	virtual void SetDescriptorHeap(ID3D12DescriptorHeap* heap);
	virtual void BarrierTransition(ID3D12Resource* pResource, D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After);
	virtual void ResourceBarrier(UINT NumBarriers, const D3D12_RESOURCE_BARRIER* pBarriers);
	virtual void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle = nullptr);
	virtual void ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE handle);
	virtual void ClearDepthStencil(D3D12_CPU_DESCRIPTOR_HANDLE handle);
	virtual void SetViewport(const D3D12_VIEWPORT& viewport);
	virtual void SetScissorRect(const D3D12_RECT& rect);
	virtual void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);
	virtual void SetGraphicsPipelineState(ID3D12PipelineState* pso);
	virtual void SetGraphicsRootSignature(ID3D12RootSignature* rootSignature);
	virtual void SetVertexBuffers(UINT StartSlot, UINT Count, const D3D12_VERTEX_BUFFER_VIEW* pViews);
	virtual void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* pView);
	virtual void SetGraphicsRootConstantBufferView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation);
	virtual void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor);
	virtual void DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation);
	virtual void DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation);
protected:
	ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList6> m_CommandList;
	D3D12_COMMAND_LIST_TYPE m_Type = {};
};

class GraphicsContext : public CommandContext {
public:
	GraphicsContext(ID3D12Device* device);
	~GraphicsContext();
private:
};

class ComputeContext : public CommandContext {
public:
	ComputeContext(ID3D12Device* device);
	~ComputeContext();
private:
};



//class CopyContext : public CommandContext {
//public:
//	CopyContext(ID3D12Device* device);
//	~CopyContext();
//private:
//};
