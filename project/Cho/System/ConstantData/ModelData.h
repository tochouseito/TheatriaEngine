#pragma once

#include"ConstantData/MeshData.h"

// 数学ライブラリ
#include"ChoMath.h"
#include"Color.h"

#include"ConstantData/WorldTransform.h"

// C++
#include<unordered_map>
#include<string>
#include<vector>
#include<map>

#include"ConstantData/AnimationData.h"

struct Node
{
	NodeTransform transform;
	Matrix4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

struct ModelMaterial {
	bool isTexture = false;
	std::string textureName="";
	Color color = { 1.0f,1.0f,1.0f,1.0f };
};

struct ObjectData {
	std::map<std::string, JointWeightData>skinClusterData;
	SkinningInformation* infoData=nullptr;
	uint32_t infoCBVIndex = 0;
	//std::vector<VertexData> vertices;// 頂点リスト
	//std::vector<uint32_t>indices;// Indexリスト
	//uint32_t meshIndex;
	// ModelMaterial
	ModelMaterial material;
};

// モデルデータ構造体
struct ModelData {
	uint32_t meshIndex = 0;
	std::unordered_map<std::string, ObjectData> objects;
	std::vector<std::string> meshNames;
	Node rootNode;
	bool isBone = false;
	std::vector<AnimationData> animations;
	Skeleton skeleton;
	SkinCluster skinCluster;
};