#pragma once

// C++
#include<numbers>
#include<vector>
#include<unordered_map>
#include <iostream>

#include"MeshPattern/MeshPattern.h"

// MeshData
#include"ConstantData/MeshData.h"
#include"ConstantData/SpriteData.h"
#include"ConstantData/MeshletData.h"
#include"ConstantData/LineData.h"

struct Meshes {
	std::vector<MeshData> meshData;
	std::string meshesName;
};

class ResourceViewManager;
class MeshGenerator
{
public:
	static Meshes GeneratorMeshes(MeshPattern pattern, ResourceViewManager* rvManager);

	// スプライト生成
	static void CreateSprite(SpriteMeshData& data, ResourceViewManager* rvManager);

	// ライン生成
	static void CreateLineMesh(ResourceViewManager* rvManager);

	// メッシュレット生成
    static void GenerateMeshlets(MeshData& meshData, ResourceViewManager* rvManager ,uint32_t maxVertices = 64, uint32_t maxTriangles = 124,float coneWeight = 1.0f);

    static void OptimizeMesh(MeshData& meshData,ResourceViewManager* rvManager);

	static void ProcessMeshForMeshShader(MeshData& meshData,ResourceViewManager* rvManager);

private:
	// 球体生成
	static VertexSize CreateSphere(MeshData& data, ResourceViewManager* rvManager);

	// 立方体生成
	static VertexSize CreateCube(MeshData& data, ResourceViewManager* rvManager);

	// 平面生成
	static VertexSize CreatePlane(MeshData& data, ResourceViewManager* rvManager);
};

