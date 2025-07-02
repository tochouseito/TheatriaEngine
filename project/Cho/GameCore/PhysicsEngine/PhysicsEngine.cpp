#include "pch.h"
#include "PhysicsEngine.h"
#include "GameCore/GameObject/GameObject.h"
#include "GameCore/ECS/ECSManager.h"
#include "GameCore/GameWorld/GameWorld.h"

void ContactListener2D::BeginContact(b2Contact* contact)
{
	Entity objectA = static_cast<Entity>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	Entity objectB = static_cast<Entity>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

	CollisionEnter(objectA, objectB);
	CollisionEnter(objectB, objectA);
}

void ContactListener2D::EndContact(b2Contact* contact)
{
	Entity objectA = static_cast<Entity>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	Entity objectB = static_cast<Entity>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

	CollisionExit(objectA, objectB);
	CollisionExit(objectB, objectA);
}

void ContactListener2D::CollisionEnter(const Entity& self, const Entity& other)
{
	ScriptComponent* script = m_pECS->GetComponent<ScriptComponent>(self);
	if (script && script->isActive)
	{
		Rigidbody2DComponent* selfRb = m_pECS->GetComponent<Rigidbody2DComponent>(self);
		if (!selfRb) { return; }
		if (!selfRb->isActive) { return; }
		// 相手のゲームオブジェクトを取得
		GameObject* otherObject = m_pGameWorld->GetGameObject(other);
		Rigidbody2DComponent* otherRb = m_pECS->GetComponent<Rigidbody2DComponent>(other);
		if (!otherRb) { return; }
		if (!otherRb->isActive) { return; }
		selfRb->isCollisionStay = true;// 衝突中フラグオン
		selfRb->otherEntity = other;
		otherRb->isCollisionStay = true;// 衝突中フラグオン
		script->onCollisionEnterFunc(*otherObject);
	}
}

void ContactListener2D::CollisionExit(const Entity& self, const Entity& other)
{
	ScriptComponent* script = m_pECS->GetComponent<ScriptComponent>(self);
	if (script && script->isActive)
	{
		Rigidbody2DComponent* selfRb = m_pECS->GetComponent<Rigidbody2DComponent>(self);
		if (!selfRb) { return; }
		if (!selfRb->isActive) { return; }
		// 相手のゲームオブジェクトを取得
		GameObject* otherObject = m_pGameWorld->GetGameObject(other);
		Rigidbody2DComponent* otherRb = m_pECS->GetComponent<Rigidbody2DComponent>(other);
		if (!otherRb) { return; }
		if (!otherRb->isActive) { return; }
		selfRb->isCollisionStay = false;// 衝突中フラグオフ
		selfRb->otherEntity = other;
		otherRb->isCollisionStay = false;// 衝突中フラグオフ
		script->onCollisionExitFunc(*otherObject);
	}
}

float RayCastCallback::ReportFixture(b2Fixture* a_Fixture, const b2Vec2& a_Point, const b2Vec2& a_Normal, float a_Fraction)
{
	Entity entity = static_cast<Entity>(a_Fixture->GetBody()->GetUserData().pointer);
	GameObject* hitObject = m_pGameWorld->GetGameObject(entity);
	if (hitObject->GetTag() != m_Tag)
	{
		return -1.0f; // タグが一致しない場合は無視
	}

	a_Fixture;
	this->hit = true;
	this->point = a_Point;
	this->normal = a_Normal;
	this->fraction = a_Fraction;
	this->fixture = a_Fixture;
	this->body = a_Fixture->GetBody();
	return a_Fraction; // 最も近いヒットのみ取得
}
