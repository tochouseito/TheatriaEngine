#pragma once

// C++
#include<cstdint>
#include<string>
#include<vector>

// Utility
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"

#include"ConstantData/MeshletData.h"

// 頂点データ構造体
struct VertexData {
	Vector4 position = { 0.0f };
	Vector2 texcoord = { 0.0f };
	Vector3 normal = { 0.0f };
};
struct VertexSize {
	uint32_t vertices = 0;
	uint32_t indices = 0;
};
struct MeshData {
	std::string name;
	bool isAnimation = false;
	void* mappedVertices = nullptr;
	void* mappedIndices = nullptr;
	std::vector<VertexData>     vertices;
	std::vector<uint32_t>       indices;
	std::vector<ResMeshlet>         meshlets;               //!< メッシュレット.
	std::vector<uint32_t>           uniqueVertexIndices;    //!< ユニーク頂点インデックス.
	std::vector<ResPrimitiveIndex>  primitiveIndices;       //!< プリミティブインデックス.
	VertexSize size;
	uint32_t meshViewIndex = 0;
	uint32_t srvVBVIndex = 0;
	uint32_t srvIBVIndex = 0;
	uint32_t srvMeshletIndex = 0;
	uint32_t srvUniqueVertexIndex = 0;
	uint32_t srvPrimitiveIndex = 0;
};
// スプライト用
struct SpriteVertexData {
	Vector4 position = { 0.0f };
	Vector2 texcoord = { 0.0f };
};
struct SpriteMeshData {
	SpriteVertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	VertexSize size;
	uint32_t meshViewIndex = 0;
};