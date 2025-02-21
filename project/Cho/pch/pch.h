#pragma once

#define NOMINMAX // Windowのminmaxマクロを除外
//#define WIN32_LEAN_AND_MEAN // ヘッダーから余計な情報を除外

#ifdef NDEBUG
#include <cassert>
#endif // NDEBUG


#include "ChoMath.h"
using namespace ChoMath;