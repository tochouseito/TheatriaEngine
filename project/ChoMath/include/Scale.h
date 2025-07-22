#pragma once
struct Scale final {
	float x = 1.0f;
	float y = 1.0f;
	float z = 1.0f;
public:
	// 初期化
	inline void Initialize() {
		x = 1.0f;
		y = 1.0f;
		z = 1.0f;
	}

	// ゼロスケール
	inline void Zero() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	// 等号演算子
	bool operator==(const Scale& other) const { return x == other.x && y == other.y && z == other.z; }

	// 不等号演算子
	bool operator!=(const Scale& other) const { return !(*this == other); }

	// 小なりイコール演算子 (<=)
	bool operator<=(const Scale& other) const {
		return (this->x <= other.x && this->y <= other.y && this->z <= other.z);
	}

	// スケール加算
	inline Scale operator+(const Scale& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}

	// スケール減算
	inline Scale operator-(const Scale& other) const {
		return { x - other.x, y - other.y, z - other.z };
	}

	// スケール乗算
	inline Scale operator*(float scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	// スケール除算
	inline Scale operator/(float scalar) const {
		if (scalar == 0.0f) {
			return { 1.0f, 1.0f, 1.0f };
		}
		return { x / scalar, y / scalar, z / scalar };
	}

	// += 演算子
	Scale& operator+=(const Scale& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	// -= 演算子
	Scale& operator-=(const Scale& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	// *= 演算子
	Scale& operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	// /= 演算子
	Scale& operator/=(float scalar) {
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

	// 均一スケール判定
	inline bool IsUniform() const {
		return x == y && y == z;
	}
public:
	// 線形補間
	static Scale Lerp(const Scale& start, const Scale& end, float t) {
		return Scale{
			start.x + t * (end.x - start.x), start.y + t * (end.y - start.y),
			start.z + t * (end.z - start.z)
		};
	}
};