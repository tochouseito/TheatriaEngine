#pragma once
#include "ChoMath.h"
#include "Core/Utility/FVector.h"
#include <unordered_map>
#include <string>
#include <list>

struct UIVertexData
{
	Vector4 position;
	Vector2 texCoord;
	Vector4 color;
};

struct UIData
{
	std::vector<UIVertexData> vertices;
	std::vector<uint32_t> indices;
	std::optional<uint32_t> vertexBufferIndex = std::nullopt;
	std::optional<uint32_t> indexBufferIndex = std::nullopt;
};

class ResourceManager;
class UIContainer
{
public:
	UIContainer(ResourceManager* resourceManager)
		:m_pResourceManager(resourceManager)
	{
	}
	~UIContainer()
	{

	}
	// UIDataの追加
	uint32_t AddUIData();
	// UIDataの削除
	void RemoveUIData(uint32_t index)
	{
		m_UIDataContainer.erase(index);
		m_UseList.remove(index);
	}
	// UIDataの取得
	UIData& GetUIData(uint32_t index)
	{
		return m_UIDataContainer[index];
	}
	// UseListの取得
	std::list<uint32_t>& GetUseList()
	{
		return m_UseList;
	}
	// UseListBufferIndexの取得
	uint32_t GetUseListBufferIndex()
	{
		return m_UseListBufferIndex;
	}
private:

	// UIDataのコンテナ
	FVector<UIData> m_UIDataContainer;
	// UseList
	std::list<uint32_t> m_UseList;
	// UseListBufferIndex
	uint32_t m_UseListBufferIndex = 0;

	// ResourceManagerのポインタ
	ResourceManager* m_pResourceManager = nullptr;
};

