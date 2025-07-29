#include "physicsengine_pch.h"
#include "3D/d3_world.h"
#include "3D/d3_body.h"
// BulletPhysics
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

using namespace physics::d3;

struct bulletWorld::Impl
{
	// Bulletのオブジェクト定義
	std::unique_ptr<btDefaultCollisionConfiguration> collisionConfig = nullptr;// 衝突設定
	std::unique_ptr<btCollisionDispatcher> dispatcher = nullptr;// 衝突ディスパッチャー
	std::unique_ptr<btBroadphaseInterface> broadphase = nullptr;// ブロードフェーズ
	std::unique_ptr<btSequentialImpulseConstraintSolver> solver = nullptr;// 制約ソルバー
	std::unique_ptr<btDiscreteDynamicsWorld> world = nullptr;// 物理ワールド
};

physics::d3::bulletWorld::bulletWorld():
	impl(std::make_unique<Impl>())
{
	// バックエンドを設定
	backend = d3Backend::bullet;
	// 衝突構成、検出の初期化
	impl->collisionConfig = std::make_unique<btDefaultCollisionConfiguration>();
	impl->dispatcher = std::make_unique<btCollisionDispatcher>(impl->collisionConfig.get());
	impl->broadphase = std::make_unique<btDbvtBroadphase>();
	impl->solver = std::make_unique<btSequentialImpulseConstraintSolver>();
	// 物理ワールドの初期化
	impl->world = std::make_unique<btDiscreteDynamicsWorld>(
		impl->dispatcher.get(),
		impl->broadphase.get(),
		impl->solver.get(),
		impl->collisionConfig.get()
	);
	impl->world->setGravity(btVector3(0, -9.8f, 0)); // 重力を設定
}

physics::d3::bulletWorld::~bulletWorld()
{
	// 物理ワールドの解放
	impl->world.reset();
	impl->solver.reset();
	impl->broadphase.reset();
	impl->dispatcher.reset();
	impl->collisionConfig.reset();
	// implの解放
	impl.reset();
}

void physics::d3::bulletWorld::Step(const float& deltaTime)
{
	impl->world->stepSimulation(deltaTime);
}

Id3Body* physics::d3::bulletWorld::CreateBody(const Id3BodyDef& bodyDef)
{
	auto body = std::make_unique<bulletBody>();
	body->Create(bodyDef);
	impl->world->addRigidBody(body->GetRigidBody()); // Bulletのワールドにボディを追加
	Id3Body* raw = body.get();
	bodies.emplace_back(std::move(body));
	return raw;
}

void physics::d3::bulletWorld::DestroyBody(Id3Body* body)
{
	if (!body) return;
	bulletBody* bulletBodyPtr = static_cast<bulletBody*>(body);
	impl->world->removeRigidBody(bulletBodyPtr->GetRigidBody());
	bulletBodyPtr->Destroy();
	std::erase_if(bodies, [body](const std::unique_ptr<Id3Body>& ptr) {
		return ptr.get() == body;
		});
}

Id3World* physics::d3::CreateWorld(d3Backend backend)
{
	switch (backend)
	{
	case physics::d3::d3Backend::bullet:
		return new bulletWorld();
		break;
	case physics::d3::d3Backend::chophysics:
		return new chophysicsWorld();
		break;
	default:
		return nullptr; // 未対応のバックエンド
		break;
	}
}

void physics::d3::DestroyWorld(Id3World* world) noexcept
{
	if (world)
	{
		delete world;
	}
}
