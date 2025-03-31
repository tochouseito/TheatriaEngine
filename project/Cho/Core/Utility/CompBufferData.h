#pragma once
#include "ChoMath.h"
// 定数バッファ用データ構造体
// Transform
struct BUFFER_DATA_TF final
{
    Matrix4 matWorld; // ローカル → ワールド変換行列
    Matrix4 worldInverse;
    Matrix4 rootMatrix;// モデルのRootMatrix
};
// ViewProjection
struct BUFFER_DATA_VIEWPROJECTION final
{
    Matrix4 view;
    Matrix4 projection;
    Matrix4 projectionInverse;
    Matrix4 matWorld;
    Matrix4 matBillboard;
    Vector3 cameraPosition;
};