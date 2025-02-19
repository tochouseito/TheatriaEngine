#pragma once
#include "Vector3.h"
#include "ConstantData/LightData.h"

struct PointLightComponent {
	Vector3 color;    //!<ライトの色
	float intensity;  //!<ライトの強さ
	Vector3 position;//!<ライトの位置
	float radius;//!<ライトの届く最大距離
	float decay;//!<減衰率
	uint32_t active;  //!<ライトの有効無効

	uint32_t index = 0; //!<ライトのインデックス
};