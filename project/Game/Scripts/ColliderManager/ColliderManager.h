#pragma once
#include<list>
#include"Vector3.h"
class Collider;
class CollisionManager {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	void Initialize();
	// リセット
	void Reset();

	/// <summary>
	/// コライダー2つの
	/// </summary>
	/// <param name="colliderA"></param>
	/// <param name="colliderB"></param>
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	// すべての当たり判定チェック
	void CheckAllCollisions();

	void AddCollider(Collider* collider);

	// 長さ（ノルム）
	float Length(const Vector3& v) {
		float result;
		result = float(sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)));
		return result;
	}
private:
	// コライダー
	std::list<Collider*> colliders_;
};
