// ChoMath.cpp : スタティック ライブラリ用の関数を定義します。
//

#include "chomath_pch.h"
#include "chomath.h"
using namespace chomath;

// 行列のチェック関数

float chomath::Pi()
{
	return std::numbers::pi_v<float>;
}

int chomath::Check(double mat[MatCount][MatCount], double inv[MatCount][MatCount])
{
	double inner_product;
	int i, j, k;
	double ans;
	double diff;
	for (i = 0; i < MatCount; i++) {
		for (j = 0; j < MatCount; j++) {
			inner_product = 0;
			for (k = 0; k < MatCount; k++) {
				inner_product += mat[i][k] * inv[k][j];
			}

			ans = (i == j) ? 1 : 0;
			diff = ans - inner_product;


		}
	}

	return 1;
}

float chomath::Clamp(float x, float min, float max)
{
	return (x < min) ? min : (x > max) ? max : x;
}

Matrix4 chomath::Transpose(const Matrix4& m)
{
	Matrix4 result = { 0 };
	for (int i = 0; i < MatCount; ++i) {
		for (int j = 0; j < MatCount; ++j) {
			result.m[j][i] = m.m[i][j];
		}
	}
	return result;
}

Vector3 chomath::TransformNormal(const Vector3& v, const Matrix4& m) {
	Vector3 result{
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]
	};
	return result;
}

Vector3 chomath::Transform(const Vector3& vector, const Matrix4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

Matrix4 chomath::MakeIdentity4x4()
{
	Matrix4 result = { 0 };
	for (int i = 0; i < MatCount; ++i) {
		result.m[i][i] = 1.0;
	}
	return result;
}

Matrix4 chomath::Multiply(const Matrix4& m1, const Matrix4& m2)
{
	Matrix4 result = { 0 };
	for (int i = 0; i < MatCount; ++i) {
		for (int j = 0; j < MatCount; ++j) {
			for (int k = 0; k < MatCount; ++k) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}

Matrix4 chomath::MakeTranslateMatrix(const Vector3& translate)
{
	Matrix4 translationMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		translate.x,translate.y, translate.z, 1.0f
	};
	return translationMatrix;
}

Matrix4 chomath::MakeScaleMatrix(const Scale& scale)
{
	Matrix4 scaleMatrix = {
		scale.x, 0.0f,    0.0f,    0.0f,
		0.0f,    scale.y, 0.0f,    0.0f,
		0.0f,    0.0f,    scale.z, 0.0f,
		0.0f,    0.0f,    0.0f,    1.0f
	};
	return scaleMatrix;
}

Matrix4 chomath::MakeScaleMatrix(const Vector3& scale)
{
	Matrix4 scaleMatrix = {
		scale.x, 0.0f,    0.0f,    0.0f,
		0.0f,    scale.y, 0.0f,    0.0f,
		0.0f,    0.0f,    scale.z, 0.0f,
		0.0f,    0.0f,    0.0f,    1.0f
	};
	return scaleMatrix;
}

Matrix4 chomath::MakeRotateXMatrix(float radian)
{
	float cosX = cosf(radian);
	float sinX = sinf(radian);
	Matrix4 result = {
		1.0f, 0.0f,  0.0f,   0.0f,
		0.0f, cosX,  sinX,   0.0f,
		0.0f, -sinX, cosX,   0.0f,
		0.0f, 0.0f,  0.0f,   1.0f
	};
	return result;
}

Matrix4 chomath::MakeRotateYMatrix(float radian)
{
	float cosY = cosf(radian);
	float sinY = sinf(radian);
	Matrix4 result = {
		cosY,  0.0f, -sinY,  0.0f,
		0.0f,  1.0f, 0.0f,   0.0f,
		sinY,  0.0f, cosY,   0.0f,
		0.0f,  0.0f, 0.0f,   1.0f
	};
	return result;
}

Matrix4 chomath::MakeRotateZMatrix(float radian)
{
	float cosZ = cosf(radian);
	float sinZ = sinf(radian);
	Matrix4 result = {
		cosZ,  sinZ,  0.0f,  0.0f,
		-sinZ, cosZ,  0.0f,  0.0f,
		0.0f,  0.0f,  1.0f,  0.0f,
		0.0f,  0.0f,  0.0f,  1.0f
	};
	return result;
}

Matrix4 chomath::MakeRotateXYZMatrix(const Vector3& rotate)
{
	Matrix4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4 rotateXYZMatrix = Multiply(Multiply(rotateZMatrix, rotateYMatrix), rotateXMatrix);

	return rotateXYZMatrix;
}


// ビューポート変換行列
Matrix4 chomath::MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4 result;
	result.m[0][0] = width / 2;
	result.m[1][0] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[3][0] = left + (width / 2);
	result.m[0][1] = 0.0f;
	result.m[1][1] = -(height / 2);
	result.m[2][1] = 0.0f;
	result.m[3][1] = top + (height / 2);
	result.m[0][2] = 0.0f;
	result.m[1][2] = 0.0f;
	result.m[2][2] = maxDepth - minDepth;
	result.m[3][2] = minDepth;
	result.m[0][3] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][3] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4 chomath::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
{
	float tanHalfFovY = std::tanf(fovY / 2.0f);
	return Matrix4{
		1.0f / (aspectRatio * tanHalfFovY), 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f / tanHalfFovY, 0.0f, 0.0f,
		0.0f, 0.0f, (farClip + nearClip) / (farClip - nearClip), 1.0f,
		0.0f, 0.0f, -(2.0f * farClip * nearClip) / (farClip - nearClip), 0.0f
	};
}

// 正射影行列
Matrix4 chomath::MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4 result;
	result.m[0][0] = 2 / (right - left);
	result.m[1][0] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[3][0] = (left + right) / (left - right);
	result.m[0][1] = 0.0f;
	result.m[1][1] = 2 / (top - bottom);
	result.m[2][1] = 0.0f;
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[0][2] = 0.0f;
	result.m[1][2] = 0.0f;
	result.m[2][2] = 1 / (farClip - nearClip);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[0][3] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][3] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}

//// ベクトルの正規化
float chomath::Normalize(float x, float min, float max)
{
	if (max - min == 0)
	{
		return 0.0f;
	}
	float normalized = (x - min) / (max - min);
	return Clamp(normalized, 0.0f, 1.0f); // 結果を [0, 1] にクランプ
}

float chomath::Dot(const Vector3& v1, const Vector3& v2)
{
	float result;
	result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	return result;
}

Matrix4 chomath::MakeRotateAxisAngle(const Vector3& axis, float angle)
{
	Vector3 normAxis = axis;
	float axisLength = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
	if (axisLength != 0.0f) {
		normAxis.x /= axisLength;
		normAxis.y /= axisLength;
		normAxis.z /= axisLength;
	}

	float cosTheta = std::cos(angle);
	float sinTheta = std::sin(angle);
	float oneMinusCosTheta = 1.0f - cosTheta;

	Matrix4 rotateMatrix;

	rotateMatrix.m[0][0] = cosTheta + normAxis.x * normAxis.x * oneMinusCosTheta;
	rotateMatrix.m[0][1] = normAxis.x * normAxis.y * oneMinusCosTheta - normAxis.z * sinTheta;
	rotateMatrix.m[0][2] = normAxis.x * normAxis.z * oneMinusCosTheta + normAxis.y * sinTheta;
	rotateMatrix.m[0][3] = 0.0f;

	rotateMatrix.m[1][0] = normAxis.y * normAxis.x * oneMinusCosTheta + normAxis.z * sinTheta;
	rotateMatrix.m[1][1] = cosTheta + normAxis.y * normAxis.y * oneMinusCosTheta;
	rotateMatrix.m[1][2] = normAxis.y * normAxis.z * oneMinusCosTheta - normAxis.x * sinTheta;
	rotateMatrix.m[1][3] = 0.0f;

	rotateMatrix.m[2][0] = normAxis.z * normAxis.x * oneMinusCosTheta - normAxis.y * sinTheta;
	rotateMatrix.m[2][1] = normAxis.z * normAxis.y * oneMinusCosTheta + normAxis.x * sinTheta;
	rotateMatrix.m[2][2] = cosTheta + normAxis.z * normAxis.z * oneMinusCosTheta;
	rotateMatrix.m[2][3] = 0.0f;

	rotateMatrix.m[3][0] = 0.0f;
	rotateMatrix.m[3][1] = 0.0f;
	rotateMatrix.m[3][2] = 0.0f;
	rotateMatrix.m[3][3] = 1.0f;

	rotateMatrix = Transpose(rotateMatrix);

	return rotateMatrix;
}

Matrix4 chomath::DirectionToDirection(const Vector3& from, const Vector3& to) {
	// 入力ベクトルを正規化
	Vector3 normalizedFrom = Vector3::Normalize(from);
	Vector3 normalizedTo = Vector3::Normalize(to);

	// 回転軸を計算
	Vector3 axis = Vector3::Cross(normalizedFrom, normalizedTo);
	axis.Normalize();

	// 特殊ケース: from と -to が一致する場合
	if (normalizedFrom == -normalizedTo) {
		// 任意の直交軸を選択
		if (std::abs(normalizedFrom.x) < std::abs(normalizedFrom.y)) {
			axis = Vector3::Normalize(Vector3{ 0.0f, -normalizedFrom.z, normalizedFrom.y });
		} else {
			axis = Vector3::Normalize(Vector3{ -normalizedFrom.y, normalizedFrom.x, 0.0f });
		}
	}

	// cosTheta と sinTheta を計算
	float cosTheta = normalizedFrom.Dot(normalizedTo);
	float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

	// 最初から転置された形で回転行列を作成
	Matrix4 rotateMatrix = {
		(axis.x * axis.x) * (1 - cosTheta) + cosTheta,        (axis.x * axis.y) * (1 - cosTheta) + (axis.z * sinTheta), (axis.x * axis.z) * (1 - cosTheta) - (axis.y * sinTheta), 0.0f,
		(axis.x * axis.y) * (1 - cosTheta) - (axis.z * sinTheta), (axis.y * axis.y) * (1 - cosTheta) + cosTheta,        (axis.y * axis.z) * (1 - cosTheta) + (axis.x * sinTheta), 0.0f,
		(axis.x * axis.z) * (1 - cosTheta) + (axis.y * sinTheta), (axis.y * axis.z) * (1 - cosTheta) - (axis.x * sinTheta), (axis.z * axis.z) * (1 - cosTheta) + cosTheta,        0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return rotateMatrix;
}

float chomath::DegreesToRadians(const float& degrees)
{
	return degrees * std::numbers::pi_v<float> / 180.0f;
}

Vector3 chomath::DegreesToRadians(const Vector3& degrees)
{
	Vector3 result;

	result.x = DegreesToRadians(degrees.x);
	result.y = DegreesToRadians(degrees.y);
	result.z = DegreesToRadians(degrees.z);

	return result;
}


float chomath::RadiansToDegrees(const float& radians)
{
	return radians * 180.0f / std::numbers::pi_v<float>;
}

Vector3 chomath::RadiansToDegrees(const Vector3& radians)
{
	Vector3 result;

	result.x = RadiansToDegrees(radians.x);
	result.y = RadiansToDegrees(radians.y);
	result.z = RadiansToDegrees(radians.z);

	return result;
}

Quaternion chomath::MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle)
{
	Vector3 normAxis = axis;

	if (normAxis.Length() == 0.0f) {
		return Quaternion(0.0f, 0.0f, 0.0f, 1.0f); // 単位クォータニオンを返す
	}

	normAxis.Normalize();
	float sinHalfAngle = std::sin(angle / 2.0f);
	float cosHalfAngle = std::cos(angle / 2.0f);
	return { normAxis.x * sinHalfAngle, normAxis.y * sinHalfAngle, normAxis.z * sinHalfAngle, cosHalfAngle };
}

Vector3 chomath::RotateVector(const Vector3& vector, const Quaternion& quaternion)
{
	// q * v * q^-1 を計算する
	Quaternion qv = { vector.x, vector.y, vector.z, 0.0f };

	// クォータニオンの共役を計算
	Quaternion qConjugate = { -quaternion.x, -quaternion.y, -quaternion.z, quaternion.w };

	// q * v
	Quaternion qvRotated = {
		quaternion.w * qv.x + quaternion.y * qv.z - quaternion.z * qv.y,
		quaternion.w * qv.y + quaternion.z * qv.x - quaternion.x * qv.z,
		quaternion.w * qv.z + quaternion.x * qv.y - quaternion.y * qv.x,
		-quaternion.x * qv.x - quaternion.y * qv.y - quaternion.z * qv.z
	};

	// (q * v) * q^-1
	Quaternion result = {
		qvRotated.w * qConjugate.x + qvRotated.x * qConjugate.w + qvRotated.y * qConjugate.z - qvRotated.z * qConjugate.y,
		qvRotated.w * qConjugate.y + qvRotated.y * qConjugate.w + qvRotated.z * qConjugate.x - qvRotated.x * qConjugate.z,
		qvRotated.w * qConjugate.z + qvRotated.z * qConjugate.w + qvRotated.x * qConjugate.y - qvRotated.y * qConjugate.x,
		-qvRotated.x * qConjugate.x - qvRotated.y * qConjugate.y - qvRotated.z * qConjugate.z
	};

	// 結果をベクトルとして返す
	return { result.x, result.y, result.z };
}

Matrix4 chomath::MakeRotateMatrix(const Quaternion& quaternion) {
	Matrix4 matrix;

	// クォータニオン成分の積
	float xx = quaternion.x * quaternion.x;
	float yy = quaternion.y * quaternion.y;
	float zz = quaternion.z * quaternion.z;
	float xy = quaternion.x * quaternion.y;
	float xz = quaternion.x * quaternion.z;
	float yz = quaternion.y * quaternion.z;
	float wx = quaternion.w * quaternion.x;
	float wy = quaternion.w * quaternion.y;
	float wz = quaternion.w * quaternion.z;

	// 左手座標系用の回転行列
	matrix.m[0][0] = 1.0f - 2.0f * (yy + zz);
	matrix.m[0][1] = 2.0f * (xy + wz); // 符号反転なし
	matrix.m[0][2] = 2.0f * (xz - wy); // 符号反転（Z軸符号反転）
	matrix.m[0][3] = 0.0f;

	matrix.m[1][0] = 2.0f * (xy - wz); // 符号反転なし
	matrix.m[1][1] = 1.0f - 2.0f * (xx + zz);
	matrix.m[1][2] = 2.0f * (yz + wx); // 符号反転（Z軸符号反転）
	matrix.m[1][3] = 0.0f;

	matrix.m[2][0] = 2.0f * (xz + wy); // 符号反転（Z軸符号反転）
	matrix.m[2][1] = 2.0f * (yz - wx); // 符号反転（Z軸符号反転）
	matrix.m[2][2] = 1.0f - 2.0f * (xx + yy);
	matrix.m[2][3] = 0.0f;

	matrix.m[3][0] = 0.0f;
	matrix.m[3][1] = 0.0f;
	matrix.m[3][2] = 0.0f;
	matrix.m[3][3] = 1.0f;

	return matrix;
}




float chomath::Dot(const Quaternion& q0, const Quaternion& q1)
{
	return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}




// オイラー角からクォータニオンを生成

Quaternion chomath::FromEulerAngles(const Vector3& eulerAngles) {
	Quaternion qx = MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), eulerAngles.x);
	Quaternion qy = MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), eulerAngles.y);
	Quaternion qz = MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), eulerAngles.z);

	// ZYX の順に合成
	return qz * qx * qy;

	//// 半角を計算
	//float halfX = eulerAngles.x * 0.5f; // ピッチ（X軸）
	//float halfY = eulerAngles.y * 0.5f; // ヨー（Y軸）
	//float halfZ = eulerAngles.z * 0.5f; // ロール（Z軸）

	//// 三角関数を計算
	//float cx = std::cos(halfX); // cos(θx / 2)
	//float sx = std::sin(halfX); // sin(θx / 2)
	//float cy = std::cos(halfY); // cos(θy / 2)
	//float sy = std::sin(halfY); // sin(θy / 2)
	//float cz = std::cos(halfZ); // cos(θz / 2)
	//float sz = std::sin(halfZ); // sin(θz / 2)

	//// 左手座標系を考慮したXYZ順のクォータニオン計算
	//return Quaternion{
	//	sx * cy * cz + cx * sy * sz, // X
	//	cx * sy * cz - sx * cy * sz, // Y
	//	cx * cy * sz - sx * sy * cz, // Z
	//	cx * cy * cz + sx * sy * sz  // W
	//};
}

// クォータニオンからオイラー角を計算（回転順序を指定）

Vector3 chomath::ToEulerAngles(const Quaternion& q, RotationOrder order) {
	Vector3 angles;

	switch (order) {
	case RotationOrder::XYZ: {
		float sinp = 2 * (q.w * q.x + q.y * q.z);
		if (std::abs(sinp) >= 1)
			angles.x = std::copysign(PiF / 2, sinp);
		else
			angles.x = std::asin(sinp);

		float siny_cosp = 2 * (q.w * q.y - q.z * q.x);
		float cosy_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
		angles.y = std::atan2(siny_cosp, cosy_cosp);

		float sinr_cosp = 2 * (q.w * q.z + q.x * q.y);
		float cosr_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		angles.z = std::atan2(sinr_cosp, cosr_cosp);
		break;
	}

	case RotationOrder::YXZ: {
		float sinp = -2 * (q.w * q.y - q.x * q.z);
		if (std::abs(sinp) >= 1)
			angles.x = std::copysign(PiF / 2, sinp);
		else
			angles.x = std::asin(sinp);

		float siny_cosp = 2 * (q.w * q.x + q.z * q.y);
		float cosy_cosp = 1 - 2 * (q.y * q.y + q.x * q.x);
		angles.y = std::atan2(siny_cosp, cosy_cosp);

		float sinr_cosp = 2 * (q.w * q.z - q.x * q.y);
		float cosr_cosp = 1 - 2 * (q.z * q.z + q.x * q.x);
		angles.z = std::atan2(sinr_cosp, cosr_cosp);
		break;
	}

	case RotationOrder::ZXY: {
		float sinp = 2 * (q.w * q.z - q.x * q.y);
		if (std::abs(sinp) >= 1)
			angles.x = std::copysign(PiF / 2, sinp);
		else
			angles.x = std::asin(sinp);

		float siny_cosp = 2 * (q.w * q.x + q.y * q.z);
		float cosy_cosp = 1 - 2 * (q.z * q.z + q.x * q.x);
		angles.y = std::atan2(siny_cosp, cosy_cosp);

		float sinr_cosp = 2 * (q.w * q.y - q.z * q.x);
		float cosr_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		angles.z = std::atan2(sinr_cosp, cosr_cosp);
		break;
	}

	case RotationOrder::ZYX: {
		float sinp = 2 * (q.w * q.y + q.z * q.x);
		if (std::abs(sinp) >= 1)
			angles.x = std::copysign(PiF / 2, sinp);
		else
			angles.x = std::asin(sinp);

		float siny_cosp = 2 * (q.w * q.x - q.y * q.z);
		float cosy_cosp = 1 - 2 * (q.z * q.z + q.x * q.x);
		angles.y = std::atan2(siny_cosp, cosy_cosp);

		float sinr_cosp = 2 * (q.w * q.z - q.x * q.y);
		float cosr_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		angles.z = std::atan2(sinr_cosp, cosr_cosp);
		break;
	}

	case RotationOrder::YZX: {
		float sinp = 2 * (q.w * q.y - q.x * q.z);
		if (std::abs(sinp) >= 1)
			angles.x = std::copysign(PiF / 2, sinp);
		else
			angles.x = std::asin(sinp);

		float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
		float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		angles.y = std::atan2(siny_cosp, cosy_cosp);

		float sinr_cosp = 2 * (q.w * q.x - q.z * q.y);
		float cosr_cosp = 1 - 2 * (q.z * q.z + q.x * q.x);
		angles.z = std::atan2(sinr_cosp, cosr_cosp);
		break;
	}

	case RotationOrder::XZY: {
		float sinp = -2 * (q.w * q.x - q.y * q.z);
		if (std::abs(sinp) >= 1)
			angles.x = std::copysign(PiF / 2, sinp);
		else
			angles.x = std::asin(sinp);

		float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
		float cosy_cosp = 1 - 2 * (q.z * q.z + q.x * q.x);
		angles.y = std::atan2(siny_cosp, cosy_cosp);

		float sinr_cosp = 2 * (q.w * q.y - q.x * q.z);
		float cosr_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		angles.z = std::atan2(sinr_cosp, cosr_cosp);
		break;
	}

	default:
		break;
	}

	return angles;
}

Matrix4 chomath::MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate)
{
	Matrix4 result;
	Matrix4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4 rotateXYZMatrix = Multiply(Multiply(rotateZMatrix, rotateYMatrix), rotateXMatrix);

	Matrix4 translateMatrix = MakeTranslateMatrix(translate);
	//result = Multiply(rotateXYZMatrix, Multiply(scaleMatrix, translateMatrix));
	result = Multiply(scaleMatrix, Multiply(rotateXYZMatrix, translateMatrix));
	//result = Multiply(translateMatrix, Multiply(scaleMatrix, rotateXYZMatrix));
	result = Multiply(Multiply(scaleMatrix, rotateXYZMatrix), translateMatrix);
	return result;
}

Matrix4 chomath::MakeAffineMatrix(const Scale& scale, const Quaternion& rotate, const Vector3& translate)
{
	Matrix4 result;
	Matrix4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4 rotateMatrix = MakeRotateMatrix(rotate);
	Matrix4 translateMatrix = MakeTranslateMatrix(translate);
	result = scaleMatrix * rotateMatrix * translateMatrix;
	return result;
}

Vector3 chomath::TransformDirection(const Vector3& v, const Matrix4& m)
{
	return {
		m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z,
		m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z,
		m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z
	};
}

float chomath::LerpShortAngle(float startAngle, float endAngle, float t) {
	// 角度の差を計算
	float delta = std::fmod(endAngle - startAngle, 2 * std::numbers::pi_v<float>);

	// 最短距離を考慮して、必要ならば角度の差を調整
	if (delta > std::numbers::pi_v<float>) {
		delta -= 2 * std::numbers::pi_v<float>;
	}
	else if (delta < -std::numbers::pi_v<float>) {
		delta += 2 * std::numbers::pi_v<float>;
	}

	// Lerpを使って角度を補間
	float result = startAngle + delta * t;

	// 結果を0~2πの範囲に収める
	result = std::fmod(result, 2 * std::numbers::pi_v<float>);
	if (result < 0.0f) {
		result += 2 * std::numbers::pi_v<float>;
	}

	return result;
}

Quaternion chomath::MakeLookRotation(const Vector3& forward, const Vector3& up)
{
	Vector3 f = forward;
	f.Normalize();

	Vector3 r = Vector3::Cross(up, f); // 右ベクトル = 上 × 前
	r.Normalize();

	Vector3 u = Vector3::Cross(f, r); // 上ベクトル = 前 × 右

	// 回転行列を構築（右・上・前）
	Matrix4 rotMat = {
		r.x, u.x, f.x, 0.0f,
		r.y, u.y, f.y, 0.0f,
		r.z, u.z, f.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return Quaternion::FromMatrix(rotMat); // 回転行列 → クォータニオン変換
}

Vector3 chomath::GetForwardVectorFromMatrix(const Matrix4& rotMatrix)
{
	return Vector3(
		rotMatrix.m[0][2], // x
		rotMatrix.m[1][2], // y
		rotMatrix.m[2][2]  // z
	);
}

// Matrix4 から SRT を取り出す関数
chomath::SRT chomath::DecomposeMatrix(const Matrix4& in)
{
	chomath::SRT out;

	Matrix4 mat = in; // コピーして操作する
	mat.Transpose(); // 行列を転置（左手座標系用）

	// 1) 平行移動成分は m[row][col] の (2,3)
	out.translation.x = mat.m[0][3];
	out.translation.y = mat.m[1][3];
	out.translation.z = mat.m[2][3];

	// 2) スケール成分は各カラムベクトルの長さ
	Vector3 col0 = { mat.m[0][0], mat.m[1][0], mat.m[2][0] };
	Vector3 col1 = { mat.m[0][1], mat.m[1][1], mat.m[2][1] };
	Vector3 col2 = { mat.m[0][2], mat.m[1][2], mat.m[2][2] };
	out.scale.x = std::sqrt(col0.x * col0.x + col0.y * col0.y + col0.z * col0.z);
	out.scale.y = std::sqrt(col1.x * col1.x + col1.y * col1.y + col1.z * col1.z);
	out.scale.z = std::sqrt(col2.x * col2.x + col2.y * col2.y + col2.z * col2.z);

	// 3) 純粋な回転行列を取り出す
	Matrix4 rotMat = mat;
	// 各軸ベクトルを正規化
	if (out.scale.x != 0) { rotMat.m[0][0] /= out.scale.x; rotMat.m[1][0] /= out.scale.x; rotMat.m[2][0] /= out.scale.x; }
	if (out.scale.y != 0) { rotMat.m[0][1] /= out.scale.y; rotMat.m[1][1] /= out.scale.y; rotMat.m[2][1] /= out.scale.y; }
	if (out.scale.z != 0) { rotMat.m[0][2] /= out.scale.z; rotMat.m[1][2] /= out.scale.z; rotMat.m[2][2] /= out.scale.z; }

	// 回転行列 → オイラー角（XYZ 順）
	{
		// まず Y 軸回転 β を取得（列0, 行2）
		float sy = rotMat.m[0][2];
		// asin の入力範囲をクリップしておく（数値誤差対策）
		if (sy > 1.0f) sy = 1.0f;
		if (sy < -1.0f) sy = -1.0f;
		out.rotationEuler.y = std::asin(sy);

		// cos(β) でジンバルロック判定
		float cosY = std::cos(out.rotationEuler.y);
		if (std::fabs(cosY) > 1e-6f)
		{
			// β が ±90° でないときは通常分解
			// α (X 軸回転) = atan2( -m[1][2], m[2][2] )
			out.rotationEuler.x = std::atan2(-rotMat.m[1][2], rotMat.m[2][2]);
			// γ (Z 軸回転) = atan2( -m[0][1], m[0][0] )
			out.rotationEuler.z = std::atan2(-rotMat.m[0][1], rotMat.m[0][0]);
		}
		else
		{
			// ジンバルロック時：β = ±90° のとき
			// α を 0 に固定し、γ を別式で算出
			out.rotationEuler.x = 0.0f;
			out.rotationEuler.z = std::atan2(rotMat.m[1][0], rotMat.m[1][1]);
		}
	}

	return out;
}

Quaternion chomath::MakeQuaternionRotation(const Vector3& rad, const Vector3& preRad, const Quaternion& quaternion)
{
	// 差分計算
	Vector3 diff = rad - preRad;

	// 各軸のクオータニオンを作成
	Quaternion qx = chomath::MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), diff.x);
	Quaternion qy = chomath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), diff.y);
	Quaternion qz = chomath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), diff.z);

	// 同時回転を累積
	Quaternion q = quaternion * qx * qy * qz;
	return q.Normalize(); // 正規化して返す
}

Quaternion chomath::MakeEulerRotation(const Vector3& rad)
{
	// オイラー角からクォータニオンを作成
	// 各軸のクオータニオンを作成
	Quaternion qx = chomath::MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), rad.x);
	Quaternion qy = chomath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), rad.y);
	Quaternion qz = chomath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), rad.z);

	// 同時回転を累積
	Quaternion q = qx * qy * qz;
	return q.Normalize(); // 正規化して返す
}

// ALLBillboard
Matrix4 chomath::BillboardMatrix(const Matrix4 cameraMatrix)
{
	Matrix4 result;

	float cosY = cos(PiF);
	float sinY = sin(PiF);

	Matrix4 backToFrontMatrix = {
		cosY, 0.0f, -sinY, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sinY, 0.0f, cosY, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };

	result = Multiply(backToFrontMatrix, cameraMatrix);
	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;

	return result;
}

float chomath::Lerp(const float& start, const float& end, const float& t)
{
	return start + (end - start) * t;
}


