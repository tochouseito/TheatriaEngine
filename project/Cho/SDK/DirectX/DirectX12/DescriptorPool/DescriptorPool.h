#pragma once

#include "SDK/DirectX/DirectX12/stdafx/stdafx.h"

class DescriptorPool
{
public:// methods
	// Destructor
	virtual ~DescriptorPool();
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

class SUVDescriptorPool : public DescriptorPool
{
public:// methods
	// Constructor
	SUVDescriptorPool(ID3D12Device8* device,const uint32_t& maxDescriptor);
	// Destructor
	~SUVDescriptorPool();
private:// members
};

class RTVDescriptorPool : public DescriptorPool
{
public:// methods
	// Constructor
	RTVDescriptorPool(ID3D12Device8* device, const uint32_t& maxDescriptor);
	// Destructor
	~RTVDescriptorPool();
private:// members
};

class DSVDescriptorPool : public DescriptorPool
{
public:// methods
	// Constructor
	DSVDescriptorPool(ID3D12Device8* device, const uint32_t& maxDescriptor);
	// Destructor
	~DSVDescriptorPool();
private:// members
};