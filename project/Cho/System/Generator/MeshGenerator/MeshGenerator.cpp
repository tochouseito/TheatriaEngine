#include "PrecompiledHeader.h"
#include "MeshGenerator.h"
#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"ConstantData/LineData.h"
// meshoptimizer
#include <meshoptimizer.h>

Meshes MeshGenerator::GeneratorMeshes(MeshPattern pattern, ResourceViewManager* rvManager)
{
	Meshes meshes;
	MeshData meshData;
	std::string name;
	switch (pattern)
	{
	case MeshPattern::Cube:
	{
		// 立方体生成
		name = "Cube";
		meshData.name = name;
		meshData.meshViewIndex = static_cast<uint32_t>(pattern);
		meshData.size = CreateCube(meshData, rvManager);
		meshes.meshesName = name;
		meshes.meshData.push_back(meshData);
	}
		break;
	case MeshPattern::Plane:
	{
		// 平面生成
		name = "Plane";
		meshData.name = name;
		meshData.meshViewIndex = static_cast<uint32_t>(pattern);
		meshData.size = CreatePlane(meshData, rvManager);
		meshes.meshesName = name;
		meshes.meshData.push_back(meshData);
		/*meshes.names.push_back(name);
		MeshData& meshData = meshes.meshData[name];
		meshData.meshViewIndex = static_cast<uint32_t>(pattern);
		meshData.size = CreatePlane(meshData, rvManager);*/
	}
		break;
	case MeshPattern::Sphere:
	{
		// 球体生成
		name = "Sphere";
		meshData.name = name;
		meshData.meshViewIndex = static_cast<uint32_t>(pattern);
		meshData.isAnimation = false;
		meshData.size = CreateSphere(meshData, rvManager);
		meshes.meshesName = name;
		meshes.meshData.push_back(meshData);
		/*meshes.names.push_back(name);
		MeshData& meshData = meshes.meshData[name];
		meshData.meshViewIndex = static_cast<uint32_t>(pattern);
		meshData.size = CreateSphere(meshData, rvManager);*/
	}
		break;
	case MeshPattern::CountPattern:
		// ここを通るはずがない
		assert(0);
		break;
	default:
		break;
	}
	return meshes;
}

VertexSize MeshGenerator::CreateSphere(MeshData& data, ResourceViewManager* rvManager)
{
	VertexSize result;

	uint32_t kSubdivision = 32;
	float kLonEvery = 2.0f * std::numbers::pi_v<float> / kSubdivision;
	float kLatEvery = std::numbers::pi_v<float> / kSubdivision;
	uint32_t vertices = kSubdivision * kSubdivision * 4;
	uint32_t indices = kSubdivision * kSubdivision * 6;

	result.vertices = vertices;
	result.indices = indices;

	data.srvVBVIndex = rvManager->GetNewHandle();

	// VBV用SRVリソース作成
	rvManager->CreateSRVResource(
		data.srvVBVIndex,
		sizeof(VertexData) * vertices
	);

	rvManager->CreateSRVforStructuredBuffer(
		data.srvVBVIndex,
		static_cast<UINT>(vertices),
		static_cast<UINT>(sizeof(VertexData))
	);

	// IBV用SRVリソース作成
	data.srvIBVIndex = rvManager->GetNewHandle();
	rvManager->CreateSRVResource(
		data.srvIBVIndex,
		sizeof(uint32_t) * indices
	);

	rvManager->CreateSRVforStructuredBuffer(
		data.srvIBVIndex,
		static_cast<UINT>(indices),
		static_cast<UINT>(sizeof(uint32_t))
	);

	rvManager->CreateMeshViewDMP(
		data.meshViewIndex,
		vertices,
		indices,
		rvManager->GetHandle(data.srvVBVIndex).resource.Get(),
		rvManager->GetHandle(data.srvIBVIndex).resource.Get()
	);

	// メモリ確保
	data.vertices.resize(vertices);
	data.indices.resize(indices);

	rvManager->GetMeshViewData(data.meshViewIndex)->vbvData.resource->Map(
		0, nullptr, &data.mappedVertices
	);
	rvManager->GetMeshViewData(data.meshViewIndex)->ibvData.resource->Map(
		0, nullptr, &data.mappedIndices
	);

#pragma region
	// 緯度の方向に分割　-π/2 ～ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex; // 現在の緯度

		// 経度の方向に分割 0 ～ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 4;//4;
			float lon = lonIndex * kLonEvery; // 現在の経度
			//uint32_t startIndex = (latIndex * kSubdivision + lonIndex) * 6;//4;
			//float u = float(lonIndex) / float(kSubdivision);
			//float v = 1.0f - float(latIndex) / float(kSubdivision);
			// 頂点にデータを入力する
			// A頂点
			data.vertices[start].position.x = cos(lat) * cos(lon);
			data.vertices[start].position.y = sin(lat);
			data.vertices[start].position.z = cos(lat) * sin(lon);
			data.vertices[start].position.w = 1.0f;
			data.vertices[start].texcoord = { float(lonIndex) / float(kSubdivision),1.0f - float(latIndex) / float(kSubdivision) };
			data.vertices[start].normal.x = data.vertices[start].position.x;
			data.vertices[start].normal.y = data.vertices[start].position.y;
			data.vertices[start].normal.z = data.vertices[start].position.z;
			// B頂点
			data.vertices[start + 1].position.x = cos(lat + kLatEvery) * cos(lon);
			data.vertices[start + 1].position.y = sin(lat + kLatEvery);
			data.vertices[start + 1].position.z = cos(lat + kLatEvery) * sin(lon);
			data.vertices[start + 1].position.w = 1.0f;
			data.vertices[start + 1].texcoord = { float(lonIndex) / float(kSubdivision),1.0f - float(latIndex + 1) / float(kSubdivision) };
			data.vertices[start + 1].normal.x = data.vertices[start + 1].position.x;
			data.vertices[start + 1].normal.y = data.vertices[start + 1].position.y;
			data.vertices[start + 1].normal.z = data.vertices[start + 1].position.z;
			// C頂点
			data.vertices[start + 2].position.x = cos(lat) * cos(lon + kLonEvery);
			data.vertices[start + 2].position.y = sin(lat);
			data.vertices[start + 2].position.z = cos(lat) * sin(lon + kLonEvery);
			data.vertices[start + 2].position.w = 1.0f;
			data.vertices[start + 2].texcoord = { float(lonIndex + 1) / float(kSubdivision),1.0f - float(latIndex) / float(kSubdivision) };
			data.vertices[start + 2].normal.x = data.vertices[start + 2].position.x;
			data.vertices[start + 2].normal.y = data.vertices[start + 2].position.y;
			data.vertices[start + 2].normal.z = data.vertices[start + 2].position.z;
			// D頂点
			data.vertices[start + 3].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery);
			data.vertices[start + 3].position.y = sin(lat + kLatEvery);
			data.vertices[start + 3].position.z = cos(lat + kLatEvery) * sin(lon + kLonEvery);
			data.vertices[start + 3].position.w = 1.0f;
			data.vertices[start + 3].texcoord = { float(lonIndex + 1) / float(kSubdivision),1.0f - float(latIndex + 1) / float(kSubdivision) };
			data.vertices[start + 3].normal.x = data.vertices[start + 3].position.x;
			data.vertices[start + 3].normal.y = data.vertices[start + 3].position.y;
			data.vertices[start + 3].normal.z = data.vertices[start + 3].position.z;

		}
	}
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			uint32_t startIndex = (latIndex * kSubdivision + lonIndex) * 4;
			data.indices[start] = 0 + startIndex;
			data.indices[start + 1] = 1 + startIndex;
			data.indices[start + 2] = 2 + startIndex;
			data.indices[start + 3] = 1 + startIndex;
			data.indices[start + 4] = 3 + startIndex;
			data.indices[start + 5] = 2 + startIndex;
		}
	}

	// コピー
	memcpy(data.mappedVertices, data.vertices.data(), sizeof(VertexData) * vertices);
	memcpy(data.mappedIndices, data.indices.data(), sizeof(uint32_t) * indices);

	// マップ解除
	rvManager->GetMeshViewData(data.meshViewIndex)->vbvData.resource->Unmap(0, nullptr);
	rvManager->GetMeshViewData(data.meshViewIndex)->ibvData.resource->Unmap(0, nullptr);

#pragma endregion
	return result;
}

VertexSize MeshGenerator::CreateCube(MeshData& data, ResourceViewManager* rvManager)
{
	VertexSize result;

	uint32_t vertices = 24;
	uint32_t indices = 36;

	result.vertices = vertices;
	result.indices = indices;

	// VBV用SRVリソース作成
	data.srvVBVIndex = rvManager->GetNewHandle();
	rvManager->CreateSRVResource(
		data.srvVBVIndex,
		sizeof(VertexData) * vertices
	);

	rvManager->CreateSRVforStructuredBuffer(
		data.srvVBVIndex,
		static_cast<UINT>(vertices),
		static_cast<UINT>(sizeof(VertexData))
	);

	// IBV用SRVリソース作成
	data.srvIBVIndex = rvManager->GetNewHandle();
	rvManager->CreateSRVResource(
		data.srvIBVIndex,
		sizeof(uint32_t) * indices
	);

	rvManager->CreateSRVforStructuredBuffer(
		data.srvIBVIndex,
		static_cast<UINT>(indices),
		static_cast<UINT>(sizeof(uint32_t))
	);

	rvManager->CreateMeshViewDMP(
		data.meshViewIndex,
		vertices,
		indices,
		rvManager->GetHandle(data.srvVBVIndex).resource.Get(),
		rvManager->GetHandle(data.srvIBVIndex).resource.Get()
	);

	// メモリ確保
	data.vertices.resize(vertices);
	data.indices.resize(indices);

	rvManager->GetMeshViewData(data.meshViewIndex)->vbvData.resource->Map(
		0, nullptr, &data.mappedVertices
	);
	rvManager->GetMeshViewData(data.meshViewIndex)->ibvData.resource->Map(
		0, nullptr, &data.mappedIndices
	);
#pragma region
	// 右面
	//data.vertices[0]={ {1.0f,  1.0f,  1.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }; // 右上
	//data.vertices[1]={ {1.0f,  1.0f, -1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }; // 左上
	//data.vertices[2]={ {1.0f, -1.0f,  1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} }; // 右下
	//data.vertices[3]={ {1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f} }; // 左下

	data.vertices[0] = { {0.5f,  0.5f,  0.5f, 1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }; // 右上
	data.vertices[1] = { {0.5f,  0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }; // 左上
	data.vertices[2] = { {0.5f, -0.5f,  0.5f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} }; // 右下
	data.vertices[3] = { {0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f} }; // 左下

	// 左面
	//data.vertices[4]={ {-1.0f,  1.0f, -1.0f, 1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} }; // 左上
	//data.vertices[5]={ {-1.0f,  1.0f,  1.0f, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} }; // 右上
	//data.vertices[6]={ {-1.0f, -1.0f, -1.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} }; // 左下
	//data.vertices[7]={ {-1.0f, -1.0f,  1.0f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} }; // 右下

	data.vertices[4] = { {-0.5f,  0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} }; // 左上
	data.vertices[5] = { {-0.5f,  0.5f,  0.5f, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} }; // 右上
	data.vertices[6] = { {-0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} }; // 左下
	data.vertices[7] = { {-0.5f, -0.5f,  0.5f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} }; // 右下

	// 前面
	//data.vertices[8]={ {-1.0f,  1.0f,  1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} }; // 左上
	//data.vertices[9]={ { 1.0f,  1.0f,  1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} }; // 右上
	//data.vertices[10]={ {-1.0f, -1.0f,  1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} }; // 左下
	//data.vertices[11]={ { 1.0f, -1.0f,  1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} }; // 右下

	data.vertices[8] = { {-0.5f,  0.5f,  0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} }; // 左上
	data.vertices[9] = { { 0.5f,  0.5f,  0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} }; // 右上
	data.vertices[10] = { {-0.5f, -0.5f,  0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} }; // 左下
	data.vertices[11] = { { 0.5f, -0.5f,  0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} }; // 右下

	// 後面
	//data.vertices[12]={ { 1.0f,  1.0f, -1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }; // 右上
	//data.vertices[13]={ {-1.0f,  1.0f, -1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }; // 左上
	//data.vertices[14]={ { 1.0f, -1.0f, -1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} }; // 右下
	//data.vertices[15]={ {-1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} }; // 左下

	data.vertices[12] = { { 0.5f,  0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }; // 右上
	data.vertices[13] = { {-0.5f,  0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }; // 左上
	data.vertices[14] = { { 0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} }; // 右下
	data.vertices[15] = { {-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} }; // 左下

	// 上面
	//data.vertices[16]={ {-1.0f,  1.0f, -1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} }; // 左奥
	//data.vertices[17]={ { 1.0f,  1.0f, -1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} }; // 右奥
	//data.vertices[18]={ {-1.0f,  1.0f,  1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f} }; // 左前
	//data.vertices[19]={ { 1.0f,  1.0f,  1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} }; // 右前

	data.vertices[16] = { {-0.5f,  0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} }; // 左奥
	data.vertices[17] = { { 0.5f,  0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} }; // 右奥
	data.vertices[18] = { {-0.5f,  0.5f,  0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f} }; // 左前
	data.vertices[19] = { { 0.5f,  0.5f,  0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} }; // 右前

	// 下面
	//data.vertices[20]={ {-1.0f, -1.0f,  1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} }; // 左前
	//data.vertices[21]={ { 1.0f, -1.0f,  1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f} }; // 右前
	//data.vertices[22]={ {-1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f} }; // 左奥
	//data.vertices[23]={ { 1.0f, -1.0f, -1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} }; // 右奥
	
	data.vertices[20] = { {-0.5f, -0.5f,  0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} }; // 左前
	data.vertices[21] = { { 0.5f, -0.5f,  0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f} }; // 右前
	data.vertices[22] = { {-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f} }; // 左奥
	data.vertices[23] = { { 0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} }; // 右奥

	// 右面インデックス
	data.indices[0]=0; data.indices[1]=2; data.indices[2]=1;
	data.indices[3]=2; data.indices[4]=3; data.indices[5]=1;

	// 左面インデックス
	data.indices[6]=4; data.indices[7]=6; data.indices[8]=5;
	data.indices[9]=6; data.indices[10]=7; data.indices[11]=5;

	// 前面インデックス
	data.indices[12]=8; data.indices[13]=10; data.indices[14]=9;
	data.indices[15]=10; data.indices[16]=11; data.indices[17]=9;

	// 後面インデックス
	data.indices[18]=12; data.indices[19]=14; data.indices[20]=13;
	data.indices[21]=14; data.indices[22]=15; data.indices[23]=13;

	// 上面インデックス
	data.indices[24]=16; data.indices[25]=18; data.indices[26]=17;
	data.indices[27]=18; data.indices[28]=19; data.indices[29]=17;

	// 下面インデックス
	data.indices[30]=20; data.indices[31]=22; data.indices[32]=21;
	data.indices[33]=22; data.indices[34]=23; data.indices[35]=21;

	// コピー
	memcpy(data.mappedVertices, data.vertices.data(), sizeof(VertexData)* vertices);
	memcpy(data.mappedIndices, data.indices.data(), sizeof(uint32_t)* indices);

	// マップ解除
	rvManager->GetMeshViewData(data.meshViewIndex)->vbvData.resource->Unmap(0, nullptr);
	rvManager->GetMeshViewData(data.meshViewIndex)->ibvData.resource->Unmap(0, nullptr);

#pragma endregion
	return result;
}

VertexSize MeshGenerator::CreatePlane(MeshData& data, ResourceViewManager* rvManager)
{
	VertexSize result;

	uint32_t vertices = 4;
	uint32_t indices = 6;

	result.vertices = vertices;
	result.indices = indices;

	// VBV用SRVリソース作成
	data.srvVBVIndex = rvManager->GetNewHandle();
	rvManager->CreateSRVResource(
		data.srvVBVIndex,
		sizeof(VertexData) * vertices
	);

	rvManager->CreateSRVforStructuredBuffer(
		data.srvVBVIndex,
		static_cast<UINT>(vertices),
		static_cast<UINT>(sizeof(VertexData))
	);

	// IBV用SRVリソース作成
	data.srvIBVIndex = rvManager->GetNewHandle();
	rvManager->CreateSRVResource(
		data.srvIBVIndex,
		sizeof(uint32_t) * indices
	);

	rvManager->CreateSRVforStructuredBuffer(
		data.srvIBVIndex,
		static_cast<UINT>(indices),
		static_cast<UINT>(sizeof(uint32_t))
	);

	rvManager->CreateMeshViewDMP(
		data.meshViewIndex,
		vertices,
		indices,
		rvManager->GetHandle(data.srvVBVIndex).resource.Get(),
		rvManager->GetHandle(data.srvIBVIndex).resource.Get()
	);

	// メモリ確保
	data.vertices.resize(vertices);
	data.indices.resize(indices);
	
	rvManager->GetMeshViewData(data.meshViewIndex)->vbvData.resource->Map(
		0, nullptr, &data.mappedVertices
	);

	rvManager->GetMeshViewData(data.meshViewIndex)->ibvData.resource->Map(
		0, nullptr, &data.mappedIndices
	);

#pragma region
	// 頂点データ（重複なし）
	data.vertices[0] = { { 1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };  // 右上
	data.vertices[1] = { {-1.0f,  1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };  // 左上
	data.vertices[2] = { { 1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } };  // 右下
	data.vertices[3] = { {-1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } };  // 左下

	data.indices[0] = 0;
	data.indices[1] = 1;
	data.indices[2] = 2;
	data.indices[3] = 1;
	data.indices[4] = 3;
	data.indices[5] = 2;

	// コピー
	memcpy(data.mappedVertices, data.vertices.data(), sizeof(VertexData) * vertices);
	memcpy(data.mappedIndices, data.indices.data(), sizeof(uint32_t) * indices);

	// マップ解除
	rvManager->GetMeshViewData(data.meshViewIndex)->vbvData.resource->Unmap(0, nullptr);
	rvManager->GetMeshViewData(data.meshViewIndex)->ibvData.resource->Unmap(0, nullptr);

	data.mappedVertices = nullptr;
	data.mappedIndices = nullptr;

#pragma endregion
	return result;
}

void MeshGenerator::CreateSprite(SpriteMeshData& data, ResourceViewManager* rvManager)
{
	VertexSize result;

	uint32_t vertices = 4;
	uint32_t indices = 6;

	data.size.vertices = vertices;
	data.size.indices = indices;

	data.meshViewIndex = rvManager->CreateMeshView(data.size.vertices, data.size.indices,sizeof(SpriteVertexData));

	rvManager->GetMeshViewData(data.meshViewIndex)->vbvData.resource->Map(
		0, nullptr, reinterpret_cast<void**>(&data.vertexData)
	);

	rvManager->GetMeshViewData(data.meshViewIndex)->ibvData.resource->Map(
		0, nullptr, reinterpret_cast<void**>(&data.indexData)
	);

	data.vertexData[0] = { {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} };    // 左上
	data.vertexData[1] = { {640.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} };  // 右上
	data.vertexData[2] = { {0.0f, 360.0f, 0.0f, 1.0f}, {0.0f, 1.0f} };  // 左下
	data.vertexData[3] = { {640.0f, 360.0f, 0.0f, 1.0f}, {1.0f, 1.0f} }; // 右下


	// インデックスバッファ (反時計回り CCW)
	data.indexData[0] = 0;  // 左上
	data.indexData[1] = 1;  // 右上
	data.indexData[2] = 2;  // 左下

	data.indexData[3] = 1;  // 右上
	data.indexData[4] = 3;  // 右下
	data.indexData[5] = 2;  // 左下


}

void MeshGenerator::CreateLineMesh(ResourceViewManager* rvManager)
{
	// 正射影リソース
	rvManager->CreateOrthographicResource();

	// 線分メッシュ生成
	UINT lineVertexCount = kMaxLineCount * kVertexCountLine;
	UINT lineIndexCount = kMaxLineCount * kIndexCountLine;

	rvManager->GetLineData()->meshViewIndex = rvManager->CreateMeshView(
		lineVertexCount,
		lineIndexCount,
		sizeof(LineVertex),
		nullptr,
		nullptr
	);

	rvManager->GetMeshViewData(rvManager->GetLineData()->meshViewIndex)->vbvData.resource->Map(
		0, nullptr, reinterpret_cast<void**>(&rvManager->GetLineData()->mapped)
	);
}

// メッシュレット生成
void MeshGenerator::GenerateMeshlets(MeshData& meshData, ResourceViewManager* rvManager, uint32_t maxVertices, uint32_t maxTriangles,float coneWeight)
{
	// メッシュレットの最大数を計算
	size_t maxMeshlets = meshopt_buildMeshletsBound(
		meshData.indices.size(),
		maxVertices,
		maxTriangles
	);

	// メッシュレットのデータを格納するためのバッファを準備
	std::vector<meshopt_Meshlet> meshlets(maxMeshlets);
	std::vector<unsigned int> meshletVertices(maxMeshlets * maxVertices);
	std::vector<unsigned char> meshletTriangles(maxMeshlets * maxTriangles * 3);

	// メッシュレットを生成
	size_t meshletCount = meshopt_buildMeshlets(
		meshlets.data(),                     // 出力: メッシュレットデータ
		meshletVertices.data(),              // 出力: 頂点インデックス
		meshletTriangles.data(),             // 出力: プリミティブインデックス
		meshData.indices.data(),                 // 入力: インデックスバッファ
		meshData.indices.size(),                 // 入力: インデックス数
		reinterpret_cast<const float*>(meshData.vertices.data()), // 入力: 頂点位置データ（XYZを含む）
		meshData.vertices.size(),                // 入力: 頂点数
		sizeof(VertexData),                  // 入力: 頂点データのストライド
		maxVertices,                         // メッシュレットの最大頂点数
		maxTriangles,                        // メッシュレットの最大プリミティブ数
		coneWeight                           // 法線コーンの重み
	);

	// サイズ計算用変数
	size_t totalUniqueVertices = 0;
	size_t totalPrimitives = 0;

	for (size_t i = 0; i < meshletCount; ++i) {
		const auto& meshlet = meshlets[i];
		totalUniqueVertices += meshlet.vertex_count;
		totalPrimitives += meshlet.triangle_count;
	}

	// サイズに合わせてリソース作成
	meshData.srvMeshletIndex = rvManager->GetNewHandle();
	rvManager->CreateSRVResource(
		meshData.srvMeshletIndex,
		sizeof(ResMeshlet) * meshletCount
	);

	rvManager->CreateSRVforStructuredBuffer(
		meshData.srvMeshletIndex,
		static_cast<UINT>(meshletCount),
		static_cast<UINT>(sizeof(ResMeshlet))
	);

	// ユニーク頂点インデックス
	meshData.srvUniqueVertexIndex = rvManager->GetNewHandle();
	rvManager->CreateSRVResource(
		meshData.srvUniqueVertexIndex,
		sizeof(uint32_t) * totalUniqueVertices
	);

	rvManager->CreateSRVforStructuredBuffer(
		meshData.srvUniqueVertexIndex,
		static_cast<UINT>(totalUniqueVertices),
		static_cast<UINT>(sizeof(uint32_t))
	);

	// プリミティブインデックス
	meshData.srvPrimitiveIndex = rvManager->GetNewHandle();
	rvManager->CreateSRVResource(
		meshData.srvPrimitiveIndex,
		sizeof(ResPrimitiveIndex) * totalPrimitives
	);

	rvManager->CreateSRVforStructuredBuffer(
		meshData.srvPrimitiveIndex,
		static_cast<UINT>(totalPrimitives),
		static_cast<UINT>(sizeof(ResPrimitiveIndex))
	);

	ResMeshlet* mappedMeshlets = nullptr;
	uint32_t* mappedUniqueVertexIndices = nullptr;
	ResPrimitiveIndex* mappedPrimitiveIndices = nullptr;

	rvManager->GetHandle(meshData.srvMeshletIndex).resource->Map(0, nullptr, reinterpret_cast<void**>(&mappedMeshlets));
	rvManager->GetHandle(meshData.srvUniqueVertexIndex).resource->Map(0, nullptr, reinterpret_cast<void**>(&mappedUniqueVertexIndices));
	rvManager->GetHandle(meshData.srvPrimitiveIndex).resource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPrimitiveIndices));

	// メッシュレットデータを ResMesh に格納
	meshData.meshlets.clear();
	meshData.uniqueVertexIndices.clear();
	meshData.primitiveIndices.clear();

	for (size_t i = 0; i < meshletCount; ++i) {
		const auto& meshlet = meshlets[i];

		// ResMeshlet を追加
		ResMeshlet resMeshlet;
		resMeshlet.VertexOffset = static_cast<uint32_t>(meshData.uniqueVertexIndices.size());
		resMeshlet.VertexCount = meshlet.vertex_count;
		resMeshlet.PrimitiveOffset = static_cast<uint32_t>(meshData.primitiveIndices.size());
		resMeshlet.PrimitiveCount = meshlet.triangle_count;
		meshData.meshlets.push_back(resMeshlet);

		// ユニーク頂点インデックスを追加
		for (size_t j = 0; j < meshlet.vertex_count; ++j) {
			meshData.uniqueVertexIndices.push_back(meshletVertices[meshlet.vertex_offset + j]);
		}

		// プリミティブインデックス（圧縮形式）を追加
		for (size_t j = 0; j < meshlet.triangle_count; ++j) {
			const unsigned char* tri = &meshletTriangles[meshlet.triangle_offset + j * 3];

			ResPrimitiveIndex resPrimitive;
			resPrimitive.index0 = tri[0];
			resPrimitive.index1 = tri[1];
			resPrimitive.index2 = tri[2];
			resPrimitive.reserved = 0;

			meshData.primitiveIndices.push_back(resPrimitive);
		}
	}

	// メモリコピー
	memcpy(mappedMeshlets, meshData.meshlets.data(), sizeof(ResMeshlet) * meshletCount);
	memcpy(mappedUniqueVertexIndices, meshData.uniqueVertexIndices.data(), sizeof(uint32_t) * totalUniqueVertices);
	memcpy(mappedPrimitiveIndices, meshData.primitiveIndices.data(), sizeof(ResPrimitiveIndex) * totalPrimitives);

	// マップ解除
	rvManager->GetHandle(meshData.srvMeshletIndex).resource->Unmap(0, nullptr);
	rvManager->GetHandle(meshData.srvUniqueVertexIndex).resource->Unmap(0, nullptr);
	rvManager->GetHandle(meshData.srvPrimitiveIndex).resource->Unmap(0, nullptr);

	std::cout << "Generated " << meshletCount << " meshlets with cone optimization.\n";
}

void MeshGenerator::OptimizeMesh(MeshData& meshData, ResourceViewManager* rvManager)
{
	rvManager;
	// 頂点インデックスをリマップするためのバッファ
	std::vector<uint32_t> remap(meshData.indices.size());

	// 頂点リマップを生成
	size_t vertexCount = meshopt_generateVertexRemap(
		remap.data(),                     // 出力: リマップ情報
		meshData.indices.data(),              // 入力: インデックスバッファ
		meshData.indices.size(),              // 入力: インデックス数
		meshData.vertices.data(),             // 入力: 頂点データ
		meshData.vertices.size(),             // 入力: 頂点数
		sizeof(VertexData));              // 頂点データのサイズ

	// リオーダリングされた頂点バッファ
	std::vector<VertexData> optimizedVertices(vertexCount);
	meshopt_remapVertexBuffer(
		optimizedVertices.data(),         // 出力: 再配置された頂点データ
		meshData.vertices.data(),             // 入力: 元の頂点データ
		meshData.vertices.size(),             // 入力: 頂点数
		sizeof(VertexData),               // 頂点データのサイズ
		remap.data());                    // リマップ情報

	// リオーダリングされたインデックスバッファ
	std::vector<uint32_t> optimizedIndices(meshData.indices.size());
	meshopt_remapIndexBuffer(
		optimizedIndices.data(),          // 出力: 再配置されたインデックスデータ
		meshData.indices.data(),              // 入力: 元のインデックスデータ
		meshData.indices.size(),              // 入力: インデックス数
		remap.data());                    // リマップ情報

	// 頂点キャッシュの効率を最適化
	meshopt_optimizeVertexCache(
		optimizedIndices.data(),          // 入力および出力: インデックスバッファ
		optimizedIndices.data(),
		optimizedIndices.size(),
		vertexCount);

	// 頂点フェッチの効率を最適化
	meshopt_optimizeVertexFetch(
		optimizedVertices.data(),         // 出力: 最適化された頂点データ
		optimizedIndices.data(),          // 入力: 最適化されたインデックスバッファ
		optimizedIndices.size(),
		optimizedVertices.data(),         // 入力: 元の頂点データ
		vertexCount,
		sizeof(VertexData));              // 頂点データのサイズ

	// 最適化結果を ResMesh に反映
	meshData.vertices = std::move(optimizedVertices);
	meshData.indices = std::move(optimizedIndices);

	std::cout << "Mesh optimization completed: "
		<< "Vertices=" << meshData.vertices.size()
		<< ", Indices=" << meshData.indices.size() << std::endl;
}

void MeshGenerator::ProcessMeshForMeshShader(MeshData& meshData, ResourceViewManager* rvManager)
{
	// 頂点とインデックスの最適化（リオーダリングと圧縮）
	OptimizeMesh(meshData,rvManager);

	// メッシュレットの生成
	GenerateMeshlets(meshData,rvManager);

	std::cout << "Mesh is now ready for Mesh Shader pipeline!" << std::endl;
}
