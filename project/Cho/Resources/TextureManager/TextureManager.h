#pragma once
#include <d3d12.h>
#include <DirectXTex.h>
#include <d3dx12.h>
#include "Cho/Core/Utility/FVector.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>
namespace fs = std::filesystem;

struct TextureData
{
	std::wstring name;
	DirectX::TexMetadata metadata;
	uint32_t bufferIndex = UINT32_MAX;
};
class ResourceManager;
class GraphicsEngine;
class CommandContext;
class TextureBuffer;
class TextureManager
{
public:
	TextureManager(ResourceManager* resourceManager, GraphicsEngine* graphicsEngine, ID3D12Device8* device) :
		m_ResourceManager(resourceManager), m_GraphicsEngine(graphicsEngine), m_Device(device)
	{
		LoadEngineTexture();
	}
	~TextureManager()
	{

	}
	// Engineのリソースをロード
	void LoadEngineTexture();
	// Resourceのアップロード
	void UploadTextureDataEx(CommandContext* context, ID3D12Resource* resource, const DirectX::ScratchImage& mipImages);
	// ダミーテクスチャバッファを取得
	TextureBuffer* GetDummyTextureBuffer();
private:
	ResourceManager* m_ResourceManager = nullptr;
	GraphicsEngine* m_GraphicsEngine = nullptr;
	ID3D12Device8* m_Device = nullptr;

	// TextureDataContainer
	FVector<TextureData> m_Textures;
	// 検索用名前コンテナ
	std::unordered_map<std::wstring, uint32_t> m_TextureNameContainer;
};

