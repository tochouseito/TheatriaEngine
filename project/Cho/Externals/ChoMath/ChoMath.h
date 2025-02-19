#pragma once
// ChoMath.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "pch.h"
#include "framework.h"

namespace ChoMath {

	float Pi();

	// 行列のチェック関数

	int Check(double mat[MatCount][MatCount], double inv[MatCount][MatCount]);


	float Clamp(float x, float min, float max);


	Matrix4 Transpose(const Matrix4& m);

	Vector3 TransformNormal(const Vector3& v, const Matrix4& m);

	Vector3 Transform(const Vector3& vector, const Matrix4& matrix);

	Matrix4 MakeIdentity4x4();


	Matrix4 Multiply(const Matrix4& m1, const Matrix4& m2);

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

	float LerpShortAngle(float startAngle, float endAngle, float t);
};