#include "pch.h"
#include "ModelManager.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "SDK/DirectX/DirectX12/VertexBuffer/VertexBuffer.h"
#include "SDK/DirectX/DirectX12/IndexBuffer/IndexBuffer.h"
#include "ChoMath.h"
#include "Core/ChoLog/ChoLog.h"
#include "Core/Utility/GenerateUnique.h"
using namespace Cho;
// assimp
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

bool ModelManager::LoadModelFile(const std::filesystem::path& filePath)
{
	Log::Write(LogLevel::Info, "LoadModelFile");
	// 変数の宣言
	std::string line;// ファイルから読んだ1行を格納するもの
	Assimp::Importer importer;
	std::string filePathString = filePath.string();

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
	modelData->rootNode = ReadNode(scene->mRootNode);

	// メッシュのデータを保存
	Meshes meshes;
	meshes.meshesName = entry.path().stem().string();
	// 各メッシュの名前を保存
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		MeshData meshData;
		aiMesh* mesh = scene->mMeshes[meshIndex];
		meshData.name = mesh->mName.C_Str();
		modelData->meshNames.push_back(meshData.name);
		modelData->objects[meshData.name];// ObjectDataを作成
		meshData.size.vertices = mesh->mNumVertices;
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			aiFace& face = mesh->mFaces[faceIndex];
			// 三角形の処理
			if (face.mNumIndices == 3)
			{
				for (uint32_t element = 0; element < face.mNumIndices; ++element)
				{
					meshData.size.indices++;
				}
			}
			// 四角形の処理（四角形の2つの三角形に分割）
			else if (face.mNumIndices == 4)
			{
				// 四角形の1つ目の三角形(0,1,2)
				meshData.size.indices++;
				meshData.size.indices++;
				meshData.size.indices++;

				// 四角形の2つ目の三角形(0,1,2)
				meshData.size.indices++;
				meshData.size.indices++;
				meshData.size.indices++;
			}
			// サポート外のポリゴン数の場合のエラーチェック
			else
			{
				assert(false && "Unsupported polygon type");
			}
		}

		// VBVのSRVリソース作成
		meshData.srvVBVIndex = rvManager_->GetNewHandle();
		rvManager_->CreateSRVResource(
			meshData.srvVBVIndex,
			sizeof(VertexData) * meshData.size.vertices
		);

		rvManager_->CreateSRVforStructuredBuffer(
			meshData.srvVBVIndex,
			static_cast<UINT>(meshData.size.vertices),
			static_cast<UINT>(sizeof(VertexData))
		);

		// IBVのSRVリソース作成
		meshData.srvIBVIndex = rvManager_->GetNewHandle();
		rvManager_->CreateSRVResource(
			meshData.srvIBVIndex,
			sizeof(uint32_t) * meshData.size.indices
		);

		rvManager_->CreateSRVforStructuredBuffer(
			meshData.srvIBVIndex,
			static_cast<UINT>(meshData.size.indices),
			static_cast<UINT>(sizeof(uint32_t))
		);

		// メモリ確保
		meshData.vertices.resize(meshData.size.vertices);
		meshData.indices.resize(meshData.size.indices);

		meshes.meshData.push_back(meshData);
		if (!mesh->mNumBones)
		{
			modelData->isBone = false;
		} else
		{
			modelData->isBone = true;
		}
	}

	if (scene->mNumAnimations != 0)
	{
		meshes.meshData[0].isAnimation = true;
		LoadAnimationFile(modelData, directoryPath, entry);
		if (modelData->isBone)
		{
			if (scene->mNumMeshes > 1)
			{
				// 複数メッシュのアニメーションは対応しない
				assert(0);
			}
			CreateSkeleton(modelData, modelData->rootNode);
			uint32_t meshNum = 0;
			for (std::string& name : modelData->meshNames)
			{
				name;
				CreateSkinCluster(modelData, meshes.meshData[meshNum]);
				meshNum++;
			}
			// Resourceの作成
			modelData->meshIndex = rvManager_->CreateMeshResource(
				&meshes
			);

			meshNum = 0;
			for (std::string& name : modelData->meshNames)
			{
				modelData->objects[name].infoCBVIndex = rvManager_->CreateCBV(sizeof(SkinningInformation));
				rvManager_->GetCBVResource(modelData->objects[name].infoCBVIndex)->Map(0, nullptr, reinterpret_cast<void**>(&modelData->objects[name].infoData));
				modelData->objects[name].infoData->numVertices = static_cast<uint32_t>(meshes.meshData[meshNum].size.vertices);
				meshNum++;
			}
		}
	} else
	{
		// Resourceの作成
		modelData->meshIndex = rvManager_->CreateMeshResource(
			&meshes
		);
	}

	Meshes* mappedMeshes = rvManager_->GetMesh(modelData->meshIndex);

	// マッピング
	for (MeshData& meshData : mappedMeshes->meshData)
	{
		meshData.mappedVertices = nullptr;
		meshData.mappedIndices = nullptr;
		rvManager_->GetMeshViewData(meshData.meshViewIndex)->vbvData.resource->Map(
			0, nullptr, &meshData.mappedVertices
		);
		rvManager_->GetMeshViewData(meshData.meshViewIndex)->ibvData.resource->Map(
			0, nullptr, &meshData.mappedIndices
		);
	}

	// Meshの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());// 法線がないMeshは非対応
		std::string meshName = mesh->mName.C_Str();
		std::wstring meshName2 = ConvertString(mesh->mName.C_Str());
		modelData->objects[meshName].material.isTexture = mesh->HasTextureCoords(0);

		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
		{
			//VertexData vertex;
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			// UV座標のチェックと設定
			if (mesh->HasTextureCoords(0))
			{
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				mappedMeshes->meshData[meshIndex].vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };
			} else
			{
				mappedMeshes->meshData[meshIndex].vertices[vertexIndex].texcoord = { 0.0f,0.0f };// ダミーのUV座標
			}
			mappedMeshes->meshData[meshIndex].vertices[vertexIndex].position = { position.x,position.y,position.z,1.0f };
			mappedMeshes->meshData[meshIndex].vertices[vertexIndex].normal = { normal.x,normal.y,normal.z };
			mappedMeshes->meshData[meshIndex].vertices[vertexIndex].position.x *= -1.0f;
			mappedMeshes->meshData[meshIndex].vertices[vertexIndex].normal.x *= -1.0f;
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
					mappedMeshes->meshData[meshIndex].indices[indexCount] = vertexIndex;
					indexCount++;
				}
			}
			// 四角形の処理（四角形の2つの三角形に分割）
			else if (face.mNumIndices == 4)
			{
				// 四角形の1つ目の三角形(0,1,2)
				mappedMeshes->meshData[meshIndex].indices[indexCount] = face.mIndices[0];
				indexCount++;
				mappedMeshes->meshData[meshIndex].indices[indexCount] = face.mIndices[1];
				indexCount++;
				mappedMeshes->meshData[meshIndex].indices[indexCount] = face.mIndices[2];
				indexCount++;

				// 四角形の2つ目の三角形(0,1,2)
				mappedMeshes->meshData[meshIndex].indices[indexCount] = face.mIndices[0];
				indexCount++;
				mappedMeshes->meshData[meshIndex].indices[indexCount] = face.mIndices[2];
				indexCount++;
				mappedMeshes->meshData[meshIndex].indices[indexCount] = face.mIndices[3];
			}
			// サポート外のポリゴン数の場合のエラーチェック
			else
			{
				assert(false && "Unsupported polygon type");
			}
		}

		// コピー
		memcpy(
			mappedMeshes->meshData[meshIndex].mappedVertices,
			mappedMeshes->meshData[meshIndex].vertices.data(),
			sizeof(VertexData) * mappedMeshes->meshData[meshIndex].size.vertices
		);
		memcpy(
			mappedMeshes->meshData[meshIndex].mappedIndices,
			mappedMeshes->meshData[meshIndex].indices.data(),
			sizeof(uint32_t) * mappedMeshes->meshData[meshIndex].size.indices
		);

		// マップ解除
		rvManager_->GetMeshViewData(
			mappedMeshes->meshData[meshIndex].meshViewIndex)->vbvData.resource->
			Unmap(0, nullptr);
		rvManager_->GetMeshViewData(
			mappedMeshes->meshData[meshIndex].meshViewIndex)->ibvData.resource->
			Unmap(0, nullptr);
		mappedMeshes->meshData[meshIndex].mappedVertices = nullptr;
		mappedMeshes->meshData[meshIndex].mappedIndices = nullptr;

		/*ボーン解析*/
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			aiBone* bone = mesh->mBones[boneIndex];// AssimpではJointをBoneと呼んでいる
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData->objects[meshName].skinClusterData[jointName];

			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();// BindPoseMatrixに戻す
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);// 成分を抽出
			/*左手系のBindPoseMatrixを作る*/
			Matrix4 bindPoseMatrix = MakeAffineMatrix(
				{ scale.x,scale.y,scale.z }, { rotate.x,-rotate.y,-rotate.z,rotate.w }, { -translate.x,translate.y,translate.z });
			/*InverseBindPoseMatrixにする*/
			jointWeightData.inverseBindPoseMatrix = Matrix4::Inverse(bindPoseMatrix);

			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
			{
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
			}
		}

		CreateInfluence(modelData, mappedMeshes->meshData[meshIndex]);

		// マテリアル解析
		uint32_t materialIndex = mesh->mMaterialIndex;
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0)
		{
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			// 画像テクスチャの読み込み処理
			texLoader_->Load(textureFilePath.C_Str());

			modelData->objects[meshName].material.textureName = fs::path(textureFilePath.C_Str()).filename().string();
			modelData->objects[meshName].material.isTexture = true;
		} else
		{
			modelData->objects[meshName].material.isTexture = false;
		}
	}
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
	meshData.vertices[0] = { {0.5f,  0.5f,  0.5f, 1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }; // 右上
	meshData.vertices[1] = { {0.5f,  0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }; // 左上
	meshData.vertices[2] = { {0.5f, -0.5f,  0.5f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} }; // 右下
	meshData.vertices[3] = { {0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f} }; // 左下

	// 左面
	meshData.vertices[4] = { {-0.5f,  0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} }; // 左上
	meshData.vertices[5] = { {-0.5f,  0.5f,  0.5f, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} }; // 右上
	meshData.vertices[6] = { {-0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} }; // 左下
	meshData.vertices[7] = { {-0.5f, -0.5f,  0.5f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} }; // 右下

	// 前面
	meshData.vertices[8] = { {-0.5f,  0.5f,  0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} }; // 左上
	meshData.vertices[9] = { { 0.5f,  0.5f,  0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} }; // 右上
	meshData.vertices[10] = { {-0.5f, -0.5f,  0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} }; // 左下
	meshData.vertices[11] = { { 0.5f, -0.5f,  0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} }; // 右下

	// 後面
	meshData.vertices[12] = { { 0.5f,  0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }; // 右上
	meshData.vertices[13] = { {-0.5f,  0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }; // 左上
	meshData.vertices[14] = { { 0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} }; // 右下
	meshData.vertices[15] = { {-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} }; // 左下

	// 上面
	meshData.vertices[16] = { {-0.5f,  0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} }; // 左奥
	meshData.vertices[17] = { { 0.5f,  0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} }; // 右奥
	meshData.vertices[18] = { {-0.5f,  0.5f,  0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f} }; // 左前
	meshData.vertices[19] = { { 0.5f,  0.5f,  0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} }; // 右前

	// 下面
	meshData.vertices[20] = { {-0.5f, -0.5f,  0.5f, 1.0f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} }; // 左前
	meshData.vertices[21] = { { 0.5f, -0.5f,  0.5f, 1.0f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f} }; // 右前
	meshData.vertices[22] = { {-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f} }; // 左奥
	meshData.vertices[23] = { { 0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} }; // 右奥

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
			// B頂点
			meshData.vertices[start + 1].position.x = cos(lat + kLatEvery) * cos(lon);
			meshData.vertices[start + 1].position.y = sin(lat + kLatEvery);
			meshData.vertices[start + 1].position.z = cos(lat + kLatEvery) * sin(lon);
			meshData.vertices[start + 1].position.w = 1.0f;
			meshData.vertices[start + 1].texCoord = { float(lonIndex) / float(kSubdivision),1.0f - float(latIndex + 1) / float(kSubdivision) };
			meshData.vertices[start + 1].normal.x = meshData.vertices[start + 1].position.x;
			meshData.vertices[start + 1].normal.y = meshData.vertices[start + 1].position.y;
			meshData.vertices[start + 1].normal.z = meshData.vertices[start + 1].position.z;
			// C頂点
			meshData.vertices[start + 2].position.x = cos(lat) * cos(lon + kLonEvery);
			meshData.vertices[start + 2].position.y = sin(lat);
			meshData.vertices[start + 2].position.z = cos(lat) * sin(lon + kLonEvery);
			meshData.vertices[start + 2].position.w = 1.0f;
			meshData.vertices[start + 2].texCoord = { float(lonIndex + 1) / float(kSubdivision),1.0f - float(latIndex) / float(kSubdivision) };
			meshData.vertices[start + 2].normal.x = meshData.vertices[start + 2].position.x;
			meshData.vertices[start + 2].normal.y = meshData.vertices[start + 2].position.y;
			meshData.vertices[start + 2].normal.z = meshData.vertices[start + 2].position.z;
			// D頂点
			meshData.vertices[start + 3].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery);
			meshData.vertices[start + 3].position.y = sin(lat + kLatEvery);
			meshData.vertices[start + 3].position.z = cos(lat + kLatEvery) * sin(lon + kLonEvery);
			meshData.vertices[start + 3].position.w = 1.0f;
			meshData.vertices[start + 3].texCoord = { float(lonIndex + 1) / float(kSubdivision),1.0f - float(latIndex + 1) / float(kSubdivision) };
			meshData.vertices[start + 3].normal.x = meshData.vertices[start + 3].position.x;
			meshData.vertices[start + 3].normal.y = meshData.vertices[start + 3].position.y;
			meshData.vertices[start + 3].normal.z = meshData.vertices[start + 3].position.z;

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
	meshData.vertices[0] = { { 1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };  // 右上
	meshData.vertices[1] = { {-1.0f,  1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };  // 左上
	meshData.vertices[2] = { { 1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } };  // 右下
	meshData.vertices[3] = { {-1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } };  // 左下
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
