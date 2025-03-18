#include "pch.h"
#include "ModelManager.h"

void ModelManager::CreateDefaultMesh()
{
	// Cube
	ModelData modelaData;
	modelaData.name = L"Cube";
	MeshData meshData;
	meshData.name = L"Cube";
	
	uint32_t vertices = 24;// 頂点数
	uint32_t indices = 36;// インデックス数
	meshData.vertices.resize(vertices);
	meshData.indices.resize(indices);

	// VertexBuffer作成

}
