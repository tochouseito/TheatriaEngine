#include "physicsengine_pch.h"
#include "2D/d2_shape.h"
#include "2D/d2_body.h"
#include "2D/d2_polygon.h"

#include <box2d.h>

using namespace physics::d2;

struct box2dShape::Impl
{
	b2ShapeId shape; // Box2Dの形状
};

physics::d2::box2dShape::box2dShape()
	: impl(std::make_unique<box2dShape::Impl>())
{
}

void physics::d2::box2dShape::CreatePolygonShape(Id2Body* body, Id2ShapeDef* shapeDef, Id2Polygon* polygon)
{
	box2dBody* box2dBodyPtr = static_cast<box2dBody*>(body);
	box2dPolygon* box2dPolygonPtr = static_cast<box2dPolygon*>(polygon);
	b2ShapeDef def = b2DefaultShapeDef();
	def.density = shapeDef->density; // 密度の設定
	def.material.friction = shapeDef->friction; // 摩擦係数の設定
	def.material.restitution = shapeDef->restitution; // 反発係数の設定
	def.isSensor = shapeDef->isSensor; // センサーかどうかの設定
	impl->shape = b2CreatePolygonShape(box2dBodyPtr->GetBody(),  &def, box2dPolygonPtr->GetPolygon());
}

void physics::d2::box2dShape::Destroy()
{
	b2DestroyShape(impl->shape, true); // 形状を削除
}
