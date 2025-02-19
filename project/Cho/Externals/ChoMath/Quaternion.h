#pragma once
#include <cmath>

/// <summary>
/// クォータニオン構造体
/// </summary>
struct Quaternion final {
    float x, y, z, w;

    // コンストラクタ
    constexpr Quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f)
        : x(x), y(y), z(z), w(w) {}

    // 初期化
    void Initialize() {
        x = 0.0f; y = 0.0f; z = 0.0f; w = 1.0f;
    }

    // 加算（演算子）
    Quaternion operator+(const Quaternion& other) const {
        return { x + other.x, y + other.y, z + other.z, w + other.w };
    }

    // 減算（演算子）
    Quaternion operator-(const Quaternion& other) const {
        return { x - other.x, y - other.y, z - other.z, w - other.w };
    }

    // スカラー乗算（演算子）
    Quaternion operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar, w * scalar };
    }

    // スカラー除算（演算子）
    Quaternion operator/(float scalar) const {
        if (scalar == 0.0f) {
            return { 0.0f, 0.0f, 0.0f, 1.0f }; // デフォルトの単位クォータニオンを返す
        }
        return { x / scalar, y / scalar, z / scalar, w / scalar };
    }

    // クォータニオンの積 (this * other)
    Quaternion Multiply(const Quaternion& other) const {
        /*return {
            other.w * x + other.x * w + other.y * z - other.z * y,
            other.w * y - other.x * z + other.y * w + other.z * x,
            other.w * z + other.x * y - other.y * x + other.z * w,
            other.w * w - other.x * x - other.y * y - other.z * z
        };*/
        return {
            w * other.x + x * other.w + y * other.z - z * other.y, // x
            w * other.y - x * other.z + y * other.w + z * other.x, // y
            w * other.z + x * other.y - y * other.x + z * other.w, // z
            w * other.w - x * other.x - y * other.y - z * other.z  // w
        };
    }

    // クォータニオン積（演算子）
    Quaternion operator*(const Quaternion& other) const {
        return Multiply(other);
    }

    // 共役クォータニオン
    void Conjugate() {
        x = -x;
        y = -y;
        z = -z;
        w = w;
    }

    // ノルム（大きさ）の計算
    float Norm() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    // 内積
    float Dot(const Quaternion& other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    // 正規化
    void Normalize() {
        float norm = Norm();
        if (norm == 0.0f) {
            x = 0.0f;y = 0.0f;z = 0.0f;w = 1.0f;
        } else {
            x /= norm;y /= norm;z /= norm;w /= norm;
        }
    }

    // 逆クォータニオン
    void Inverse() {
        Quaternion q = { x,y,z,w };
        Quaternion conjugate = Quaternion::Conjugate(q);
        float norm = Norm();
        float normSq = norm * norm;
        if (normSq == 0.0f) {
            x = 0.0f;y = 0.0f;z = 0.0f;w = 1.0f;
        } else {
            x = conjugate.x / normSq;y = conjugate.y / normSq;
            z = conjugate.z / normSq; w = conjugate.w / normSq;
        }
    }
public:// 静的メンバ

    // 線形補間（Lerp）
    static Quaternion Lerp(const Quaternion& start, const Quaternion& end, float t) {
        Quaternion result = (start * (1.0f - t) + end * t);
        result.Normalize();
        return result;
    }

    // 球面線形補間（Slerp）
    static Quaternion Slerp(const Quaternion& start, const Quaternion& end, float t) {
        // クォータニオンの内積を計算
		float dot = start.Dot(end);
        const float threshold = 0.9995f;

        // クォータニオンが反対向きの場合、内積が負になるので符号を反転
        if (dot < 0.0f) {
            dot = -dot;
            Quaternion negQ1 = { -end.x, -end.y, -end.z, -end.w };
            return Slerp(start, negQ1, t);
        }

        // 内積が閾値以上の場合、線形補間を使用
        if (dot > threshold) {
            Quaternion result = {
                start.x + t * (end.x - start.x),
                start.y + t * (end.y - start.y),
                start.z + t * (end.z - start.z),
                start.w + t * (end.w - start.w)
            };
            // 正規化
            float norm = std::sqrt(result.x * result.x + result.y * result.y + result.z * result.z + result.w * result.w);
            return { result.x / norm, result.y / norm, result.z / norm, result.w / norm };
        }

        // 角度を計算
        float theta_0 = std::acos(dot);  // θ0 = angle between input vectors
        float theta = theta_0 * t;       // θ = angle between q0 and result
        float sin_theta = std::sin(theta); // Compute this value only once
        float sin_theta_0 = std::sin(theta_0); // Compute this value only once

        float s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;  // s0 = sin((1 - t) * theta) / sin(theta)
        float s1 = sin_theta / sin_theta_0; // s1 = sin(t * theta) / sin(theta)

        return {
            s0 * start.x + s1 * end.x,
            s0 * start.y + s1 * end.y,
            s0 * start.z + s1 * end.z,
            s0 * start.w + s1 * end.w
        };

    }

    // 単位クォータニオン
    static Quaternion Identity() {
        return { 0.0f, 0.0f, 0.0f, 1.0f };
    }

    // 共役クォータニオン
    static Quaternion Conjugate(const Quaternion& q) {
        Quaternion result;
        result.x = -q.x;
        result.y = -q.y;
        result.z = -q.z;
        result.w = q.w;
        return result;
    }

    // 正規化
    static Quaternion Normalize(const Quaternion& q) {
        Quaternion result;
        float norm = q.Norm();
        if (norm == 0.0f) {
            result.x = 0.0f; result.y = 0.0f; result.z = 0.0f; result.w = 1.0f;
        } else {
            result.x /= norm; result.y /= norm; result.z /= norm; result.w /= norm;
        }
        return result;
    }

    // 逆クォータニオン
    static Quaternion Inverse(const Quaternion& q) {
        Quaternion result;
        Quaternion conjugate = Quaternion::Conjugate(q);
        float norm = q.Norm();
        float normSq = norm * norm;
        if (normSq == 0.0f) {
            result.x = 0.0f; result.y = 0.0f; result.z = 0.0f; result.w = 1.0f;
        } else {
            result.x = conjugate.x / normSq; result.y = conjugate.y / normSq;
            result.z = conjugate.z / normSq; result.w = conjugate.w / normSq;
        }
        return result;
    }
};
