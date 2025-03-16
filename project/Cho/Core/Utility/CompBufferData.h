#pragma once
#include "ChoMath.h"
// 定数バッファ用データ構造体
struct BUFFER_DATA_TF final
{
    Matrix4 matWorld; // ローカル → ワールド変換行列
    Matrix4 worldInverse;
    Matrix4 rootNode;// モデルのRootMatrix
};