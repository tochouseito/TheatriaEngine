#include "physicsengine_pch.h"
#include "2D/d2_world.h"
#include "2D/d2_body.h"
#include "2D/d2_converter.h"
// box2d
#include <box2d.h>

using namespace physics::d2;

// ワールド作成
Id2World* physics::d2::CreateWorld(d2Backend backend)
{
	switch (backend)
	{
	case physics::d2::d2Backend::box2d:
		return new box2dWorld(backend);
		break;
	case physics::d2::d2Backend::chophysics:
		return new choPhysicsWorld(backend);
		break;
	default:
		break;
	}
}

void physics::d2::DestroyWorld(Id2World* world)
{
	delete world; // ワールドの破棄
}

physics::d2::box2dWorld::box2dWorld(d2Backend be)
{
	backend = be;	
	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = b2Vec2(0.0f, -9.8f); // 重力の設定
	world = &b2CreateWorld(&worldDef);
}

physics::d2::box2dWorld::~box2dWorld()
{
	b2DestroyWorld(*world); // ワールドの破棄
}

Id2Body* physics::d2::box2dWorld::CreateBody(const Id2BodyDef& bodyDef)
{
	b2BodyDef b2BodyDef = b2DefaultBodyDef();
	b2BodyDef.userData = bodyDef.userData; // ユーザーデータの設定
	b2BodyDef.type = ConvertBodyType(bodyDef.type); // ボディタイプの変換
	b2BodyDef.gravityScale = bodyDef.gravityScale; // 重力スケールの設定
	b2BodyDef.fixedRotation = bodyDef.fixedRotation; // 回転の固定設定
	b2BodyDef.position = b2Vec2(bodyDef.position.x, bodyDef.position.y); // 位置の設定
	b2BodyDef.rotation = b2MakeRot(bodyDef.angle); // 角度の設定
	return new box2dBody(*world, &b2BodyDef);
}

void physics::d2::box2dWorld::DestroyBody(Id2Body* body)
{
	delete body; // ボディの破棄
}

void physics::d2::box2dWorld::Step(const float& deltaTime)
{
}

Vector2 physics::d2::box2dWorld::GetGravity() const
{
	return Vector2();
}

void physics::d2::box2dWorld::SetGravity(const Vector2& gravity)
{
}

Id2Body* physics::d2::choPhysicsWorld::CreateBody(const Id2BodyDef& bodyDef)
{
	return nullptr;
}

void physics::d2::choPhysicsWorld::DestroyBody(Id2Body* body)
{
}

void physics::d2::choPhysicsWorld::Step(const float& deltaTime)
{
}

Vector2 physics::d2::choPhysicsWorld::GetGravity() const
{
	return Vector2();
}

void physics::d2::choPhysicsWorld::SetGravity(const Vector2& gravity)
{
}
