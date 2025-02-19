#pragma once

#include<d3d12.h>
#include<DirectXTex.h>
#include<d3dx12.h>
#include <string>
#include <unordered_map>

#include <filesystem>
namespace fs = std::filesystem;

// Thread
#include <mutex>

/// <summary>
/// テクスチャ
/// </summary>
struct Texture {
	DirectX::TexMetadata metadata;
	uint32_t rvIndex;
};

class D3DDevice;
class D3DCommand;
class ResourceViewManager;
class TextureLoader
{
public:

	void Initialize(
		D3DDevice* d3dDevice,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager
	);

	void Load(const std::string& filePath);

	/// <summary>
	/// 開始ロード
	/// </summary>
	void FirstResourceLoad(const std::string& directoryPath);

	// テクスチャの取得
	Texture GetTexture(const std::string& TextureName) {
		if (textureData.contains(TextureName)) {
			return textureData[TextureName];
		} else {
			return GetDummy();
		}
	}

	// テクスチャコンテナの取得
	std::unordered_map<std::string, Texture>& GetTextureData() {
		return textureData;
	}

	Texture GetDummy() {
		const std::string& dummyTex = "Dummy202411181622.png";
		return textureData[dummyTex];
	}

	Texture GetWhitePixel() {
		const std::string& whitePixel = "White1x1.png";
		return textureData[whitePixel];
	}

	Vector2 GetTextureSize(const std::string& texID) {
		const DirectX::TexMetadata& metaData = textureData[texID].metadata;
		return Vector2(static_cast<float>(metaData.width), static_cast<float>(metaData.height));
	}

private:



private:
	D3DDevice* d3dDevice_ = nullptr;
	D3DCommand* d3dCommand_ = nullptr;
	ResourceViewManager* rvManager_ = nullptr;

	// テクスチャコンテナ
	std::unordered_map<std::string, Texture> textureData;

	// 排他処理用
	std::mutex textureMutex;

	// ファイルパス
	const std::string& engineFilePath = "Cho\\Resources\\Texture\\";
	const std::string& gameFilePath = "Game\\Assets\\Texture\\";
};

