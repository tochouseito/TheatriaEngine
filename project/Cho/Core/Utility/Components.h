#pragma once
#include "ChoMath.h"
struct TransformComponent final
{
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

	//ConstBufferDataWorldTransform* constData = nullptr;

	uint32_t cbvIndex = 0;

	// インスタンシング描画用
	/*bool isInstance = false;
	uint32_t srvIndex = 0;
	uint32_t instanceCount = 0;
	std::vector<Transforms> transforms;*/
};
// Node用Transform構造体
struct NodeTransform
{
    Vector3 translation = { 0.0f, 0.0f, 0.0f };
    Quaternion rotation = { 0.0f, 0.0f, 0.0f,1.0f };
    Scale scale = { 1.0f, 1.0f, 1.0f };
};

struct CameraComponent final
{
    Vector3 translation = { 0.0f,0.0f,-30.0f }; // 座標
    Quaternion rotation = { 0.0f,0.0f,0.0f,1.0f };// 回転

    // 度数表示,操作用変数
    Vector3 degrees = { 0.0f,0.0f,0.0f };

    // 差分計算用
    Vector3 prePos = { 0.0f,0.0f,0.0f };
    Vector3 preRot = { 0.0f,0.0f,0.0f };

    Matrix4 matWorld = ChoMath::MakeIdentity4x4(); // ワールド行列
    Matrix4* parent = nullptr;

    // 垂直方向視野角
    float fovAngleY = 45.0f * std::numbers::pi_v<float> / 180.0f;
    // ビューポートのアスペクト比
    float aspectRatio = 16.0f / 9.0f;
    // 深度限界（手前側）
    float nearZ = 0.1f;
    // 深度限界（奥側）
    float farZ = 1000.0f;

    //ConstBufferDataViewProjection* constData = nullptr;
    uint32_t cbvIndex = 0;
};