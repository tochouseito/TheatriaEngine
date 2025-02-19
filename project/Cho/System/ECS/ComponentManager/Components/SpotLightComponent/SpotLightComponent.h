#pragma once
#include "Vector3.h"
#include "ConstantData/LightData.h"

struct SpotLightComponent {
	Vector3 color;    //!<ライトの色
	float intensity;  //!<ライトの強さ
	Vector3 direction;//!<ライトの向き
	float distance;//!<ライトの届く最大距離
	Vector3 position; //!<ライトの位置
	float decay;//!<減衰率
	float cosAngle;//!<スポットライトの余弦
	float cosFalloffStart;//Falloff開始の角度
	uint32_t active;  //!<ライトの有効無効

	uint32_t index = 0; //!<ライトのインデックス
};