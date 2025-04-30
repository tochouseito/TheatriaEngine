#pragma once
#include "Core/Utility/FVector.h"
#include <unordered_map>
#include <string>
#include "Core/Utility/EffectStruct.h"

class EffectContainer
{
public:
	EffectContainer()
	{

	}
	~EffectContainer()
	{

	}
	// エフェクトデータを追加
	/*void AddEffectData(const std::string& name, const EffectData& data)
	{
		uint32_t index = m_EffectData.push_back(data);
		m_EffectNameToIndex[name] = index;
	}*/


	// 編集用エフェクトデータを取得
	EffectData* GetEffectDataTemp()
	{
		return &m_EffectDataTemp;
	}
private:
	FVector<EffectData> m_EffectData; // エフェクトデータ
	std::unordered_map<std::string, uint32_t> m_EffectNameToIndex; // エフェクト名とインデックスのマップ
	EffectData m_EffectDataTemp; // 編集用エフェクトデータ
};

