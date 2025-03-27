#pragma once
#include "Cho/Core/Utility/FVector.h"
#include "Cho/Core/Utility/CompBufferData.h"
struct IntegrationData
{
	// データ型
	// マップ用
	uint32_t bufferIndex = UINT32_MAX;// BufferIndex
};
// Transform用統合データ
struct TransformIntegrationData
{
	// サイズ
	uint32_t size = 0;
	// データ型
	BUFFER_DATA_TF* pBufferData = nullptr;
	// BufferIndex
	uint32_t bufferIndex = UINT32_MAX;
	// 使用する頂点ごとに分けるためのアクセス用番号コンテナ
	FVector<FVector<uint32_t>> useVertexToTransformIndex;// [頂点番号][使用するSRV番号]
	uint32_t useBufferIndex = UINT32_MAX;// アクセス用コンテナのBufferIndex
};

class BufferManager;
class IntegrationBuffer
{
public:
	// Constructor
	IntegrationBuffer()
	{
	}
	// Destructor
	~IntegrationBuffer()
	{
	}
	// Transform用統合データのBufferIndexを取得する
	uint32_t GetTransformBufferIndex() const { return m_TransformIntegrationData.bufferIndex; }
	// TransformIndexを取得する
	uint32_t GetTransformIndexBufferIndex() const { return m_TransformIntegrationData.useBufferIndex; }
	// どの頂点データが使用されているかどうかのコンテナを取得する
	std::vector<bool>& GetUseVertexFlag() { return m_UseVertexFlag.GetVector(); }
private:
	// Transform用統合データ
	TransformIntegrationData m_TransformIntegrationData;
	// 頂点データが使用されているかどうかのフラグコンテナ
	FVector<bool> m_UseVertexFlag;
};

