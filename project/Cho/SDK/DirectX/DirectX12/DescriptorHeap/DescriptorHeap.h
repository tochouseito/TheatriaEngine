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
	// SetCPUHandle
	void SetCPUHandle(const D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle) { m_CpuHandle = cpuHandle; }
	// SetGPUHandle
	void SetGPUHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle) { m_GpuHandle = gpuHandle; }

	// NullCheck
	//bool IsNull() const{ return m_CpuHandle.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN; }
	//bool IsShaderVisible() const { return m_GpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN; }
private:// members
	// CPUHandle
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle = {};
	// GPUHandle
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
	// GetHandle
	virtual DescriptorHandle GetHandle(const uint32_t& index) const { return m_Handles[index]; }
	virtual D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(const uint32_t& index) { return m_Handles[index].GetCpuHandle(); }
	virtual D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(const uint32_t& index) { return m_Handles[index].GetGpuHandle(); }
	// CreateView
	virtual uint32_t Create() = 0;
protected:// methods
	// CreateDescriptor
	virtual void CreateDescriptor(ID3D12Device8* device, const uint32_t& num, D3D12_DESCRIPTOR_HEAP_TYPE type, const bool& shaderVisible);
	// GetHandle
	virtual D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const uint32_t& index);
	virtual D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const uint32_t& index);
protected:// members
	// 最大ディスクリプタ数
	uint32_t m_MaxDescriptor = {};
	// ヒープタイプ
	D3D12_DESCRIPTOR_HEAP_TYPE m_Type = {};
	// デスクリプタサイズ
	uint32_t m_Size = {};
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap = {};
	// ハンドルコンテナ
	FVector<DescriptorHandle> m_Handles;
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
	// CreateSUV
	uint32_t Create() override;
private:// methods
	
private:// members
};

class RTVDescriptorHeap : public DescriptorHeap
{
public:// methods
	// Constructor
	RTVDescriptorHeap(ID3D12Device8* device, const uint32_t& maxDescriptor);
	// Destructor
	~RTVDescriptorHeap();
	// CreateRTV
	uint32_t Create() override;
private:// members
};

class DSVDescriptorHeap : public DescriptorHeap
{
public:// methods
	// Constructor
	DSVDescriptorHeap(ID3D12Device8* device, const uint32_t& maxDescriptor);
	// Destructor
	~DSVDescriptorHeap();
	// CreateDSV
	uint32_t Create() override;
private:// members
};