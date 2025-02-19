#pragma once

// 数学ライブラリ
#include"ChoMath.h"

#include"ConstantData/WorldTransform.h"

// C++
#include<cstdint>
#include<vector>
#include<map>
#include<string>
#include<optional>
#include<span>

const uint32_t kNumMaxInfluence = 4;

template<typename T>
struct Keyframe {
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
struct NodeAnimation {
	AnimationCurve<Scale> scale;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> translate;
};

// ジョイント
struct Joint {
	NodeTransform transform;// Transform情報
	Matrix4 localMatrix;
	Matrix4 skeletonSpaceMatrix;// skeletonSpaceでの変換行列
	std::string name;// 名前
	std::vector<int32_t> children;// 子JointのIndexのリスト。いなければ空
	int32_t index = 0;// 自身のIndex
	std::optional<int32_t> parent;// 親JointのIndex。いなければnull
};

// スケルトン
struct Skeleton {
	int32_t root=0;// RootJointのIndex
	std::map<std::string, int32_t> jointMap;// Joint名とIndexとの辞書
	std::vector<Joint> joints;// 所属しているJoint
	Skeleton() = default;
};

// ウェイトデータ
struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;
};
struct JointWeightData {
	Matrix4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};

// skinning
struct SkinningInformation {
	uint32_t numVertices;
};


struct ConstBufferDataVertexInfluence {
	std::array<float, kNumMaxInfluence>weights;
	std::array<int32_t, kNumMaxInfluence>jointIndices;
};
struct InfluenceData {
	std::span<ConstBufferDataVertexInfluence> map;
	uint32_t srvIndex = 0;
	uint32_t meshViewIndex = 0;
};
struct ConstBufferDataWell {
	Matrix4 skeletonSpaceMatrix;// 位置用
	Matrix4 skeletonSpaceInverseTransposeMatrix;// 法線用
};
struct PaletteData {
	std::span<ConstBufferDataWell> map;
	uint32_t srvIndex = 0;
};
struct SkinningData {
	uint32_t outputMVIndex = 0;
	//uint32_t inputMVIndex = 0;
	uint32_t outputUAVIndex = 0;
	//uint32_t inputSRVIndex = 0;
};
struct SkinCluster {
	std::vector<Matrix4> inverseBindPoseMatrices;
	PaletteData paletteData;
	InfluenceData influenceData;
	SkinningData skinningData;
};

// アニメーションデータ構造体
struct AnimationData {
	float duration = 0.0f;// アニメーション全体の尺（単位は秒）
	uint32_t allFrame = 0;// アニメーション全体のフレーム数
	std::map<std::string, NodeAnimation> nodeAnimations;
	AnimationData() = default;
};