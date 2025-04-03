#pragma once

#include "SDK/DirectX/DirectX12/stdafx/stdafx.h"

class DescriptorHeap
{
public:// methods
	// Constructor
	DescriptorHeap(ID3D12Device8* device, const uint32_t& maxDescriptor, D3D12_DESCRIPTOR_HEAP_TYPE heapType, const bool& shaderVisible)
		: m_MaxDescriptor(maxDescriptor), m_Type(heapType), m_NextHandleIndex(0)
	{
		CreateDescriptor(device, maxDescriptor, heapType, shaderVisible);
	}
	// Destructor
	~DescriptorHeap()
	{

	}
	// GetMaxDescriptor
	uint32_t GetMaxDescriptor() const;
	// GetType
	D3D12_DESCRIPTOR_HEAP_TYPE GetType() const;
	// GetSize
	uint32_t GetSize() const;
	// GetDescriptorHeap
	ID3D12DescriptorHeap* GetDescriptorHeap() const;
	// GetHandle
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const uint32_t& index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const uint32_t& index);
	// AllocateHandle
	std::optional<uint32_t> Allocate();
	// RemoveHandle
	bool RemoveHandle(const uint32_t& index);
private:
	// CreateDescriptor
	void CreateDescriptor(ID3D12Device8* device, const uint32_t& num, D3D12_DESCRIPTOR_HEAP_TYPE type, const bool& shaderVisible);

	// 最大ディスクリプタ数
	uint32_t m_MaxDescriptor;
	// ヒープタイプ
	D3D12_DESCRIPTOR_HEAP_TYPE m_Type;
	// デスクリプタサイズ
	uint32_t m_Size = {};
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap = nullptr;
	// 次の使用可能なインデックス
	uint32_t m_NextHandleIndex;
	// 返却されたインデックス
	std::vector<uint32_t> m_RemovedHandleIndex = {};
};