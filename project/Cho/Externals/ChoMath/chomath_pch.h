// pch.h: プリコンパイル済みヘッダー ファイルです。
// 次のファイルは、その後のビルドのビルド パフォーマンスを向上させるため 1 回だけコンパイルされます。
// コード補完や多くのコード参照機能などの IntelliSense パフォーマンスにも影響します。
// ただし、ここに一覧表示されているファイルは、ビルド間でいずれかが更新されると、すべてが再コンパイルされます。
// 頻繁に更新するファイルをここに追加しないでください。追加すると、パフォーマンス上の利点がなくなります。

#ifndef CHOMATH_PCH_H
#define CHOMATH_PCH_H

// プリコンパイルするヘッダーをここに追加します
#define WIN32_LEAN_AND_MEAN					// Windows ヘッダーからほとんど使用されていない部分を除外する
#define NOMINMAX							// min / max マクロの定義を抑止（std::min 等との衝突を回避）
#define _WINSOCK_DEPRECATED_NO_WARNINGS		// 古いWinsock関数の警告抑制（inet_addrなど）
#define _CRT_SECURE_NO_WARNINGS				// strcpy, fopenなどの警告抑制
#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING  // C++17関連の警告抑制
#define _USE_MATH_DEFINES					// M_PIなどの数学定数を使えるようにする

// ------------------------
// Windows API
// ------------------------
#include <windows.h>

// ------------------------
// C++ / 標準ライブラリ
// ------------------------
#include <cmath>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <numbers>
#include <array>

// ------------------------
// SIMD用(予定)
// ------------------------

// ------------------------
// マルチスレッド用(予定)
// ------------------------
// #include <thread>
// #include <mutex>
// #include <condition_variable>
// #include <atomic>

// ------------------------
// デバッグ
// ------------------------
#ifdef _DEBUG
#define ENABLE_ASSERT
#include <cassert>
#define DEBUG_LOG(x) std::cout << "[DEBUG] " << x << std::endl;
#include <crtdbg.h>        // メモリリーク検出 
#endif // _DEBUG

// ------------------------
// リリース
// ------------------------

#ifdef NDEBUG
    // リリースビルドでは無効化
#define assert(x) ((void)0)
#define DEBUG_LOG(x) ((void)0)
#endif // NDEBUG

// ------------------------
// DLLExportMacro
// ------------------------
#ifdef CHOMATH_EXPORTS
#define CHOMATH_API __declspec(dllexport)
//#define REGISTER_SCRIPT_FACTORY(SCRIPTNAME) \
//    extern "C" __declspec(dllexport) Marionnette* Create##SCRIPTNAME##Script(GameObject& object) { \
//        return new SCRIPTNAME(object);}
#else
#define CHOMATH_API __declspec(dllimport)
#endif

#include"Vector2.h"
#include"Vector3.h"
#include"Scale.h"
#include"Vector4.h"
#include"Quaternion.h"
#include"Matrix4.h"
#include"mathShapes.h"

namespace chomath
{
	constexpr float PiF = std::numbers::pi_v<float>; // float型のπを定数として定義
	const uint32_t MatCount = 4; //逆行列を求める行列の行数・列数
	enum class RotationOrder
	{
		XYZ,
		YXZ,
		ZXY,
		ZYX,
		YZX,
		XZY
	};

}

#endif // CHOMATH_PCH_H
