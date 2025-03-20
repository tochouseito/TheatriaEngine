#include "pch.h"
#include "ModelManager.h"
#include "Resources/ResourceManager/ResourceManager.h"

void ModelManager::CreateDefaultMesh()
{
	// Cube
	ModelData modelaData;
	modelaData.name = L"Cube";
	MeshData meshData;
	meshData.name = L"Cube";
	
	uint32_t vertices = 24;// 頂点数
	uint32_t indices = 36;// インデックス数
	// メモリ確保
	meshData.vertices.resize(vertices);
	meshData.indices.resize(indices);

	// VertexBuffer作成
	BUFFER_VERTEX_DESC desc = {};
	desc.numElements = static_cast<UINT>(vertices);
	desc.structuredByteStride = static_cast<UINT>(sizeof(VertexData));
	desc.state = D3D12_RESOURCE_STATE_GENERIC_READ;
	desc.numElementsForIBV = static_cast<UINT>(indices);
	desc.structuredByteStrideForIBV = static_cast<UINT>(sizeof(uint32_t));
	meshData.vertexBufferIndex = m_ResourceManager->CreateVertexBuffer(desc);
	

}
