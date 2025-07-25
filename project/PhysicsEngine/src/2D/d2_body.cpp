#include "physicsengine_pch.h"
#include "2D/d2_body.h"
#include "2D/d2_world.h"
#include "2D/d2_converter.h"

// box2d
#include <box2d.h>

using namespace physics::d2;

struct box2dBody::Impl
{
	b2BodyId body;
	b2ShapeId* pShapeId = nullptr; // 形状IDのポインタ
};

physics::d2::box2dBody::box2dBody()
	: impl(std::make_unique<box2dBody::Impl>())
{
}

physics::d2::box2dBody::~box2dBody()
{
	Destroy();
}

void physics::d2::box2dBody::Create(Id2World* world, const Id2BodyDef& bodyDef)
{
	pWorld = static_cast<box2dWorld*>(world); // 所属するワールドを設定
	b2BodyDef b2BodyDef = b2DefaultBodyDef();
	b2BodyDef.userData = bodyDef.userData; // ユーザーデータの設定
	b2BodyDef.type = ConvertBodyType(bodyDef.type); // ボディタイプの変換
	b2BodyDef.gravityScale = bodyDef.gravityScale; // 重力スケールの設定
	b2BodyDef.fixedRotation = bodyDef.fixedRotation; // 回転の固定設定
	b2BodyDef.position = b2Vec2(bodyDef.position.x, bodyDef.position.y); // 位置の設定
	b2BodyDef.rotation = b2MakeRot(bodyDef.angle); // 角度の設定
	impl->body = b2CreateBody(pWorld->GetWorld(), &b2BodyDef); // Box2Dのボディを作成
}

void physics::d2::box2dBody::Destroy()
{
	if(impl->pShapeId)
	{
		pWorld->RemoveShapeId(*impl->pShapeId); // ワールドから形状IDを削除
		impl->pShapeId = nullptr; // ポインタをnullptrに設定
	}
	b2DestroyBody(impl->body);
}

void physics::d2::box2dBody::SetAwake(bool flag)
{
	flag;
}

b2BodyId physics::d2::box2dBody::GetBody()
{
	return impl->body;
}

box2dWorld* physics::d2::box2dBody::GetWorld() const
{
	return pWorld; // 所属するワールドを取得
}

void physics::d2::box2dBody::SetShapeId(b2ShapeId* shapeId) { impl->pShapeId = shapeId; }

// 形状IDを設定
void physics::d2::box2dBody::RemoveShapeId() { impl->pShapeId = nullptr; }
