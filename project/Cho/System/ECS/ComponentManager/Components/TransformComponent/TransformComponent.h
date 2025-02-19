#pragma once
#include "ConstantData/WorldTransform.h"
#include "ChoMath.h"
#include <vector>
// Transform

struct Transforms {
	Vector3 translation = { 0.0f, 0.0f, 0.0f };// 位置
	Quaternion rotation = { 0.0f, 0.0f, 0.0f,1.0f }; // 回転
	Scale scale = { 1.0f, 1.0f, 1.0f }; // スケール

	// 度数表示,操作用変数
	Vector3 degrees = { 0.0f,0.0f,0.0f };// 度数表示

	// ワールド変換行列
	Matrix4 matWorld = ChoMath::MakeIdentity4x4();
	// ワールド逆行列
	Matrix4 worldInverse = ChoMath::MakeIdentity4x4();
	// ルートノード
	Matrix4 rootNode = ChoMath::MakeIdentity4x4();
};

struct TransformComponent final {
	Vector3 translation = { 0.0f, 0.0f, 0.0f };// 位置
	Quaternion rotation = { 0.0f, 0.0f, 0.0f,1.0f }; // 回転
	Scale scale = { 1.0f, 1.0f, 1.0f }; // スケール

	Matrix4 matWorld = ChoMath::MakeIdentity4x4();// ワールド行列
	Matrix4* parent = nullptr; // 親のワールド行列
	Matrix4 rootMatrix = ChoMath::MakeIdentity4x4(); // ルートのワールド行列

	// 度数表示,操作用変数
	Vector3 degrees = { 0.0f,0.0f,0.0f };// 度数表示

	// 差分計算用
	Vector3 prePos = { 0.0f,0.0f,0.0f };
	Vector3 preRot = { 0.0f,0.0f,0.0f };
	Scale preScale = { 1.0f,1.0f,1.0f };
	Matrix4 preParent = ChoMath::MakeIdentity4x4();

	ConstBufferDataWorldTransform* constData = nullptr;

	uint32_t cbvIndex = 0;

	// インスタンシング描画用
	bool isInstance = false;
	uint32_t srvIndex = 0;
	uint32_t instanceCount = 0;
	std::vector<Transforms> transforms;
};