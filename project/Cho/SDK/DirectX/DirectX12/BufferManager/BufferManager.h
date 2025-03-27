#pragma once

#include "Cho/SDK/DirectX/DirectX12/stdafx/stdafx.h"
#include "Cho/SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "Cho/SDK/DirectX/DirectX12/ColorBuffer/ColorBuffer.h"
#include "Cho/SDK/DirectX/DirectX12/DepthBuffer/DepthBuffer.h"
#include "Cho/SDK/DirectX/DirectX12/VertexBuffer/VertexBuffer.h"

// 有効なBufferの型
template<typename T>
using BufferType = std::enable_if_t<
	std::is_same_v<T, ColorBuffer> ||
	std::is_same_v<T, DepthBuffer> ||
	std::is_same_v<T, VertexBuffer>||
	std::is_same_v<T, ConstantBuffer>||
	std::is_same_v<T, StructuredBuffer>
>;
template<typename T>
using BufferDescType = std::enable_if_t<
	std::is_same_v<T, BUFFER_COLOR_DESC> ||
	std::is_same_v<T, BUFFER_DEPTH_DESC> ||
	std::is_same_v<T, BUFFER_VERTEX_DESC>||
	std::is_same_v<T, BUFFER_CONSTANT_DESC>||
	std::is_same_v<T, BUFFER_STRUCTURED_DESC>
>;

class ResourceManager;
class BufferManager
{
public:// メンバ関数

	// Constructor
	BufferManager(ID3D12Device8* device,ResourceManager* resourceManager);
	// Destructor
	~BufferManager();
	uint32_t CreateForSwapChain(const BUFFER_COLOR_DESC& desc, ID3D12Resource* pResource) { return CreateBufferForSwapChain(desc,pResource); }
	// Create Buffer
	// return: BufferNumber
	template<typename T, typename = BufferDescType<T>>
	uint32_t CreateBuffer(T& desc) { return CreateBufferProcess(desc); }
	// RemakeBuffer
	template<typename T, typename = BufferDescType<T>>
	void RemakeBuffer(const uint32_t& index, const T& desc) { RemakeBufferProcess(index, desc); }
	// ReleaseBuffer
	void ReleaseColorBuffer(const uint32_t& index);
	void ReleaseDepthBuffer(const uint32_t& index);
	void ReleaseVertexBuffer(const uint32_t& index);

	// Get Buffer
	ColorBuffer* GetColorBuffer(const uint32_t& index) { return &m_ColorBuffers[index]; }
	DepthBuffer* GetDepthBuffer(const uint32_t& index) { return &m_DepthBuffers[index]; }
	VertexBuffer* GetVertexBuffer(const uint32_t& index) { return &m_VertexBuffers[index]; }
	ConstantBuffer* GetConstantBuffer(const uint32_t& index) { return &m_ConstantBuffers[index]; }
	StructuredBuffer* GetStructuredBuffer(const uint32_t& index) { return &m_StructuredBuffers[index]; }
	//template<typename T, typename = BufferType<T>>
	//T* GetBuffer(const uint32_t& index) const { return GetBufferProcess(index); }
private:
	uint32_t CreateBufferForSwapChain(const BUFFER_COLOR_DESC& desc, ID3D12Resource* pResource);
	// ColorBufferProcess
	uint32_t CreateBufferProcess(BUFFER_COLOR_DESC& desc);
	// DepthBufferProcess
	uint32_t CreateBufferProcess(BUFFER_DEPTH_DESC& desc);
	// VertexBufferProcess
	uint32_t CreateBufferProcess(BUFFER_VERTEX_DESC& desc);
	// ConstantBufferProcess
	uint32_t CreateBufferProcess(BUFFER_CONSTANT_DESC& desc);
	// StructuredBufferProcess
	uint32_t CreateBufferProcess(BUFFER_STRUCTURED_DESC& desc);

	// RemakeBufferProcess
	void RemakeBufferProcess(const uint32_t& index, const BUFFER_COLOR_DESC& desc);
	void RemakeBufferProcess(const uint32_t& index, const BUFFER_DEPTH_DESC& desc);
	void RemakeBufferProcess(const uint32_t& index, const BUFFER_VERTEX_DESC& desc);

	ID3D12Device8* m_Device = nullptr;
	ResourceManager* m_ResourceManager = nullptr;
	// BufferContainer
	FVector<ColorBuffer> m_ColorBuffers;			// カラーバッファ
	FVector<DepthBuffer> m_DepthBuffers;			// 深度バッファ
	FVector<VertexBuffer> m_VertexBuffers;			// 頂点バッファ,インデックスバッファ
	FVector<ConstantBuffer> m_ConstantBuffers;		// 定数バッファ
	FVector<StructuredBuffer> m_StructuredBuffers;	// 構造化バッファ
};

