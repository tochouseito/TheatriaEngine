#pragma once
/// <summary>
/// カラーデータ
/// </summary>
struct Color final {
    float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	float a = 1.0f;
    // カラー加算
    inline Color operator+(const Color& other) const {
        return { r + other.r, g + other.g, b + other.b, a + other.a };
    }

    // カラー減算
    inline Color operator-(const Color& other) const {
        return { r - other.r, g - other.g, b - other.b, a - other.a };
    }

    // スカラー乗算
    inline Color operator*(float scalar) const {
        return { r * scalar, g * scalar, b * scalar, a * scalar };
    }

    // スカラー除算
    inline Color operator/(float scalar) const {
        if (scalar == 0.0f) {
            return { 0.0f, 0.0f, 0.0f, 0.0f };
        }
        return { r / scalar, g / scalar, b / scalar, a / scalar };
    }

    // カラー補間（Lerp）
    inline static Color Lerp(const Color& start, const Color& end, float t) {
        return {
            start.r + t * (end.r - start.r),
            start.g + t * (end.g - start.g),
            start.b + t * (end.b - start.b),
            start.a + t * (end.a - start.a)
        };
    }

    // 明るさを調整
    inline Color AdjustBrightness(float factor) const {
        return { r * factor, g * factor, b * factor, a };
    }

    // 透明度を設定
    inline Color SetAlpha(float alpha) const {
        return { r, g, b, alpha };
    }
};