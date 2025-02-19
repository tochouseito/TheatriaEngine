#pragma once
/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 final {
	float x = 0.0f;
	float y = 0.0f;

public:
	// 初期化
	inline void Initialize() {
		x = 0.0f;
		y = 0.0f;
	}
    // 負号演算子のオーバーロード
    Vector2 operator-() const { return { -x, -y }; }
    // 加算
    inline Vector2 operator+(const Vector2& other) const {
        return { x + other.x, y + other.y };
    }

    // 減算
    inline Vector2 operator-(const Vector2& other) const {
        return { x - other.x, y - other.y };
    }

    // スカラー乗算
    inline Vector2 operator*(float scalar) const {
        return { x * scalar, y * scalar };
    }

    // スカラー除算
    inline Vector2 operator/(float scalar) const {
        if (scalar == 0.0f) {
            return { 0.0f, 0.0f };
        }
        return { x / scalar, y / scalar };
    }

    // += 演算子
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    // -= 演算子
    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    // 内積
    inline float Dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    // 長さ
    inline float Length() const {
        return std::sqrt(x * x + y * y);
    }

    // 正規化
    inline Vector2 Normalize() const {
        float len = Length();
        if (len == 0.0f) {
            return { 0.0f, 0.0f };
        }
        return { x / len, y / len };
    }

    // 距離
    inline float Distance(const Vector2& other) const {
        return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
    }
};