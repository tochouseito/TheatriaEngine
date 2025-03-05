#include "pch.h"
#include "DescriptorHeap.h"

DescriptorHandle::DescriptorHandle()
{
}

DescriptorHandle::DescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
	m_CpuHandle = cpuHandle;
	m_GpuHandle = gpuHandle;
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

void DescriptorHeap::CreateDescriptor(ID3D12Device8* device, const uint32_t& num, D3D12_DESCRIPTOR_HEAP_TYPE type, const bool& shaderVisible)
{
	m_MaxDescriptor = num;
	m_Type = type;
	m_Size = device->GetDescriptorHandleIncrementSize(m_Type);

	// Create Descriptor Heap
	HRESULT hr = {};
	m_DescriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = m_Type;
	desc.NumDescriptors = m_MaxDescriptor;
	desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_DescriptorHeap));
	ChoAssertLog("CreateDescriptorHeap Failed.", hr, __FILE__, __LINE__);
}

// GetHandle
D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUDescriptorHandle(const uint32_t& index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (m_Size * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUDescriptorHandle(const uint32_t& index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (m_Size* index);
	return handleGPU;
}

SUVDescriptorHeap::SUVDescriptorHeap(ID3D12Device8* device, const uint32_t& maxDescriptor)
{
	CreateDescriptor(device, maxDescriptor, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);
}

SUVDescriptorHeap::~SUVDescriptorHeap()
{
}

uint32_t SUVDescriptorHeap::Create()
{
	return 0;
}

RTVDescriptorHeap::RTVDescriptorHeap(ID3D12Device8* device, const uint32_t& maxDescriptor)
{
	CreateDescriptor(device, maxDescriptor, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false);
}

RTVDescriptorHeap::~RTVDescriptorHeap()
{
}

uint32_t RTVDescriptorHeap::Create()
{
	uint32_t index = static_cast<uint32_t>(m_Handles.push_back(DescriptorHandle()));
	m_Handles[index].SetCPUHandle(GetCPUDescriptorHandle(index));
	//m_Handles[index].SetGPUHandle(GetGPUDescriptorHandle(index));
	return index;
}

DSVDescriptorHeap::DSVDescriptorHeap(ID3D12Device8* device, const uint32_t& maxDescriptor)
{
	CreateDescriptor(device, maxDescriptor, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false);
}

DSVDescriptorHeap::~DSVDescriptorHeap()
{
}

uint32_t DSVDescriptorHeap::Create()
{
	return 0;
}
