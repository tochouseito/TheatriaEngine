#pragma once

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する

//C++
#include<cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <algorithm>
#include <numbers>
#include <array>

#ifdef _DEBUG

#include <assert.h>

#endif // _DEBUG

constexpr float PiF = std::numbers::pi_v<float>; // float型のπを定数として定義

enum class RotationOrder {
	XYZ,
	YXZ,
	ZXY,
	ZYX,
	YZX,
	XZY
};

const uint32_t MatCount = 4; //逆行列を求める行列の行数・列数