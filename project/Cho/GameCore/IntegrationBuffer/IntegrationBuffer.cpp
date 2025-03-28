#include "pch.h"
#include "IntegrationBuffer.h"

void IntegrationBuffer::AddNewGroup(const size_t& size)
{
	// モデルの数とフラグの数が一致していない場合
	if (m_TransformIntegrationData.useVertexToTransformIndex.GetVector().size() != size &&
		m_UseVertexFlag.GetVector().size() != size)
	{
		ChoAssertLog("Size is not match", false, __FILE__, __LINE__);
	}
	// 新しいグループ用のフラグと要素を追加
	m_UseVertexFlag.push_back(false);
	m_TransformIntegrationData.useVertexToTransformIndex.push_back(FVector<uint32_t>());
}
