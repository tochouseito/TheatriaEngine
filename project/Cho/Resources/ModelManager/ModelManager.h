#pragma once
#include "ChoMath.h"
#include "Cho/Core/Utility/FVector.h"
#include "Cho/SDK/DirectX/DirectX12/stdafx/stdafx.h"
#include <unordered_map>
// 頂点データ構造体
struct VertexData
{
	Vector4 position = { 0.0f };
	Vector2 texCoord = { 0.0f };
	Vector3 normal = { 0.0f };
};
struct MeshData
{
	std::string name;
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	uint32_t vertexBufferIndex = UINT32_MAX;
};
struct ModelData
{
	std::string name;
	std::vector<MeshData> meshes;
};

class ResourceManager;
class IntegrationBuffer;
class ModelManager
{
public:
	ModelManager(ResourceManager* resourceManager,IntegrationBuffer* integrationBuffer)
		:m_ResourceManager(resourceManager), m_IntegrationBuffer(integrationBuffer)
	{
		CreateDefaultMesh();
	}
	~ModelManager()
	{

	}
	// モデルデータを取得する
	ModelData* GetModelData(const uint32_t& index) { return &m_Models[index]; }
private:
	// デフォルトメッシュの生成
	void CreateDefaultMesh();
	// ModelDataの追加
	uint32_t AddModelData(ModelData& modelData,const std::string& name);

	ResourceManager* m_ResourceManager = nullptr;
	IntegrationBuffer* m_IntegrationBuffer = nullptr;
	// モデルデータコンテナ
	FVector<ModelData> m_Models;
	// モデルのキーを名前で管理するコンテナ
	std::unordered_map<std::string, uint32_t> m_ModelNameContainer;
};

