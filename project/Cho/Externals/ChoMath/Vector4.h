#pragma once
#include <cmath>

/// <summary>
/// 4次元ベクトル構造体
/// </summary>
struct Vector4 final {
    float x, y, z, w;

    // コンストラクタ
    constexpr Vector4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f)
        : x(x), y(y), z(z), w(w) {}

    // 初期化
    void Initialize() {
        x = 0.0f; y = 0.0f; z = 0.0f; w = 1.0f;
    }

    // ベクトル加算
    Vector4 operator+(const Vector4& other) const {
        return { x + other.x, y + other.y, z + other.z, w + other.w };
    }

    // ベクトル減算
    Vector4 operator-(const Vector4& other) const {
        return { x - other.x, y - other.y, z - other.z, w - other.w };
    }

    // スカラー乗算
    Vector4 operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar, w * scalar };
    }

    // ベクトルの長さ
    float Length() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    // 正規化
    void Normalize() {
        float len = Length();
        if (len == 0.0f) {
            Initialize();
        }
        x = x / len;
        y = y / len;
        z = z / len;
        w = w / len;
    }

    // 内積
    float Dot(const Vector4& other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    // 距離計算
    float Distance(const Vector4& other) const {
        return std::sqrt(
            (x - other.x) * (x - other.x) +
            (y - other.y) * (y - other.y) +
            (z - other.z) * (z - other.z) +
            (w - other.w) * (w - other.w)
        );
    }
};