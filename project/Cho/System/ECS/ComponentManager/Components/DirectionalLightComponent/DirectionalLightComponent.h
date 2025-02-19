#pragma once
#include "Vector3.h"
#include "ConstantData/LightData.h"

struct DirectionalLightComponent {
	Vector3 color;    //!<ライトの色
	float intensity;  //!<ライトの強さ
	Vector3 direction;//!<ライトの向き
	uint32_t active;  //!<ライトの有効無効

	uint32_t index = 0; //!<ライトのインデックス
};