#include "pch.h"
#include "IntegrationBuffer.h"
#include "Resources/ResourceManager/ResourceManager.h"


// Transform統合バッファの取得
StructuredBuffer* IntegrationBuffer::GetTFBuffer() const
{
	return m_ResourceManager->GetBufferManager()->GetStructuredBuffer(m_TransformIntegrationData.bufferIndex);
}

// 使用する頂点ごとに分けるためのアクセス用番号コンテナのBufferIndexを取得する
StructuredBuffer* IntegrationBuffer::GetUseInstanceBuffer(const uint32_t& modelIndex) const
{
	return m_ResourceManager->GetBufferManager()->GetStructuredBuffer(m_TransformIntegrationData.useInstance[modelIndex].bufferIndex);
}

uint32_t IntegrationBuffer::GetMappedVPID(const uint32_t& bufferIndex)
{
	uint32_t id = static_cast<uint32_t>(m_MappedViewProjection.push_back(nullptr));
	// マッピングデータの取得
	m_ResourceManager->GetBufferManager()->GetConstantBuffer(bufferIndex)->Map(
		reinterpret_cast<void**>(&m_MappedViewProjection[id])
	);
	return id;
}

void IntegrationBuffer::CreateDefaultTFResource()
{
	// デフォルトのTransformIntegrationDataを作成
	BUFFER_STRUCTURED_DESC desc = {};
	desc.numElements = kDefaultTFSize;
	desc.structuredByteStride = sizeof(BUFFER_DATA_TF);
	desc.state = D3D12_RESOURCE_STATE_GENERIC_READ;
	m_TransformIntegrationData.size = kDefaultTFSize;
	m_TransformIntegrationData.bufferIndex = m_ResourceManager->CreateStructuredBuffer(desc);
	// マップ
	m_ResourceManager->GetBufferManager()->GetStructuredBuffer(
		m_TransformIntegrationData.bufferIndex)->Map(
			reinterpret_cast<void**>(&m_TransformIntegrationData.mappedData)
		);
}

void IntegrationBuffer::AddNewGroup(const size_t& size)
{
	// モデルの数とフラグの数が一致していない場合
	if (m_TransformIntegrationData.useInstance.GetVector().size() != size)
	{
		ChoAssertLog("Size is not match", false, __FILE__, __LINE__);
	}
	// 新しいグループ用のフラグと要素を追加
	size_t index = m_TransformIntegrationData.useInstance.push_back(UseInstance());
	// UseIndexResourceの作成
	BUFFER_STRUCTURED_DESC desc = {};
	desc.numElements = kDefaultGroupSize;
	desc.structuredByteStride = sizeof(uint32_t);
	desc.state = D3D12_RESOURCE_STATE_GENERIC_READ;
	m_TransformIntegrationData.useInstance[index].bufferIndex = m_ResourceManager->CreateStructuredBuffer(desc);
	m_TransformIntegrationData.useInstance[index].size = kDefaultGroupSize;
	// マップ
	m_ResourceManager->GetBufferManager()->GetStructuredBuffer(
		m_TransformIntegrationData.useInstance[index].bufferIndex)->Map(
			reinterpret_cast<void**>(m_TransformIntegrationData.useInstance[index].mappedData)
		);
}
