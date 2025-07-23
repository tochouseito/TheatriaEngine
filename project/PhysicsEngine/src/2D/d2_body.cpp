#include "physicsengine_pch.h"
#include "2D/d2_body.h"

// box2d
#include <box2d.h>

using namespace physics::d2;

physics::d2::box2dBody::box2dBody(b2WorldId worldId, const b2BodyDef* def)
{
	m_body = &b2CreateBody(worldId, def); // Box2Dのボディを作成
}

Id2Fixture* physics::d2::box2dBody::CreateFixture(const Id2FixtureDef& fixtureDef)
{
    return nullptr;
}

void physics::d2::box2dBody::DestroyFixture(Id2Fixture* fixture)
{
}

void physics::d2::box2dBody::SetAwake(bool flag)
{
}
