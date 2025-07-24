#include "physicsengine_pch.h"
#include "2D/d2_world.h"
#include "2D/d2_body.h"
#include "2D/d2_converter.h"
// box2d
#include <box2d.h>

using namespace physics::d2;

struct box2dWorld::Impl
{
	b2WorldId world;
};

physics::d2::box2dWorld::box2dWorld(d2Backend be)
	: impl(std::make_unique<box2dWorld::Impl>())
{
	backend = be;	
}

void physics::d2::box2dWorld::Create()
{
	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = b2Vec2(0.0f, -9.8f); // 重力の設定
	impl->world = b2CreateWorld(&worldDef); // Box2Dのワールドを作成
}

void physics::d2::box2dWorld::Destroy()
{
	b2DestroyWorld(impl->world); // Box2Dのワールドを破棄
}

void physics::d2::box2dWorld::Step(const float& deltaTime)
{
	deltaTime;
}

Vector2 physics::d2::box2dWorld::GetGravity() const
{
	return Vector2();
}

void physics::d2::box2dWorld::SetGravity(const Vector2& gravity)
{
	gravity;
}

b2WorldId physics::d2::box2dWorld::GetWorld() const { return impl->world; }

struct choPhysicsWorld::Impl
{
	int dummy = 0; // ChoPhysicsの実装に必要なダミー変数
};

physics::d2::choPhysicsWorld::choPhysicsWorld(d2Backend be)
	: impl(std::make_unique<choPhysicsWorld::Impl>())
{
	backend = be;
}

void physics::d2::choPhysicsWorld::Create()
{
}

void physics::d2::choPhysicsWorld::Destroy()
{
}

void physics::d2::choPhysicsWorld::Step(const float& deltaTime)
{
	deltaTime;
}

Vector2 physics::d2::choPhysicsWorld::GetGravity() const
{
	return Vector2();
}

void physics::d2::choPhysicsWorld::SetGravity(const Vector2& gravity)
{
	gravity;
}
