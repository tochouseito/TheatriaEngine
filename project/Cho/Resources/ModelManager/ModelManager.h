#pragma once
#include "ChoMath.h"
#include "Core/Utility/FVector.h"
#include "SDK/DirectX/DirectX12/stdafx/stdafx.h"
#include <unordered_map>
#include <list>
// 頂点データ構造体
struct VertexData
{
	Vector4 position = { 0.0f };
	Vector2 texCoord = { 0.0f };
	Vector3 normal = { 0.0f };
};
struct MeshData
{
	std::wstring name;
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	std::optional<uint32_t> vertexBufferIndex = std::nullopt;
	std::optional<uint32_t> indexBufferIndex = std::nullopt;
};
struct ModelData
{
	std::wstring name;
	std::vector<MeshData> meshes;
	// このモデルを使用しているTransformのインデックス
	std::list<uint32_t> useTransformList;
	// このモデルを使用しているTFリストのバッファインデックス
	std::optional<uint32_t> useTransformBufferIndex = std::nullopt;
};

class ResourceManager;
class ModelManager
{
public:
	ModelManager(ResourceManager* resourceManager)
		:m_pResourceManager(resourceManager)
	{
		CreateDefaultMesh();
	}
	~ModelManager()
	{

	}
	// 名前コンテナを取得する
	std::unordered_map<std::wstring, uint32_t>& GetModelNameContainer() { return m_ModelNameContainer; }
	// モデルデータコンテナを取得する
	std::vector<ModelData>& GetModelDataContainer() { return m_Models.GetVector(); }
	// 名前で検索してインデックスを取得する
	std::optional<uint32_t> GetModelDataIndex(const std::wstring& name);
	// モデルデータを取得する
	ModelData* GetModelData(const uint32_t& index) { return &m_Models[index]; }
	// モデルのUseListに登録する
	void RegisterModelUseList(const std::variant<uint32_t,std::wstring>& key, const uint32_t& transformMapID);
private:
	// デフォルトメッシュの生成
	void CreateDefaultMesh();
	// モデルコンテナの要素数を取得する
	uint32_t GetModelDataSize() { return static_cast<uint32_t>(m_Models.size()); }
	// ModelDataの追加
	void AddModelData(ModelData& modelData);

	ResourceManager* m_pResourceManager = nullptr;
	// モデルデータコンテナ
	FVector<ModelData> m_Models;
	// モデルのキーを名前で管理するコンテナ
	std::unordered_map<std::wstring, uint32_t> m_ModelNameContainer;

	// モデルの使用可能なTransformの数のオフセット
	static const uint32_t kUseTransformOffset = 50;
};

