#pragma once
#include"Color.h"
#include"ChoMath.h"

struct ConstBufferDataMaterial {
	Color color;
	int enableLighting;
	float padding1[3];
	Matrix4 matUV;
	float shininess;
	float padding2[12];    // 追加: 256 バイトアライメントを確保
};