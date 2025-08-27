#pragma once
// ChoMath.cpp : スタティック ライブラリ用の関数を定義します。
//

/*
行ベクトル　row-vector
行優先 row-major
*/

#include "chomath_pch.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Scale.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix4.h"
#include "mathShapes.h"

namespace chomath {

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


	// SRT をまとめた構造体
	struct SRT
	{
		Vector3 translation;
		Vector3 rotationEuler; // ラジアン or 度数法 に合わせる
		Scale scale;
	};

	float Pi();

	// 行列のチェック関数

	int Check(double mat[MatCount][MatCount], double inv[MatCount][MatCount]);


	float Clamp(float x, float min, float max);


	Matrix4 Transpose(const Matrix4& m);

	Vector3 TransformNormal(const Vector3& v, const Matrix4& m);

	Vector3 Transform(const Vector3& vector, const Matrix4& matrix);
	bool Transform(const Vector3& in, const Matrix4& matrix, Vector3& out);

	Matrix4 MakeIdentity4x4();


	Matrix4 Multiply(const Matrix4& m1, const Matrix4& m2);

	// 行列とベクトルの掛け算
	Vector4 Mul(const Matrix4& mat, const Vector3& v, float w = 1.0f);

	Matrix4 MakeTranslateMatrix(const Vector3& translate);


	Matrix4 MakeScaleMatrix(const Scale& scale);

	Matrix4 MakeScaleMatrix(const Vector3& scale);

	Matrix4 MakeRotateXMatrix(float radian);

	Matrix4 MakeRotateYMatrix(float radian);

	Matrix4 MakeRotateZMatrix(float radian);

	Matrix4 MakeRotateXYZMatrix(const Vector3& rotate);

	// ビューポート変換行列
	Matrix4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

	Matrix4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

	// 正射影行列
	Matrix4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

	//// ベクトルの正規化
	float Normalize(float x, float min, float max);

	float Dot(const Vector3& v1, const Vector3& v2);

	Matrix4 MakeRotateAxisAngle(const Vector3& axis, float angle);

	Matrix4 DirectionToDirection(const Vector3& from, const Vector3& to);

	// 度数からラジアンに変換
	float DegreesToRadians(const float& degrees);
	Vector3 DegreesToRadians(const Vector3& degrees);

	// ラジアンから度数に変換
	float RadiansToDegrees(const float& radians);
	Vector3 RadiansToDegrees(const Vector3& radians);

	Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);
	Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);
	Matrix4 MakeRotateMatrix(const Quaternion& quaternion);
	float Dot(const Quaternion& q0, const Quaternion& q1);
	// オイラー角からクォータニオンを生成

	Quaternion FromEulerAngles(const Vector3& eulerAngles);

	// クォータニオンからオイラー角を計算（回転順序を指定）

	Vector3 ToEulerAngles(const Quaternion& q, RotationOrder order);
	Matrix4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	Matrix4 MakeAffineMatrix(const Scale& scale, const Quaternion& rotate, const Vector3& translate);

	Vector3 TransformDirection(const Vector3& v,const Matrix4& m);

	float LerpShortAngle(float startAngle, float endAngle, float t);

	Quaternion MakeLookRotation(const Vector3& forward, const Vector3& up);

	Vector3 GetForwardVectorFromMatrix(const Matrix4& rotMatrix);

	// Matrix4 から SRT を取り出す関数
	SRT DecomposeMatrix(const Matrix4& in);

	float Lerp(const float& start, const float& end, const float& t);

	Quaternion MakeQuaternionRotation(const Vector3& rad, const Vector3& preRad, const Quaternion& quaternion);

	Quaternion MakeEulerRotation(const Vector3& rad);

	// ALLBillboard
	Matrix4 BillboardMatrix(const Matrix4 cameraMatrix);

	// 3D空間から2D空間への変換
	Vector2 WorldToScreen(const Vector3& worldPos, const Matrix4& viewMatrix, const Matrix4& projMatrix, const uint32_t& screenWidth, const uint32_t& screenHeight);
};