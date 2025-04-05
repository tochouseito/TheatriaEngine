#include "pch.h"
#include "ModelManager.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "SDK/DirectX/DirectX12/VertexBuffer/VertexBuffer.h"
#include "SDK/DirectX/DirectX12/IndexBuffer/IndexBuffer.h"
#include "Core/ChoLog/ChoLog.h"
using namespace Cho;

// 名前で検索してインデックスを取得する
std::optional<uint32_t> ModelManager::GetModelDataIndex(const std::wstring& name, std::optional<uint32_t>& transformIndex)
{
	if (m_ModelNameContainer.contains(name))
	{
		// 既にUseListに登録されている場合は、インデックスを取得
		if (m_Models[m_ModelNameContainer[name]].useTransformIndex.size() > 0)
		{
			for (auto& index : m_Models[m_ModelNameContainer[name]].useTransformIndex)
			{
				if (index == transformIndex.value())
				{
					return m_ModelNameContainer[name];
				}
			}
			// ほかのモデルに登録されていたら、削除して追加
			for (auto& model : m_Models.GetVector())
			{
				if (model.useTransformIndex.size() > 0)
				{
					for (auto& index : model.useTransformIndex)
					{
						if (index == transformIndex.value())
						{
							model.useTransformIndex.remove(index);
							break;
						}
					}
				}
			}
		}
		// モデルの使用Transformインデックスにない場合は、追加
		m_Models[m_ModelNameContainer[name]].useTransformIndex.push_back(transformIndex.value());
		// バッファも更新
		StructuredBuffer<uint32_t>* buffer = dynamic_cast<StructuredBuffer<uint32_t>*>(m_pResourceManager->GetBuffer<IStructuredBuffer>(m_Models[m_ModelNameContainer[name]].useTransformBufferIndex));
		if (buffer)
		{
			uint32_t i = 0;
			for (uint32_t& useIndex : m_Models[m_ModelNameContainer[name]].useTransformIndex)
			{
				buffer->UpdateData(useIndex, i);
				i++;
			}
		} else
		{
			Log::Write(LogLevel::Assert, "Buffer is nullptr");
		}
		return m_ModelNameContainer[name];
	}
	std::string modelName = ConvertString(name);
	std::string msg = "Model name not found: " + modelName;
	Log::Write(LogLevel::Assert, msg);
	return std::nullopt;
}

void ModelManager::CreateDefaultMesh()
{
	// Cube
	std::wstring modelName = L"Cube";
	ModelData modelData;
	modelData.name = modelName;
	MeshData meshData;
	meshData.name = L"Cube";
	// 頂点数とインデックス数
	uint32_t vertices = 24;// 頂点数
	uint32_t indices = 36;// インデックス数
	// メモリ確保
	meshData.vertices.resize(vertices);
	meshData.indices.resize(indices);
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
	// コンテナに追加
	modelData.meshes.push_back(meshData);
	// modelDataを追加
	AddModelData(modelData);
}

// ModelDataの追加
void ModelManager::AddModelData(ModelData& modelData)
{
	for (MeshData& mesh : modelData.meshes)
	{
		// VertexBuffer,IndexBuffer作成
		mesh.vertexBufferIndex = m_pResourceManager->CreateVertexBuffer<VertexData>(static_cast<UINT>(mesh.vertices.size()));
		mesh.indexBufferIndex = m_pResourceManager->CreateIndexBuffer<uint32_t>(static_cast<UINT>(mesh.indices.size()));
		VertexBuffer<VertexData>* vertexBuffer = dynamic_cast<VertexBuffer<VertexData>*>(m_pResourceManager->GetBuffer<IVertexBuffer>(mesh.vertexBufferIndex));
		IndexBuffer<uint32_t>* indexBuffer = dynamic_cast<IndexBuffer<uint32_t>*>(m_pResourceManager->GetBuffer<IIndexBuffer>(mesh.indexBufferIndex));
		// VBV,IBV作成
		vertexBuffer->CreateVBV();
		indexBuffer->CreateIBV();
		// コピー
		vertexBuffer->MappedDataCopy(mesh.vertices);
		indexBuffer->MappedDataCopy(mesh.indices);
	}
	// UseTransformのリソースを作成
	modelData.useTransformBufferIndex = m_pResourceManager->CreateStructuredBuffer<uint32_t>(kUseTransformOffset);
	// 名前が重複していたら、エラー
	// ここに処理を追加する

	// モデルをコンテナに追加
	std::wstring name = modelData.name;
	uint32_t index = static_cast<uint32_t>(m_Models.push_back(std::move(modelData)));
	// 名前コンテナに登録
	m_ModelNameContainer[name] = index;
}
