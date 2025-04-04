#pragma once
#include "Cho/Core/Utility/FVector.h"
#include "Cho/Core/Utility/CompBufferData.h"
#include <list>

// 統合バッファのインタフェース
class IIntegrationData
{
public:
	// Constructor
	IIntegrationData()
	{

	}
	// Destructor
	virtual ~IIntegrationData() = default;
	// サイズを取得
	virtual size_t size() const = 0;
	// バッファのインデックスを取得
	virtual std::optional<uint32_t> GetBufferIndex() const = 0;
	// MapIDを取得
	virtual uint32_t GetMapID() const = 0;
};

class ResourceManager;
template<typename T>
class IntegrationData : public IIntegrationData
{
public:
	// Constructor
	IntegrationBuffer(std::optional<uint32_t>& index)
		: m_StructuredBufferIndex(index)
	{
		m_Size = 100;// 初期サイズ
	}
	// Destructor
	~IntegrationBuffer()
	{
	}
	// サイズを取得
	size_t size() const
	{
		return m_Size;
	}
	// バッファのインデックスを取得
	std::optional<uint32_t> GetBufferIndex() const
	{
		return m_StructuredBufferIndex;
	}
	// MapIDを取得
	uint32_t GetMapID() const
	{
		// 返却されたIDがあるなら取得
		if (!m_RemoveMapID.empty())
		{
			uint32_t id = m_RemoveMapID.back();
			m_RemoveMapID.pop_back();
			return id;
		}
		// IDを取得
		uint32_t id = m_NextMapID;
		// 一つ進める
		m_NextMapID++;
		return id;
	}
private:
	// リソースマネージャーのポインタ
	ResourceManager* m_ResourceManager = nullptr;
	// 構造化バッファのインデックス
	std::optional<uint32_t> m_StructuredBufferIndex = std::nullopt;
	// 統合データのサイズ
	size_t m_Size = 0;
	// 統合データの次のID
	uint32_t m_NextMapID = 0;
	// 統合データの返却されたID
	std::vector<uint32_t> m_RemoveMapID = {};

	// 統合データのサイズオフセット
	static const uint32_t kSizeOffset = 50;
};