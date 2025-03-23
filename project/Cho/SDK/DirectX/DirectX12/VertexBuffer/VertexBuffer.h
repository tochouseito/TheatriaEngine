#pragma once
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
struct BUFFER_VERTEX_DESC
{
	UINT numElements;
	UINT structuredByteStride;
	D3D12_RESOURCE_STATES state;
	uint32_t suvDHIndex;
	uint32_t suvDHIndexForIBV;
	UINT numElementsForIBV;
	UINT structuredByteStrideForIBV;
	// マップ用データにコピーするためのポインタ
	void* mappedVertices = nullptr;
	void* mappedIndices = nullptr;
};
class VertexBuffer : public GpuBuffer
{
public:
	// Constructor
	VertexBuffer(BUFFER_VERTEX_DESC desc) : GpuBuffer()
	{
		SetSUVHandleIndex(desc.suvDHIndex);
		SetResourceState(desc.state);
		m_IndexBuffer.SetSUVHandleIndex(desc.suvDHIndexForIBV);
	}
	// Constructor
	VertexBuffer(ID3D12Resource* pResource, BUFFER_VERTEX_DESC desc) :
		GpuBuffer(pResource, desc.state)
	{
	}
	// Destructor
	~VertexBuffer()
	{
	}
	// DestroyIndexBuffer
	void DestroyIndexBuffer() { m_IndexBuffer.Destroy(); }
	ID3D12Resource* GetIndexResource() { return m_IndexBuffer.GetResource(); }
	void UnMap();
	uint32_t GetIndexBufferDHandleIndex() const { return m_IndexBuffer.GetSUVHandleIndex(); }
	void CreateVertexResource(ID3D12Device8* device, const UINT& numElements, const UINT& structuredByteStride);
	void CreateIndexResource(ID3D12Device8* device, const UINT& numElements, const UINT& structuredByteStride);
private:
	GpuBuffer m_IndexBuffer;// IndexBufferResource
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView = {};
};

