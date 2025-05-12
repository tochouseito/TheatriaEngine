#include "pch.h"
#include "PhysicsEngine.h"
#include "GameCore/GameObject/GameObject.h"
#include "GameCore/ECS/ECSManager.h"
#include "GameCore/ObjectContainer/ObjectContainer.h"

void ContactListener2D::BeginContact(b2Contact* contact)
{
	ObjectID objectA = static_cast<ObjectID>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	ObjectID objectB = static_cast<ObjectID>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

	CollisionEnter(objectA, objectB);
	CollisionEnter(objectB, objectA);
}

void ContactListener2D::EndContact(b2Contact* contact)
{
	ObjectID objectA = static_cast<ObjectID>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	ObjectID objectB = static_cast<ObjectID>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

	CollisionExit(objectA, objectB);
	CollisionExit(objectB, objectA);
}

void ContactListener2D::CollisionEnter(ObjectID self, ObjectID other)
{
	GameObject& selfObject = m_pObjectContainer->GetGameObject(self);
	if (!selfObject.IsActive()) { return; }
	ScriptComponent* script = m_pECS->GetComponent<ScriptComponent>(selfObject.GetEntity());
	if (script && script->isActive)
	{
		Rigidbody2DComponent* selfRb = m_pECS->GetComponent<Rigidbody2DComponent>(selfObject.GetEntity());
		if (!selfRb) { return; }
		if (!selfRb->isActive) { return; }
		// 相手のゲームオブジェクトを取得
		GameObject& otherObject = m_pObjectContainer->GetGameObject(other);
		if (!otherObject.IsActive()) { return; }
		otherObject.Initialize(false);// 初期化 これいる？？
		Rigidbody2DComponent* otherRb = m_pECS->GetComponent<Rigidbody2DComponent>(otherObject.GetEntity());
		if (!otherRb) { return; }
		if (!otherRb->isActive) { return; }
		selfRb->isCollisionStay = true;// 衝突中フラグオン
		selfRb->otherObjectID = other;
		otherRb->isCollisionStay = true;// 衝突中フラグオン
		script->onCollisionEnterFunc(otherObject);
	}
}

void ContactListener2D::CollisionExit(ObjectID self, ObjectID other)
{
	GameObject& selfObject = m_pObjectContainer->GetGameObject(self);
	if (!selfObject.IsActive()) { return; }
	ScriptComponent* script = m_pECS->GetComponent<ScriptComponent>(selfObject.GetEntity());
	if (script && script->isActive)
	{
		Rigidbody2DComponent* selfRb = m_pECS->GetComponent<Rigidbody2DComponent>(self);
		if (!selfRb) { return; }
		if (!selfRb->isActive) { return; }
		// 相手のゲームオブジェクトを取得
		GameObject& otherObject = m_pObjectContainer->GetGameObject(other);
		if (!otherObject.IsActive()) return;
		otherObject.Initialize(false);// 初期化 これいる？？
		Rigidbody2DComponent* otherRb = m_pECS->GetComponent<Rigidbody2DComponent>(other);
		if (!otherRb) { return; }
		if (!otherRb->isActive) { return; }
		selfRb->isCollisionStay = false;// 衝突中フラグオフ
		selfRb->otherObjectID = other;
		otherRb->isCollisionStay = false;// 衝突中フラグオフ
		script->onCollisionExitFunc(otherObject);
	}
}

float RayCastCallback::ReportFixture(b2Fixture* a_Fixture, const b2Vec2& a_Point, const b2Vec2& a_Normal, float a_Fraction)
{
	ObjectID id = static_cast<ObjectID>(a_Fixture->GetBody()->GetUserData().pointer);
	GameObject& hitObject = m_ObjectContainer->GetGameObject(id);
	if (hitObject.GetTag() != m_Tag)
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
