#pragma once
#include "ChoMath.h"
#include "Cho/Core/Utility/FVector.h"
#include "Cho/SDK/DirectX/DirectX12/stdafx/stdafx.h"
// 頂点データ構造体
struct VertexData
{
	Vector4 position = { 0.0f };
	Vector2 texcoord = { 0.0f };
	Vector3 normal = { 0.0f };
};
struct MeshData
{
	std::wstring name;
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
};
struct ModelData
{
	std::wstring name;
	std::vector<MeshData> meshes;
};

class BufferManager;
class ModelManager
{
public:
	ModelManager(BufferManager* bufferManager)
		:m_BufferManager(bufferManager)
	{
		CreateDefaultMesh();
	}
	~ModelManager()
	{

	}
	
private:
	// デフォルトメッシュの生成
	void CreateDefaultMesh();

	BufferManager* m_BufferManager = nullptr;
	// モデルデータコンテナ
	FVector<ModelData> m_Models;
};

