#pragma once
#include"ChoMath.h"
// 定数バッファ用データ構造体
struct ConstBufferDataViewProjection final {
    Matrix4 view;
    Matrix4 projection;
    Matrix4 projectionInverse;
    Matrix4 matWorld;
    Matrix4 matBillboard;
    Vector3 cameraPosition;
};