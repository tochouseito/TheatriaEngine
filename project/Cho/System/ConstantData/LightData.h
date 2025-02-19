#pragma once
#include "Color.h"
#include "Vector3.h"
#include <array>

struct DirectionalLightData
{
	Vector3 color;    //!<ライトの色
	float intensity;  //!<ライトの強さ
	Vector3 direction;//!<ライトの向き
	uint32_t active;  //!<ライトの有効無効
};

struct PointLightData
{
	Vector3 color;    //!<ライトの色
	float intensity;  //!<ライトの強さ
	Vector3 position;//!<ライトの位置
	float radius;//!<ライトの届く最大距離
	float decay;//!<減衰率
	uint32_t active;  //!<ライトの有効無効
	float pad1[2];
};

struct SpotLightData
{
	Vector3 color;    //!<ライトの色
	float intensity;  //!<ライトの強さ
	Vector3 direction;//!<ライトの向き
	float distance;//!<ライトの届く最大距離
	Vector3 position; //!<ライトの位置
	float decay;//!<減衰率
	float cosAngle;//!<スポットライトの余弦
	float cosFalloffStart;//Falloff開始の角度
	uint32_t active;  //!<ライトの有効無効
	float pad1;
};

// 平行光源の数
static const int kDirLightNum = 10;
// 点光源の数
static const int kPointLightNum = 10;
// スポットライトの数
static const int kSpotLightNum = 10;

struct PunctualLightData {
	// 環境光
	Vector3 ambientColor;
	float pad1;
	// 平行光源
	std::array<DirectionalLightData, kDirLightNum> dirLights;
	// 点光源
	std::array<PointLightData, kPointLightNum> pointLights;
	// スポットライト
	std::array<SpotLightData, kSpotLightNum> spotLights;
};