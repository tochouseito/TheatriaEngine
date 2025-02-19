#pragma once

// C++
#include<string>
#include<memory>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

// assimp
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

// モデルデータ構造体
#include"ConstantData/ModelData.h"

class ResourceViewManager;
class TextureLoader;
class MeshLoader;
class ModelLoader
{
public:

	void Initialize(ResourceViewManager* rvManager,TextureLoader* texLoader, MeshLoader* meshLoader);

	void LoadModel(const std::string& directoryPath, const fs::directory_entry& entry);

	/// <summary>
	/// 開始ロード
	/// </summary>
	void FirstResourceLoad(const std::string& directoryPath);

private:

	void LoadModelFile(ModelData* modelData, const std::string& directoryPath, const fs::directory_entry& entry);

	void LoadAnimationFile(ModelData* modelData, const std::string& directoryPath, const fs::directory_entry& entry);

	Node ReadNode(aiNode* node);

	void CreateSkeleton(ModelData* modelData,const Node& rootNode);

	int32_t CreateJoint(
		const Node& node,
		const std::optional<int32_t>& parent,
		std::vector<Joint>& joints
	);

	void CreateInfluence(
		ModelData* modelData,
		const MeshData& meshData
	);

	void CreateSkinCluster(
		ModelData* modelData,
		const MeshData& meshData
	);

private:
	// ポインタ
	ResourceViewManager* rvManager_ = nullptr;
	TextureLoader* texLoader_ = nullptr;
	MeshLoader* meshLoader_ = nullptr;
};