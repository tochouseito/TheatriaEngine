#pragma once

#include "SDK/DirectX/DirectX12/stdafx/stdafx.h"

class DescriptorHandle 
{
public:// methods
	// Constructor
	DescriptorHandle();
	// Constructor
	DescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);
	// GetCPUHandle
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const { return m_CpuHandle; }
	// GetGPUHandle
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const { return m_GpuHandle; }
	// GetCpuPtr
	size_t GetCpuPtr() const { return m_CpuHandle.ptr; }
	// GetGpuPtr
	uint64_t GetGpuPtr() const { return m_GpuHandle.ptr; }
	// NullCheck
	//bool IsNull() const{ return m_CpuHandle.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN; }
	//bool IsShaderVisible() const { return m_GpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN; }
private:// members
	// CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle = {};
	// GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE m_GpuHandle = {};
};

class DescriptorHeap
{
public:// methods
	// Destructor
	virtual ~DescriptorHeap();
	// GetMaxDescriptor
	virtual uint32_t GetMaxDescriptor() const;
	// GetType
	virtual D3D12_DESCRIPTOR_HEAP_TYPE GetType() const;
	// GetSize
	virtual uint32_t GetSize() const;
	// GetDescriptorHeap
	virtual ID3D12DescriptorHeap* GetDescriptorHeap() const;
protected:// members
	// 最大ディスクリプタ数
	uint32_t m_MaxDescriptor = {};
	// ヒープタイプ
	D3D12_DESCRIPTOR_HEAP_TYPE m_Type = {};
	// デスクリプタサイズ
	uint32_t m_Size = {};
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap = {};
};

/*============================================*/
/*
 * This class is SRV, UAV, CBV Descriptor Pool
*/
/*============================================*/

class SUVDescriptorHeap : public DescriptorHeap
{
public:// methods
	// Constructor
	SUVDescriptorHeap(ID3D12Device8* device, const uint32_t& maxDescriptor);
	// Destructor
	~SUVDescriptorHeap();
private:// members
};

class RTVDescriptorHeap : public DescriptorHeap
{
public:// methods
	// Constructor
	RTVDescriptorHeap(ID3D12Device8* device, const uint32_t& maxDescriptor);
	// Destructor
	~RTVDescriptorHeap();
private:// members
};

class DSVDescriptorHeap : public DescriptorHeap
{
public:// methods
	// Constructor
	DSVDescriptorHeap(ID3D12Device8* device, const uint32_t& maxDescriptor);
	// Destructor
	~DSVDescriptorHeap();
private:// members
};