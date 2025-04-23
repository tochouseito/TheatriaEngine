#pragma once
#include "ChoMath.h"
#include "Core/Utility/Color.h"
// 定数バッファ用データ構造体
// Transform
struct BUFFER_DATA_TF final
{
	// ローカル → ワールド変換行列
	Matrix4 matWorld;		// 64バイト
    Matrix4 worldInverse;	// 64バイト
	// モデルのRootMatrix
    Matrix4 rootMatrix;		// 64バイト
};
// ViewProjection
struct BUFFER_DATA_VIEWPROJECTION final
{
    Matrix4 view;				// 64バイト
    Matrix4 projection;			// 64バイト
    Matrix4 projectionInverse;	// 64バイト
    Matrix4 matWorld;			// 64バイト
    Matrix4 matBillboard;		// 64バイト
	Vector3 cameraPosition;		// 12バイト
	float padding1;				// 4バイト
};
// Line
struct BUFFER_DATA_LINE final
{
	Vector3 position;	// 12バイト
	float padding1;		// 4バイト
	Color color;		// 16バイト
};
// Material
struct BUFFER_DATA_MATERIAL final
{
	Color color;		// 16バイト
	int enableLighting;	// 4バイト
	int enableTexture;	// 4バイト
	int textureId;		// 4バイト
	float padding1[1];	// 4バイト
	Matrix4 matUV;		// 64バイト
	float shininess;	// 4バイト
	float padding2[3];	// 12バイト
};