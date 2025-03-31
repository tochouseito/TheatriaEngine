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
	// マッピング
	BUFFER_DATA_TF* mappedData = nullptr;
};

class StructuredBuffer;
class ResourceManager;
class IntegrationBuffer
{
	friend class ModelManager;
public:
	// Constructor
	IntegrationBuffer(ResourceManager* resourceManager) :
		m_ResourceManager(resourceManager)
	{
		CreateDefaultTFResource();
	}
	// Destructor
	~IntegrationBuffer()
	{
	}
	// TF統合バッファの先頭IDから取得
	uint32_t GetNextMapID()
	{
		uint32_t id;
		// IDを取得
		// 返却されたIDがあるなら取得
		if (!m_TransformIntegrationData.returnMapID.empty())
		{
			id = m_TransformIntegrationData.returnMapID.front();
			m_TransformIntegrationData.returnMapID.pop_back();
			return id;
		}
		id = m_TransformIntegrationData.nextMapID;
		// 一つ進める
		m_TransformIntegrationData.nextMapID++;
		return id;
	}
	void AddUseIndex(const uint32_t& modelIndex, const uint32_t& mapID)
	{
		// 既にリストに存在していたら追加しない
		for (auto& id : m_TransformIntegrationData.useInstance[modelIndex].useVertexToTransformIndex)
		{
			if (id == mapID) { return; }
		}
		m_TransformIntegrationData.useInstance[modelIndex].useVertexToTransformIndex.push_back(mapID);
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
	bool IsUsedModels(const uint32_t& modelIndex)
	{
		if (m_TransformIntegrationData.useInstance.GetVector().empty())
		{
			return false;
		}
		if (m_TransformIntegrationData.useInstance[modelIndex].useVertexToTransformIndex.empty())
		{
			return false;
		}
		return true;
	}
	void TransferTFData(const uint32_t& mapID, const BUFFER_DATA_TF& data)
	{
		m_TransformIntegrationData.mappedData[mapID].matWorld = data.matWorld;
		m_TransformIntegrationData.mappedData[mapID].worldInverse = data.worldInverse;
		m_TransformIntegrationData.mappedData[mapID].rootMatrix = data.rootMatrix;
	}
	void TransferVPData(const uint32_t& mapID, const BUFFER_DATA_VIEWPROJECTION& data)
	{
		m_MappedViewProjection[mapID]->view = data.view;
		m_MappedViewProjection[mapID]->projection = data.projection;
		m_MappedViewProjection[mapID]->projectionInverse = data.projectionInverse;
		m_MappedViewProjection[mapID]->matWorld = data.matWorld;
		m_MappedViewProjection[mapID]->matBillboard = data.matBillboard;
		m_MappedViewProjection[mapID]->cameraPosition = data.cameraPosition;
	}
	// 仮置き
	uint32_t GetMappedVPID(const uint32_t& bufferIndex);
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

	// 仮置き
	FVector<BUFFER_DATA_VIEWPROJECTION*> m_MappedViewProjection;
};

