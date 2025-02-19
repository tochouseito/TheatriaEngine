#pragma once
#include <cmath>

/// <summary>
/// 3次元ベクトル構造体
/// </summary>
struct Vector3 final {
    float x, y, z;

    // コンストラクタ
    constexpr Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
        : x(x), y(y), z(z) {}

    // 初期化
    void Initialize() {
        x = 0.0f; y = 0.0f; z = 0.0f;
    }

    // 等号演算子
    bool operator==(const Vector3& other) const { return x == other.x && y == other.y && z == other.z; }

    // 不等号演算子
    bool operator!=(const Vector3& other) const { return !(*this == other); }

    // 小なりイコール演算子 (<=)
    bool operator<=(const Vector3& other) const {
        return (this->x <= other.x && this->y <= other.y && this->z <= other.z);
    }

    // 小なり演算子 (<)
    bool operator<(const Vector3& other) const {
        return (x < other.x && y < other.y && z < other.z);
    }

    // 大なり演算子 (>)
    bool operator>(const Vector3& other) const {
        return (x > other.x && y > other.y && z > other.z);
    }

    // 負号演算子のオーバーロード
    Vector3 operator-() const { return { -x, -y, -z }; }

    // ベクトル加算
    Vector3 operator+(const Vector3& other) const {
        return { x + other.x, y + other.y, z + other.z };
    }

    // ベクトル減算
    Vector3 operator-(const Vector3& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }

    // スカラー乗算
    Vector3 operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }

    // += 演算子
    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    // -= 演算子
    Vector3& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    // *= 演算子
    Vector3& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    // /= 演算子
    Vector3& operator/=(float scalar) {
        if (scalar != 0.0f) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
        }
        else {
            Initialize(); // ゼロ除算の場合、初期化
        }
        return *this;
    }

    // ベクトルの長さ
    float Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // 正規化
    void Normalize() {
        float len = Length();
        if (len == 0.0f) {
            Initialize();
            return;
        }
        x = x / len;
        y = y / len;
        z = z / len;
    }

    // 内積
    float Dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // 外積
    void Cross(const Vector3& other) {
        x = y * other.z - z * other.y;
        y = z * other.x - x * other.z;
        z = x * other.y - y * other.x;
    }

    // 距離計算
    float Distance(const Vector3& other) const {
        return std::sqrt(
            (x - other.x) * (x - other.x) +
            (y - other.y) * (y - other.y) +
            (z - other.z) * (z - other.z)
        );
    }

    // ベクトルの他のベクトルへの投影
    Vector3 ProjectOnto(const Vector3& other) const {
        float dot = Dot(other);
        float otherLengthSq = other.Length() * other.Length();
        if (otherLengthSq == 0.0f) return { 0.0f, 0.0f, 0.0f }; // 長さがゼロの場合
        return other * (dot / otherLengthSq);
    }

public:// 静的メンバ
    static Vector3 Normalize(const Vector3& v) {
        Vector3 result=v;
        float len = result.Length();
        if (len == 0.0f) {
            result.Initialize();
        }
        result.x = result.x / len;
        result.y = result.y / len;
        result.z = result.z / len;
        return result;
    }
    // 外積
    static Vector3 Cross(const Vector3& v, const Vector3& other) {
        return {
            v.y * other.z - v.z * other.y,
            v.z * other.x - v.x * other.z,
            v.x * other.y - v.y * other.x
        };
    }
    // 線形補間
    static Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {
        return Vector3{
            start.x + t * (end.x - start.x), start.y + t * (end.y - start.y),
            start.z + t * (end.z - start.z)
        };
    }
};