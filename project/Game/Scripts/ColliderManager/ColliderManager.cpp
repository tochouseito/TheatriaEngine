#include "PrecompiledHeader.h"
#include "Game/Scripts/Collider/Collider.h"
#include "ColliderManager.h"
#include "Game/Scripts/ColliderID/ColliderType.h"

void CollisionManager::Initialize()
{
}

void CollisionManager::Reset()
{
	// リストを空っぽにする
	colliders_.clear();
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB)
{
	// どちらかがNoneだったり同じタイプなら早期リターン
	if (colliderA->GetTypeID() == TypeID::kNone || colliderB->GetTypeID() == TypeID::kNone) {
		return;
	}
	if (colliderA->GetTypeID() == colliderB->GetTypeID()) {
		return;
	}
	/*if (colliderA->IsActive() == false || colliderB->IsActive() == false) {
		return;
	}*/

	Vector3 PositionA = colliderA->GetCenterPosition();
	// コライダーB
	Vector3 PositionB = colliderB->GetCenterPosition();
	// 座標の差分ベクトル
	Vector3 subtract = PositionB - PositionA;
	// 座標AとBの距離を求める
	float distance = Length(subtract);
	// 球と球の交差判定
	if (distance <= colliderA->GetRadius() + colliderB->GetRadius()) {
		// コライダーAの衝突時コールバックを呼び出す
		colliderA->OnCollision(colliderB);
		// コライダーBの衝突時コールバックを呼び出す
		colliderB->OnCollision(colliderA);
	}
}

void CollisionManager::CheckAllCollisions()
{
	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		Collider* colliderA = *itrA;

		// イテレータBはイテレータAの次の要素から回す（重複判定を回避）
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;
			// ペアの当たり判定
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::AddCollider(Collider* collider)
{
	colliders_.push_back(collider);
}
