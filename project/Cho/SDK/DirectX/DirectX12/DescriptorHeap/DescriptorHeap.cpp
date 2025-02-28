#include "pch.h"
#include "DescriptorHeap.h"

DescriptorHandle::DescriptorHandle()
{
}

DescriptorHandle::DescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
}

DescriptorHeap::~DescriptorHeap()
{
}

uint32_t DescriptorHeap::GetMaxDescriptor() const
{
	return m_MaxDescriptor;
}

D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeap::GetType() const
{
	return m_Type;
}

uint32_t DescriptorHeap::GetSize() const
{
	return m_Size;
}

ID3D12DescriptorHeap* DescriptorHeap::GetDescriptorHeap() const
{
	return m_DescriptorHeap.Get();
}

SUVDescriptorHeap::SUVDescriptorHeap(ID3D12Device8* device, const uint32_t& maxDescriptor)
{
}

SUVDescriptorHeap::~SUVDescriptorHeap()
{
}

RTVDescriptorHeap::RTVDescriptorHeap(ID3D12Device8* device, const uint32_t& maxDescriptor)
{
}

RTVDescriptorHeap::~RTVDescriptorHeap()
{
}

DSVDescriptorHeap::DSVDescriptorHeap(ID3D12Device8* device, const uint32_t& maxDescriptor)
{
}

DSVDescriptorHeap::~DSVDescriptorHeap()
{
}
