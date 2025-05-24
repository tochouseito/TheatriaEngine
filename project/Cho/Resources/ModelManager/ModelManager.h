#pragma once
#include "ChoMath.h"
#include "Core/Utility/FVector.h"
#include "SDK/DirectX/DirectX12/stdafx/stdafx.h"
#include "Core/Utility/Components.h"
#include <unordered_map>
#include <filesystem>
#include <list>
#include <map>
// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/version.h>
// 頂点データ構造体
struct VertexData
{
	Vector4 position = { 0.0f };
	Vector2 texCoord = { 0.0f };
	Vector3 normal = { 0.0f };
	Color color = { 1.0f,1.0f,1.0f,1.0f };
	uint32_t vertexID = 0;
};
struct Node
{
	NodeTransform transform;
	Matrix4 localMatrix;
	std::string name;
	std::vector<Node> children;
};
struct MaterialData
{
	Color color = { 1.0f,1.0f,1.0f,1.0f };
	std::string diffuseTexPath = "";
};
const uint32_t kNumMaxInfluence = 4;

template<typename T>
struct Keyframe
{
	float time = 0.0f;
	T value;
};
using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;
using KeyframeScale = Keyframe<Scale>;

template<typename T>
struct AnimationCurve
{
	std::vector<Keyframe<T>> keyframes;
};
struct NodeAnimation
{
	AnimationCurve<Scale> scale;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> translate;
};

// ジョイント
struct Joint
{
	NodeTransform transform;// Transform情報
	Matrix4 localMatrix;
	Matrix4 skeletonSpaceMatrix;// skeletonSpaceでの変換行列
	std::string name;// 名前
	std::vector<int32_t> children;// 子JointのIndexのリスト。いなければ空
	int32_t index = 0;// 自身のIndex
	std::optional<int32_t> parent;// 親JointのIndex。いなければnull
};

// スケルトン
struct Skeleton
{
	int32_t root = 0;// RootJointのIndex
	std::map<std::string, int32_t> jointMap;// Joint名とIndexとの辞書
	std::vector<Joint> joints;// 所属しているJoint
};

// ウェイトデータ
struct VertexWeightData
{
	float weight;
	uint32_t vertexIndex;
};
struct JointWeightData
{
	Matrix4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};
// skinning
struct SkinningInformation
{
	uint32_t numVertices;
};
struct ConstBufferDataVertexInfluence
{
	std::array<float, kNumMaxInfluence>weights;
	std::array<int32_t, kNumMaxInfluence>jointIndices;
};
struct InfluenceData
{
	std::vector<ConstBufferDataVertexInfluence> data;
	//uint32_t bufferIndex = 0;
};
struct ConstBufferDataWell
{
	Matrix4 skeletonSpaceMatrix;// 位置用
	Matrix4 skeletonSpaceInverseTransposeMatrix;// 法線用
};
struct PaletteData
{
	std::vector<ConstBufferDataWell> data;
	//uint32_t bufferIndex = 0;
};
struct SkinCluster
{
	std::vector<Matrix4> inverseBindPoseMatrices;
	PaletteData paletteData;
	InfluenceData influenceData;
	//uint32_t skinningBufferIndex = 0;
};

// アニメーションデータ構造体
struct AnimationData
{
	float duration = 0.0f;// アニメーション全体の尺（単位は秒）
	uint32_t allFrame = 0;// アニメーション全体のフレーム数
	std::map<std::string, NodeAnimation> nodeAnimations;
};
struct MeshData
{
	std::wstring name;
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	std::vector<MaterialData> materials;
	std::map<std::string, JointWeightData>skinClusterData;
	SkinningInformation* infoData = nullptr;
	std::optional<uint32_t> vertexBufferIndex = std::nullopt;
	std::optional<uint32_t> indexBufferIndex = std::nullopt;
};
struct ModelData
{
	std::wstring name;
	std::vector<MeshData> meshes;
	Node rootNode;
	// このモデルを使用しているTransformのインデックス
	std::list<uint32_t> useTransformList;
	// このモデルを使用しているTFリストのバッファインデックス
	std::optional<uint32_t> useTransformBufferIndex = std::nullopt;
	bool isBone = false;
	std::vector<AnimationData> animations;
	Skeleton skeleton;
	SkinCluster skinCluster;
};

class ResourceManager;
class ModelManager
{
public:
	ModelManager(ResourceManager* resourceManager)
		:m_pResourceManager(resourceManager)
	{
		// Assimpのオプションチェック
		CheckAssimpOption();
		CreateDefaultMesh();
	}
	~ModelManager()
	{

	}
	// モデルファイルを読み込む
	bool LoadModelFile(const std::filesystem::path& filePath);

	// 名前コンテナを取得する
	std::unordered_map<std::wstring, uint32_t>& GetModelNameContainer() { return m_ModelNameContainer; }
	// モデルデータコンテナを取得する
	std::vector<ModelData>& GetModelDataContainer() { return m_Models.GetVector(); }
	// 名前で検索してインデックスを取得する
	std::optional<uint32_t> GetModelDataIndex(const std::wstring& name);
	// モデルデータを取得する
	ModelData* GetModelData(const uint32_t& index) { return &m_Models[index]; }
	ModelData* GetModelData(const std::wstring& name)
	{
		if (m_ModelNameContainer.contains(name))
		{
			return &m_Models[m_ModelNameContainer[name]];
		}
		return nullptr;
	}
	// モデルのUseListに登録する
	void RegisterModelUseList(const std::variant<uint32_t,std::wstring>& key, const uint32_t& transformMapID);
	// モデルのUseListから削除する
	void RemoveModelUseList(const std::variant<uint32_t, std::wstring>& key, const uint32_t& transformMapID);
private:
	// Assimpのオプションチェック
	void CheckAssimpOption();
	// デフォルトメッシュの生成
	void CreateDefaultMesh();
	// Cubeの生成
	void CreateCube();
	// Sphereの生成
	void CreateSphere();
	// Planeの生成
	void CreatePlane();
	// Ringの生成
	void CreateRing();
	// Cylinderの生成
	void CreateCylinder();

	// 
	Node ReadNode(aiNode* node);
	// ジョイントの生成
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
	// モデルコンテナの要素数を取得する
	//uint32_t GetModelDataSize() { return static_cast<uint32_t>(m_Models.size()); }
	// ModelDataの追加
	void AddModelData(ModelData& modelData);

	ResourceManager* m_pResourceManager = nullptr;
	// モデルデータコンテナ
	FVector<ModelData> m_Models;
	// モデルのキーを名前で管理するコンテナ
	std::unordered_map<std::wstring, uint32_t> m_ModelNameContainer;

	// モデルの使用可能なTransformの数のオフセット
	static const uint32_t kUseTransformOffset = 100;
};

