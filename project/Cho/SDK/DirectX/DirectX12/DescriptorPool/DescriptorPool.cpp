#include "pch.h"
#include "DescriptorPool.h"

DescriptorPool::~DescriptorPool()
{
}

uint32_t DescriptorPool::GetMaxDescriptor() const
{
	return m_MaxDescriptor;
}

D3D12_DESCRIPTOR_HEAP_TYPE DescriptorPool::GetType() const
{
	return m_Type;
}

uint32_t DescriptorPool::GetSize() const
{
	return m_Size;
}

ID3D12DescriptorHeap* DescriptorPool::GetDescriptorHeap() const
{
	return m_DescriptorHeap.Get();
}

SUVDescriptorPool::SUVDescriptorPool(ID3D12Device8* device, const uint32_t& maxDescriptor)
{
}

SUVDescriptorPool::~SUVDescriptorPool()
{
}

RTVDescriptorPool::RTVDescriptorPool(ID3D12Device8* device, const uint32_t& maxDescriptor)
{
}

RTVDescriptorPool::~RTVDescriptorPool()
{
}

DSVDescriptorPool::DSVDescriptorPool(ID3D12Device8* device, const uint32_t& maxDescriptor)
{
}

DSVDescriptorPool::~DSVDescriptorPool()
{
}
