#include "pch.h"
#include "ModelManager.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Cho/GameCore/IntegrationBuffer/IntegrationBuffer.h"

void ModelManager::CreateDefaultMesh()
{
	// Cube
	std::wstring modelName = L"Cube";
	ModelData modelData;
	modelData.name = modelName;
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
	// 頂点データを設定
#pragma region
	// 右面
	meshData.vertices[0] = { {0.5f,  0.5f,  0.5f, 1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }; // 右上
	meshData.vertices[1] = { {0.5f,  0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }; // 左上
	meshData.vertices[2] = { {0.5f, -0.5f,  0.5f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} }; // 右下
	meshData.vertices[3] = { {0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f} }; // 左下

	// 左面
	meshData.vertices[4] = { {-0.5f,  0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} }; // 左上
	meshData.vertices[5] = { {-0.5f,  0.5f,  0.5f, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} }; // 右上
	meshData.vertices[6] = { {-0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} }; // 左下
	meshData.vertices[7] = { {-0.5f, -0.5f,  0.5f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} }; // 右下

	// 前面
	meshData.vertices[8] = { {-0.5f,  0.5f,  0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} }; // 左上
	meshData.vertices[9] = { { 0.5f,  0.5f,  0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} }; // 右上
	meshData.vertices[10] = { {-0.5f, -0.5f,  0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} }; // 左下
	meshData.vertices[11] = { { 0.5f, -0.5f,  0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} }; // 右下

	// 後面
	meshData.vertices[12] = { { 0.5f,  0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }; // 右上
	meshData.vertices[13] = { {-0.5f,  0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }; // 左上
	meshData.vertices[14] = { { 0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} }; // 右下
	meshData.vertices[15] = { {-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} }; // 左下

	// 上面
	meshData.vertices[16] = { {-0.5f,  0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} }; // 左奥
	meshData.vertices[17] = { { 0.5f,  0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} }; // 右奥
	meshData.vertices[18] = { {-0.5f,  0.5f,  0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f} }; // 左前
	meshData.vertices[19] = { { 0.5f,  0.5f,  0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} }; // 右前

	// 下面
	meshData.vertices[20] = { {-0.5f, -0.5f,  0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} }; // 左前
	meshData.vertices[21] = { { 0.5f, -0.5f,  0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f} }; // 右前
	meshData.vertices[22] = { {-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f} }; // 左奥
	meshData.vertices[23] = { { 0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} }; // 右奥

	// 右面インデックス
	meshData.indices[0] = 0; meshData.indices[1] = 2; meshData.indices[2] = 1;
	meshData.indices[3] = 2; meshData.indices[4] = 3; meshData.indices[5] = 1;

	// 左面インデックス
	meshData.indices[6] = 4; meshData.indices[7] = 6; meshData.indices[8] = 5;
	meshData.indices[9] = 6; meshData.indices[10] = 7; meshData.indices[11] = 5;

	// 前面インデックス
	meshData.indices[12] = 8; meshData.indices[13] = 10; meshData.indices[14] = 9;
	meshData.indices[15] = 10; meshData.indices[16] = 11; meshData.indices[17] = 9;

	// 後面インデックス
	meshData.indices[18] = 12; meshData.indices[19] = 14; meshData.indices[20] = 13;
	meshData.indices[21] = 14; meshData.indices[22] = 15; meshData.indices[23] = 13;

	// 上面インデックス
	meshData.indices[24] = 16; meshData.indices[25] = 18; meshData.indices[26] = 17;
	meshData.indices[27] = 18; meshData.indices[28] = 19; meshData.indices[29] = 17;

	// 下面インデックス
	meshData.indices[30] = 20; meshData.indices[31] = 22; meshData.indices[32] = 21;
	meshData.indices[33] = 22; meshData.indices[34] = 23; meshData.indices[35] = 21;
#pragma endregion
	// コピー
	if (desc.mappedVertices != nullptr && desc.mappedIndices != nullptr)
	{
		memcpy(desc.mappedVertices, meshData.vertices.data(), sizeof(VertexData) * vertices);
		memcpy(desc.mappedIndices, meshData.indices.data(), sizeof(uint32_t) * indices);
	} else
	{
		ChoAssertLog("Failed to copy vertices and indices.", false, __FILE__, __LINE__);
	}
	// マップ解除
	m_ResourceManager->GetBufferManager()->GetVertexBuffer(meshData.vertexBufferIndex)->UnMap();
	desc.mappedVertices = nullptr;
	desc.mappedIndices = nullptr;
	// コンテナに追加
	modelData.meshes.push_back(meshData);
	AddModelData(modelData, modelName);
}

// ModelDataの追加
uint32_t ModelManager::AddModelData(ModelData& modelData, const std::wstring& name)
{
	uint32_t index = static_cast<uint32_t>(m_Models.push_back(std::move(modelData)));
	m_ModelNameContainer[name] = index;
	// 統合バッファも合わせる
	m_IntegrationBuffer->AddNewGroup(m_Models.GetVector().size() - 1);
	return index;
}
