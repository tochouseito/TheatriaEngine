#pragma once
#include "Cho/Core/Utility/FVector.h"
#include "Cho/Core/Utility/CompBufferData.h"
#include <list>
// モデルごとに統合バッファにアクセスしているIndexを格納するData
struct UseInstance
{
	uint32_t bufferIndex = UINT32_MAX;
	uint32_t* mappedData = nullptr;
	uint32_t size = 0;
	// このモデルを使用しているTFリソースのIndexリスト
	std::list<uint32_t> useVertexToTransformIndex;
};
// Transform用統合データ
struct TransformIntegrationData
{
	// サイズ
	uint32_t size = 0;
	// BufferIndex
	uint32_t bufferIndex = UINT32_MAX;
	// 新たにマッピングデータにアクセスするID
	uint32_t nextMapID = 0;
	// 返却されたID
	std::vector<uint32_t> returnMapID;
	// 使用するInstanceのBufferIndex
	FVector<UseInstance> useInstance;
};

class StructuredBuffer;
class ResourceManager;
class IntegrationBuffer
{
	friend class ModelManager;
public:
	// Constructor
	IntegrationBuffer(ResourceManager* resourceManager):
		m_ResourceManager(resourceManager)
	{
		CreateDefaultTFResource();
	}
	// Destructor
	~IntegrationBuffer()
	{
	}
	// Transform統合バッファの取得
	StructuredBuffer* GetTFBuffer() const;
	// 使用する頂点ごとに分けるためのアクセス用番号コンテナのBufferIndexを取得する
	StructuredBuffer* GetUseInstanceBuffer(const uint32_t& modelIndex) const;
	// モデルを使用しているTFインスタンスのリストを取得
	std::list<uint32_t>& GetUseVertexToList(const uint32_t& modelIndex)
	{
		return m_TransformIntegrationData.useInstance[modelIndex].useVertexToTransformIndex;
	}

private:
	// 初期リソースの生成
	void CreateDefaultTFResource();
	// 新たなモデルが追加されたときに新しいグループようのフラグと要素を追加する
	void AddNewGroup(const size_t& size);

	ResourceManager* m_ResourceManager = nullptr;
	// Transform用統合データ
	TransformIntegrationData m_TransformIntegrationData;

	// デフォルトのTransformデータ数
	static const UINT kDefaultTFSize = 100;
	// Modelごとの描画可能な数（自動で増える）
	static const UINT kDefaultGroupSize = 10;
};

