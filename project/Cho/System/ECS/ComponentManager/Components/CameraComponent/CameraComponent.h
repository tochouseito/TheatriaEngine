#pragma once
#include"ConstantData/ViewProjection.h"
#include"ChoMath.h"
#include"SystemState/SystemState.h"

struct CameraComponent final{
    Vector3 translation = {0.0f,0.0f,-30.0f}; // 座標
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

    ConstBufferDataViewProjection* constData = nullptr;
    uint32_t cbvIndex=0;
};