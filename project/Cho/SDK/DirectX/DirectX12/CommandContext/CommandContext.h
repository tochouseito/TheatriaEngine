#pragma once

#include "Cho/SDK/DirectX/DirectX12/stdafx/stdafx.h"
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
