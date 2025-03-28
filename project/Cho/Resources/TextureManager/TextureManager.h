#pragma once

#include <d3d12.h>
#include <DirectXTex.h>
#include <d3dx12.h>
#include <string>
#include "Cho/Core/Utility/FVector.h"
#include <memory>
#include <unordered_map>
#include <filesystem>
namespace fs = std::filesystem;

struct TextureData
{
	std::string name;
	DirectX::TexMetadata metadata;
	uint32_t bufferIndex = UINT32_MAX;
};
class ResourceManager;
class GraphicsEngine;
class TextureManager
{
public:
	TextureManager(ResourceManager* resourceManager,GraphicsEngine* graphicsEngine):
		m_ResourceManager(resourceManager), m_GraphicsEngine(graphicsEngine)
	{
	}
	~TextureManager()
	{

	}
	// Engineのリソースをロード
	void LoadEngineTexture();
private:
	ResourceManager* m_ResourceManager = nullptr;
	GraphicsEngine* m_GraphicsEngine = nullptr;

	// TextureDataContainer
	FVector<TextureData> m_Textures;
	// 検索用名前コンテナ
	std::unordered_map<std::string, uint32_t> m_TextureNameContainer;
};

