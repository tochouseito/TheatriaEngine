#include "pch.h"
#include "ModelManager.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "SDK/DirectX/DirectX12/VertexBuffer/VertexBuffer.h"
#include "SDK/DirectX/DirectX12/IndexBuffer/IndexBuffer.h"
#include "ChoMath.h"
#include "Core/ChoLog/ChoLog.h"
#include "Core/Utility/GenerateUnique.h"
using namespace Cho;

bool ModelManager::LoadModelFile(const std::filesystem::path& filePath)
{
	Log::Write(LogLevel::Info, "LoadModelFile");
	// 変数の宣言
	std::string line;// ファイルから読んだ1行を格納するもの
	Assimp::Importer importer;
	std::string filePathString = filePath.string();
	ModelData modelData;
	// 頂点数とインデックス数
	uint32_t vertices=0;// 頂点数
	uint32_t indices=0;// インデックス数

	// ここからファイルを開く
	Log::Write(LogLevel::Info, "Start ReadFile");
	const aiScene* scene = importer.ReadFile(filePathString.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	Log::Write(LogLevel::Info, "End ReadFile");
	std::string err = importer.GetErrorString();
	err = "ErrorString:" + err;
	Log::Write(LogLevel::Info, err);
	if (!scene->HasMeshes())
	{
		Log::Write(LogLevel::Assert, "No Meshes");// メッシュがないのは対応しない
	}
	// SceneのRootNodeを読んでシーン全体の階層構造を作り上げる
	modelData.rootNode = ReadNode(scene->mRootNode);
	// 
	modelData.name = filePath.stem().wstring();
	modelData.name = GenerateUniqueName(modelData.name, m_ModelNameContainer);
	// メッシュのデータを保存
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		MeshData meshData;
		aiMesh* mesh = scene->mMeshes[meshIndex];
		meshData.name = ConvertString(mesh->mName.C_Str());
		vertices = mesh->mNumVertices;
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			aiFace& face = mesh->mFaces[faceIndex];
			// 三角形の処理
			if (face.mNumIndices == 3)
			{
				for (uint32_t element = 0; element < face.mNumIndices; ++element)
				{
					indices++;
				}
			}
			// 四角形の処理（四角形の2つの三角形に分割）
			else if (face.mNumIndices == 4)
			{
				// 四角形の1つ目の三角形(0,1,2)
				indices++;
				indices++;
				indices++;

				// 四角形の2つ目の三角形(0,1,2)
				indices++;
				indices++;
				indices++;
			}
			// サポート外のポリゴン数の場合のエラーチェック
			else
			{
				assert(false && "Unsupported polygon type");
			}
		}

		// メモリ確保
		meshData.vertices.resize(vertices);
		meshData.indices.resize(indices);

		modelData.meshes.push_back(meshData);
	}

	// Meshの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());// 法線がないMeshは非対応
		std::string meshName = mesh->mName.C_Str();
		std::wstring meshName2 = ConvertString(mesh->mName.C_Str());
		//modelData->objects[meshName].material.isTexture = mesh->HasTextureCoords(0);

		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
		{
			//VertexData vertex;
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			// UV座標のチェックと設定
			if (mesh->HasTextureCoords(0))
			{
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				modelData.meshes[meshIndex].vertices[vertexIndex].texCoord = { texcoord.x,texcoord.y };
			} else
			{
				modelData.meshes[meshIndex].vertices[vertexIndex].texCoord = { 0.0f,0.0f };// ダミーのUV座標
			}
			modelData.meshes[meshIndex].vertices[vertexIndex].position = { position.x,position.y,position.z,1.0f };
			modelData.meshes[meshIndex].vertices[vertexIndex].normal = { normal.x,normal.y,normal.z };
			modelData.meshes[meshIndex].vertices[vertexIndex].position.x *= -1.0f;
			modelData.meshes[meshIndex].vertices[vertexIndex].normal.x *= -1.0f;
			modelData.meshes[meshIndex].vertices[vertexIndex].color = { 1.0f,1.0f,1.0f,1.0f };
			modelData.meshes[meshIndex].vertices[vertexIndex].vertexID = vertexIndex;
		}
		// Index解析
		uint32_t indexCount = 0;
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			aiFace& face = mesh->mFaces[faceIndex];
			// 三角形の処理
			if (face.mNumIndices == 3)
			{
				for (uint32_t element = 0; element < face.mNumIndices; ++element)
				{
					uint32_t vertexIndex = face.mIndices[element];
					modelData.meshes[meshIndex].indices[indexCount] = vertexIndex;
					indexCount++;
				}
			}
			// 四角形の処理（四角形の2つの三角形に分割）
			else if (face.mNumIndices == 4)
			{
				// 四角形の1つ目の三角形(0,1,2)
				modelData.meshes[meshIndex].indices[indexCount] = face.mIndices[0];
				indexCount++;
				modelData.meshes[meshIndex].indices[indexCount] = face.mIndices[1];
				indexCount++;
				modelData.meshes[meshIndex].indices[indexCount] = face.mIndices[2];
				indexCount++;

				// 四角形の2つ目の三角形(0,1,2)
				modelData.meshes[meshIndex].indices[indexCount] = face.mIndices[0];
				indexCount++;
				modelData.meshes[meshIndex].indices[indexCount] = face.mIndices[2];
				indexCount++;
				modelData.meshes[meshIndex].indices[indexCount] = face.mIndices[3];
			}
			// サポート外のポリゴン数の場合のエラーチェック
			else
			{
				assert(false && "Unsupported polygon type");
			}
		}

		//// マテリアル解析
		//uint32_t materialIndex = mesh->mMaterialIndex;
		//aiMaterial* material = scene->mMaterials[materialIndex];
		//if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0)
		//{
		//	aiString textureFilePath;
		//	material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
		//	// 画像テクスチャの読み込み処理
		//	texLoader_->Load(textureFilePath.C_Str());

		//	modelData->objects[meshName].material.textureName = fs::path(textureFilePath.C_Str()).filename().string();
		//	modelData->objects[meshName].material.isTexture = true;
		//} else
		//{
		//	modelData->objects[meshName].material.isTexture = false;
		//}
	}
	// modelDataを追加
	AddModelData(modelData);
	return true;
}

// 名前で検索してインデックスを取得する
std::optional<uint32_t> ModelManager::GetModelDataIndex(const std::wstring& name)
{
	if (m_ModelNameContainer.contains(name))
	{
		return m_ModelNameContainer[name];
	} else
	{
		std::string modelName = ConvertString(name);
		std::string msg = "Model name not found: " + modelName;
		Log::Write(LogLevel::Assert, msg);
	}
	return std::nullopt;
}

void ModelManager::RegisterModelUseList(const std::variant<uint32_t, std::wstring>& key, const uint32_t& transformMapID)
{
	// コンテナのキー
	uint32_t keyIndex = 0;
	if (std::holds_alternative<uint32_t>(key))
	{
		keyIndex = std::get<uint32_t>(key);
	} else if (std::holds_alternative<std::wstring>(key))
	{
		std::wstring name = std::get<std::wstring>(key);
		if (!m_ModelNameContainer.contains(name))
		{
			Log::Write(LogLevel::Assert, "Model name not found: " + ConvertString(name));
			return;
		}
		keyIndex = m_ModelNameContainer[name];
	}
	// 既にUseListに登録されている場合は登録しない
	if (!m_Models[keyIndex].useTransformList.empty())
	{
		// 指定されたモデルに登録されているか確認
		for (auto& index : m_Models[keyIndex].useTransformList)
		{
			if (index == transformMapID)
			{
				return;
			}
		}
	}
	// ほかのモデルに登録されていたら、削除
	for (auto& model : m_Models.GetVector())
	{
		if (model.useTransformList.size() > 0)
		{
			for (auto& index : model.useTransformList)
			{
				if (index == transformMapID)
				{
					model.useTransformList.remove(index);
					break;
				}
			}
		}
	}
	// どのモデルのUseListにない場合は、追加
	m_Models[keyIndex].useTransformList.push_back(transformMapID);
	// UseListのバッファ更新
	StructuredBuffer<uint32_t>* buffer = dynamic_cast<StructuredBuffer<uint32_t>*>(m_pResourceManager->GetBuffer<IStructuredBuffer>(m_Models[keyIndex].useTransformBufferIndex));
	if (buffer)
	{
		// UseListの全てをバッファに転送
		uint32_t i = 0;
		for (uint32_t& useIndex : m_Models[keyIndex].useTransformList)
		{
			buffer->UpdateData(useIndex, i);
			i++;
		}
	} else
	{
		Log::Write(LogLevel::Assert, "Buffer is nullptr");
	}
}

void ModelManager::RemoveModelUseList(const std::variant<uint32_t, std::wstring>& key, const uint32_t& transformMapID)
{
	// コンテナのキー
	uint32_t keyIndex = 0;
	if (std::holds_alternative<uint32_t>(key))
	{
		keyIndex = std::get<uint32_t>(key);
	} else if (std::holds_alternative<std::wstring>(key))
	{
		std::wstring name = std::get<std::wstring>(key);
		if (!m_ModelNameContainer.contains(name))
		{
			Log::Write(LogLevel::Assert, "Model name not found: " + ConvertString(name));
			return;
		}
		keyIndex = m_ModelNameContainer[name];
	}
	// 既にUseListに登録されている場合は登録しない
	if (!m_Models[keyIndex].useTransformList.empty())
	{
		// 指定されたモデルに登録されているか確認、削除する
		for (auto& index : m_Models[keyIndex].useTransformList)
		{
			if (index == transformMapID)
			{
				// 見つかったら削除
				m_Models[keyIndex].useTransformList.remove(transformMapID);
				break;
			}
		}
		// ほかのモデルに登録されていたら、削除
		for (auto& model : m_Models.GetVector())
		{
			if (model.useTransformList.size() > 0)
			{
				for (auto& index : model.useTransformList)
				{
					if (index == transformMapID)
					{
						model.useTransformList.remove(transformMapID);
						break;
					}
				}
			}
		}
	}
	// どのモデルのUseListにない場合は終わり
	// UseListのバッファ更新
	StructuredBuffer<uint32_t>* buffer = dynamic_cast<StructuredBuffer<uint32_t>*>(m_pResourceManager->GetBuffer<IStructuredBuffer>(m_Models[keyIndex].useTransformBufferIndex));
	if (buffer)
	{
		// UseListの全てをバッファに転送
		uint32_t i = 0;
		for (uint32_t& useIndex : m_Models[keyIndex].useTransformList)
		{
			buffer->UpdateData(useIndex, i);
			i++;
		}
	} else
	{
		Log::Write(LogLevel::Assert, "Buffer is nullptr");
	}
}

void ModelManager::CreateDefaultMesh()
{
	// Cube
	CreateCube();
	// Sphere
	CreateSphere();
	// Plane
	CreatePlane();
}

void ModelManager::CreateCube()
{
	// Cube
	std::wstring modelName = L"Cube";
	modelName = GenerateUniqueName(modelName, m_ModelNameContainer);
	ModelData modelData;
	modelData.name = modelName;
	MeshData meshData;
	meshData.name = modelName;
	// 頂点数とインデックス数
	uint32_t vertices = 24;// 頂点数
	uint32_t indices = 36;// インデックス数
	// メモリ確保
	meshData.vertices.resize(vertices);
	meshData.indices.resize(indices);
	// 頂点データを設定
#pragma region
	// 右面
	meshData.vertices[0] = { {0.5f,  0.5f,  0.5f, 1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f},{1.0f,1.0f,1.0f,1.0f},{0} }; // 右上
	meshData.vertices[1] = { {0.5f,  0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} ,{1.0f,1.0f,1.0f,1.0f},{1} }; // 左上
	meshData.vertices[2] = { {0.5f, -0.5f,  0.5f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} ,{1.0f,1.0f,1.0f,1.0f},{2} }; // 右下
	meshData.vertices[3] = { {0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f},{1.0f,1.0f,1.0f,1.0f},{3} }; // 左下

	// 左面
	meshData.vertices[4] = { {-0.5f,  0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} ,{1.0f,1.0f,1.0f,1.0f},{4} }; // 左上
	meshData.vertices[5] = { {-0.5f,  0.5f,  0.5f, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} ,{1.0f,1.0f,1.0f,1.0f},{5} }; // 右上
	meshData.vertices[6] = { {-0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} ,{1.0f,1.0f,1.0f,1.0f},{6} }; // 左下
	meshData.vertices[7] = { {-0.5f, -0.5f,  0.5f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} ,{1.0f,1.0f,1.0f,1.0f},{7} }; // 右下

	// 前面
	meshData.vertices[8] = { {-0.5f,  0.5f,  0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} ,{1.0f,1.0f,1.0f,1.0f},{8} }; // 左上
	meshData.vertices[9] = { { 0.5f,  0.5f,  0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} ,{1.0f,1.0f,1.0f,1.0f},{9} }; // 右上
	meshData.vertices[10] = { {-0.5f, -0.5f,  0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} ,{1.0f,1.0f,1.0f,1.0f},{10} }; // 左下
	meshData.vertices[11] = { { 0.5f, -0.5f,  0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} ,{1.0f,1.0f,1.0f,1.0f},{11} }; // 右下

	// 後面
	meshData.vertices[12] = { { 0.5f,  0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} ,{1.0f,1.0f,1.0f,1.0f},{12} }; // 右上
	meshData.vertices[13] = { {-0.5f,  0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} ,{1.0f,1.0f,1.0f,1.0f},{13} }; // 左上
	meshData.vertices[14] = { { 0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} ,{1.0f,1.0f,1.0f,1.0f},{14} }; // 右下
	meshData.vertices[15] = { {-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f},{1.0f,1.0f,1.0f,1.0f},{15} }; // 左下

	// 上面
	meshData.vertices[16] = { {-0.5f,  0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} ,{1.0f,1.0f,1.0f,1.0f},{16} }; // 左奥
	meshData.vertices[17] = { { 0.5f,  0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} ,{1.0f,1.0f,1.0f,1.0f},{17} }; // 右奥
	meshData.vertices[18] = { {-0.5f,  0.5f,  0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f} ,{1.0f,1.0f,1.0f,1.0f},{18} }; // 左前
	meshData.vertices[19] = { { 0.5f,  0.5f,  0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} ,{1.0f,1.0f,1.0f,1.0f},{19} }; // 右前

	// 下面
	meshData.vertices[20] = { {-0.5f, -0.5f,  0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} ,{1.0f,1.0f,1.0f,1.0f},{20} }; // 左前
	meshData.vertices[21] = { { 0.5f, -0.5f,  0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f} ,{1.0f,1.0f,1.0f,1.0f},{21} }; // 右前
	meshData.vertices[22] = { {-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f} ,{1.0f,1.0f,1.0f,1.0f},{22} }; // 左奥
	meshData.vertices[23] = { { 0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} ,{1.0f,1.0f,1.0f,1.0f},{23} }; // 右奥

	// 右面インデックス
	meshData.indices[0] = 0; meshData.indices[1] = 2; meshData.indices[2] = 1;
	meshData.indices[3] = 2; meshData.indices[4] = 3; meshData.indices[5] = 1;

	// 左面インデックス
	meshData.indices[6] = 4; meshData.indices[7] = 6; meshData.indices[8] = 5;
	meshData.indices[9] = 6; meshData.indices[10] = 7; meshData.indices[11] = 5;

	// 前面インデックス
	meshData.indices[12] = 8; meshData.indices[13] = 10; meshData.indices[14] = 9;
	meshData.indices[15] = 10; meshData.indices[16] = 11; meshData.indices[17] = 9;

	// 後面インデックス
	meshData.indices[18] = 12; meshData.indices[19] = 14; meshData.indices[20] = 13;
	meshData.indices[21] = 14; meshData.indices[22] = 15; meshData.indices[23] = 13;

	// 上面インデックス
	meshData.indices[24] = 16; meshData.indices[25] = 18; meshData.indices[26] = 17;
	meshData.indices[27] = 18; meshData.indices[28] = 19; meshData.indices[29] = 17;

	// 下面インデックス
	meshData.indices[30] = 20; meshData.indices[31] = 22; meshData.indices[32] = 21;
	meshData.indices[33] = 22; meshData.indices[34] = 23; meshData.indices[35] = 21;
#pragma endregion
	// コンテナに追加
	modelData.meshes.push_back(meshData);
	// modelDataを追加
	AddModelData(modelData);
}

void ModelManager::CreateSphere()
{
	// Sphere
	std::wstring modelName = L"Sphere";
	modelName = GenerateUniqueName(modelName, m_ModelNameContainer);
	ModelData modelData;
	modelData.name = modelName;
	MeshData meshData;
	meshData.name = modelName;
	// 頂点数とインデックス数
	uint32_t kSubdivision = 32; // 分割数
	float kLonEvery = 2.0f * PiF / kSubdivision;
	float kLatEvery = PiF / kSubdivision;
	uint32_t vertices = kSubdivision * kSubdivision * 4;
	uint32_t indices = kSubdivision * kSubdivision * 6;
	// メモリ確保
	meshData.vertices.resize(vertices);
	meshData.indices.resize(indices);
	// 頂点データを設定
#pragma region
	// 緯度の方向に分割　-π/2 ～ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex)
	{
		float lat = -PiF / 2.0f + kLatEvery * latIndex; // 現在の緯度

		// 経度の方向に分割 0 ～ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
		{
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 4;;
			float lon = lonIndex * kLonEvery; // 現在の経度
			// 頂点にデータを入力する
			// A頂点
			meshData.vertices[start].position.x = cos(lat) * cos(lon);
			meshData.vertices[start].position.y = sin(lat);
			meshData.vertices[start].position.z = cos(lat) * sin(lon);
			meshData.vertices[start].position.w = 1.0f;
			meshData.vertices[start].texCoord = { float(lonIndex) / float(kSubdivision),1.0f - float(latIndex) / float(kSubdivision) };
			meshData.vertices[start].normal.x = meshData.vertices[start].position.x;
			meshData.vertices[start].normal.y = meshData.vertices[start].position.y;
			meshData.vertices[start].normal.z = meshData.vertices[start].position.z;
			meshData.vertices[start].color = { 1.0f,1.0f,1.0f,1.0f };
			meshData.vertices[start].vertexID = start;
			// B頂点
			meshData.vertices[start + 1].position.x = cos(lat + kLatEvery) * cos(lon);
			meshData.vertices[start + 1].position.y = sin(lat + kLatEvery);
			meshData.vertices[start + 1].position.z = cos(lat + kLatEvery) * sin(lon);
			meshData.vertices[start + 1].position.w = 1.0f;
			meshData.vertices[start + 1].texCoord = { float(lonIndex) / float(kSubdivision),1.0f - float(latIndex + 1) / float(kSubdivision) };
			meshData.vertices[start + 1].normal.x = meshData.vertices[start + 1].position.x;
			meshData.vertices[start + 1].normal.y = meshData.vertices[start + 1].position.y;
			meshData.vertices[start + 1].normal.z = meshData.vertices[start + 1].position.z;
			meshData.vertices[start + 1].color = { 1.0f,1.0f,1.0f,1.0f };
			meshData.vertices[start + 1].vertexID = start + 1;
			// C頂点
			meshData.vertices[start + 2].position.x = cos(lat) * cos(lon + kLonEvery);
			meshData.vertices[start + 2].position.y = sin(lat);
			meshData.vertices[start + 2].position.z = cos(lat) * sin(lon + kLonEvery);
			meshData.vertices[start + 2].position.w = 1.0f;
			meshData.vertices[start + 2].texCoord = { float(lonIndex + 1) / float(kSubdivision),1.0f - float(latIndex) / float(kSubdivision) };
			meshData.vertices[start + 2].normal.x = meshData.vertices[start + 2].position.x;
			meshData.vertices[start + 2].normal.y = meshData.vertices[start + 2].position.y;
			meshData.vertices[start + 2].normal.z = meshData.vertices[start + 2].position.z;
			meshData.vertices[start + 2].color = { 1.0f,1.0f,1.0f,1.0f };
			meshData.vertices[start + 2].vertexID = start + 2;
			// D頂点
			meshData.vertices[start + 3].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery);
			meshData.vertices[start + 3].position.y = sin(lat + kLatEvery);
			meshData.vertices[start + 3].position.z = cos(lat + kLatEvery) * sin(lon + kLonEvery);
			meshData.vertices[start + 3].position.w = 1.0f;
			meshData.vertices[start + 3].texCoord = { float(lonIndex + 1) / float(kSubdivision),1.0f - float(latIndex + 1) / float(kSubdivision) };
			meshData.vertices[start + 3].normal.x = meshData.vertices[start + 3].position.x;
			meshData.vertices[start + 3].normal.y = meshData.vertices[start + 3].position.y;
			meshData.vertices[start + 3].normal.z = meshData.vertices[start + 3].position.z;
			meshData.vertices[start + 3].color = { 1.0f,1.0f,1.0f,1.0f };
			meshData.vertices[start + 3].vertexID = start + 3;

		}
	}
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex)
	{
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
		{
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			uint32_t startIndex = (latIndex * kSubdivision + lonIndex) * 4;
			meshData.indices[start] = 0 + startIndex;
			meshData.indices[start + 1] = 1 + startIndex;
			meshData.indices[start + 2] = 2 + startIndex;
			meshData.indices[start + 3] = 1 + startIndex;
			meshData.indices[start + 4] = 3 + startIndex;
			meshData.indices[start + 5] = 2 + startIndex;
		}
	}
#pragma endregion
	// コンテナに追加
	modelData.meshes.push_back(meshData);
	// modelDataを追加
	AddModelData(modelData);
}

void ModelManager::CreatePlane()
{
	// Plane
	std::wstring modelName = L"Plane";
	modelName = GenerateUniqueName(modelName, m_ModelNameContainer);
	ModelData modelData;
	modelData.name = modelName;
	MeshData meshData;
	meshData.name = modelName;
	// 頂点数とインデックス数
	uint32_t vertices = 4;// 頂点数
	uint32_t indices = 6;// インデックス数
	// メモリ確保
	meshData.vertices.resize(vertices);
	meshData.indices.resize(indices);
	// 頂点データを設定
#pragma region
	// 頂点データ（重複なし）
	meshData.vertices[0] = { { 1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f },{1.0f,1.0f,1.0f,1.0f},{0} };  // 右上
	meshData.vertices[1] = { {-1.0f,  1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } ,{1.0f,1.0f,1.0f,1.0f},{1} };  // 左上
	meshData.vertices[2] = { { 1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } ,{1.0f,1.0f,1.0f,1.0f},{2} };  // 右下
	meshData.vertices[3] = { {-1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f },{1.0f,1.0f,1.0f,1.0f},{3} };  // 左下
	// インデックスデータ
	meshData.indices[0] = 0;
	meshData.indices[1] = 1;
	meshData.indices[2] = 2;
	meshData.indices[3] = 1;
	meshData.indices[4] = 3;
	meshData.indices[5] = 2;
#pragma endregion
	// コンテナに追加
	modelData.meshes.push_back(meshData);
	// modelDataを追加
	AddModelData(modelData);
}

Node ModelManager::ReadNode(aiNode* node)
{
	Node result;
	Node result2;
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;// nodeのlocalMatrixを取得
	aiLocalMatrix.Transpose();// 列ベクトルを行ベクトルに転置
	for (uint32_t mindex = 0; mindex < 4; ++mindex)
	{
		for (uint32_t index = 0; index < 4; ++index)
		{
			result2.localMatrix.m[mindex][index] = aiLocalMatrix[mindex][index];
		}
	}
	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate);// assimpの行列からSRTを抽出する関数を利用
	result.transform.scale = { scale.x,scale.y,scale.z };// scaleはそのまま
	result.transform.rotation = { rotate.x,-rotate.y,-rotate.z,rotate.w };// x軸を反転。さらに回転方向が逆なので軸を反転させる
	result.transform.translation = { -translate.x,translate.y,translate.z };// x軸を反転
	result.localMatrix = ChoMath::MakeAffineMatrix(result.transform.scale, result.transform.rotation, result.transform.translation);
	result.name = node->mName.C_Str();// Node名を格納
	result.children.resize(node->mNumChildren);// 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
	{
		// 再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}

// ModelDataの追加
void ModelManager::AddModelData(ModelData& modelData)
{
	for (MeshData& mesh : modelData.meshes)
	{
		// VertexBuffer,IndexBuffer作成
		mesh.vertexBufferIndex = m_pResourceManager->CreateVertexBuffer<VertexData>(static_cast<UINT>(mesh.vertices.size()));
		mesh.indexBufferIndex = m_pResourceManager->CreateIndexBuffer<uint32_t>(static_cast<UINT>(mesh.indices.size()));
		VertexBuffer<VertexData>* vertexBuffer = dynamic_cast<VertexBuffer<VertexData>*>(m_pResourceManager->GetBuffer<IVertexBuffer>(mesh.vertexBufferIndex));
		IndexBuffer<uint32_t>* indexBuffer = dynamic_cast<IndexBuffer<uint32_t>*>(m_pResourceManager->GetBuffer<IIndexBuffer>(mesh.indexBufferIndex));
		// VBV,IBV作成
		vertexBuffer->CreateVBV();
		indexBuffer->CreateIBV();
		// コピー
		vertexBuffer->MappedDataCopy(mesh.vertices);
		indexBuffer->MappedDataCopy(mesh.indices);
	}
	// UseTransformのリソースを作成
	modelData.useTransformBufferIndex = m_pResourceManager->CreateStructuredBuffer<uint32_t>(kUseTransformOffset);
	// 名前が重複していたら、エラー
	// ここに処理を追加する

	// モデルをコンテナに追加
	std::wstring name = modelData.name;
	uint32_t index = static_cast<uint32_t>(m_Models.push_back(std::move(modelData)));
	// 名前コンテナに登録
	m_ModelNameContainer[name] = index;
}
