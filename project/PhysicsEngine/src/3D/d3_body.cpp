#include "physicsengine_pch.h"
#include "3D/d3_body.h"

// BulletPhysics
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

using namespace physics::d3;

struct bulletBody::Impl
{
	std::unique_ptr<btCollisionShape> shape; // Bulletの衝突形状データ
	std::unique_ptr<btDefaultMotionState> motionState; // 物理オブジェクトの状態
	std::unique_ptr<btRigidBody> rigidBody; // 剛体データ
};

physics::d3::bulletBody::bulletBody():
	impl(std::make_unique<Impl>())
{
}

void physics::d3::bulletBody::Create(const Id3BodyDef& bodyDef)
{
	impl->shape = std::make_unique<btBoxShape>(btVector3(bodyDef.halfsize.x, bodyDef.halfsize.y, bodyDef.halfsize.z));
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(bodyDef.position.x, bodyDef.position.y, bodyDef.position.z));
	impl->motionState = std::make_unique<btDefaultMotionState>(transform);
	btScalar mass = bodyDef.mass; // 質量を設定
	btVector3 inertia(0, 0, 0);
	if(mass != 0.0f)
	{
		impl->shape->calculateLocalInertia(mass, inertia);
	}
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, impl->motionState.get(), impl->shape.get(), inertia);
	impl->rigidBody = std::make_unique<btRigidBody>(rbInfo);
	impl->rigidBody->setFriction(bodyDef.friction);
	impl->rigidBody->setRestitution(bodyDef.restitution);
	impl->rigidBody->setUserPointer(bodyDef.userData); // ユーザーデータを設定
	impl->rigidBody->setUserIndex(bodyDef.userIndex); // ユーザーインデックスを設定
}

void physics::d3::bulletBody::Destroy()
{
	impl->motionState.reset(); // モーションステートの解放
	impl->rigidBody.reset(); // 剛体の解放
	impl->shape.reset(); // 衝突形状の解放
}

Vector3 physics::d3::bulletBody::GetPosition() const
{
	btTransform transform;
	impl->rigidBody->getMotionState()->getWorldTransform(transform);
	return Vector3(transform.getOrigin().x(), transform.getOrigin().y(), transform.getOrigin().z());
}

Quaternion physics::d3::bulletBody::GetRotation() const
{
	btTransform transform;
	impl->rigidBody->getMotionState()->getWorldTransform(transform);
	return Quaternion(transform.getRotation().x(), transform.getRotation().y(), transform.getRotation().z(), transform.getRotation().w());
}

void physics::d3::bulletBody::SetTransform(const Vector3& position, const Quaternion& rotation)
{
	btTransform transform = impl->rigidBody->getWorldTransform();
	transform.setOrigin(btVector3(position.x, position.y, position.z));
	transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
	impl->rigidBody->getMotionState()->setWorldTransform(transform);
	impl->rigidBody->setWorldTransform(transform); // 剛体のワールド変換を更新
}

void physics::d3::bulletBody::SetTransform(const Vector3& position)
{
	btTransform transform = impl->rigidBody->getWorldTransform();
	transform.setOrigin(btVector3(position.x, position.y, position.z));
	impl->rigidBody->getMotionState()->setWorldTransform(transform);
	impl->rigidBody->setWorldTransform(transform); // 剛体のワールド変換を更新
}

Vector3 physics::d3::bulletBody::GetLinearVelocity() const
{
	btVector3 linVel = impl->rigidBody->getLinearVelocity();
	return Vector3(linVel.x(), linVel.y(), linVel.z());
}

void physics::d3::bulletBody::SetLinearVelocity(const Vector3& velocity)
{
	impl->rigidBody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

Vector3 physics::d3::bulletBody::GetAngularVelocity() const 
{ 
	btVector3 angVel = impl->rigidBody->getAngularVelocity();
	return Vector3(angVel.x(), angVel.y(), angVel.z());
}

void physics::d3::bulletBody::SetAngularVelocity(const Vector3& angularVelocity)
{
	impl->rigidBody->setAngularVelocity(btVector3(angularVelocity.x, angularVelocity.y, angularVelocity.z));
}

btRigidBody* physics::d3::bulletBody::GetRigidBody() const { return impl->rigidBody.get(); }

// Bulletの剛体データを取得
bool physics::d3::bulletBody::IsActive() const { return impl->rigidBody->isActive(); }

// 有効かどうかを取得
void physics::d3::bulletBody::SetActive(bool active) { impl->rigidBody->activate(active); }

void physics::d3::bulletBody::SetKinematic(bool isKinematic)
{
	if(isKinematic)
	{
		impl->rigidBody->setCollisionFlags(impl->rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		impl->rigidBody->setActivationState(DISABLE_DEACTIVATION); // 非アクティブ化を無効にする
	}
	else
	{
		impl->rigidBody->setCollisionFlags(impl->rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		impl->rigidBody->setActivationState(ACTIVE_TAG); // アクティブ化状態に戻す
	}
}

void physics::d3::bulletBody::SetSensor(bool isSensor)
{
	// センサーオブジェクトの設定
	if (isSensor)
	{
		impl->rigidBody->setCollisionFlags(impl->rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
	else
	{
		impl->rigidBody->setCollisionFlags(impl->rigidBody->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
}

void physics::d3::chophysicsBody::Create(const Id3BodyDef&)
{
}

void physics::d3::chophysicsBody::Destroy()
{
}

Vector3 physics::d3::chophysicsBody::GetPosition() const
{
	return Vector3();
}

Quaternion physics::d3::chophysicsBody::GetRotation() const
{
	return Quaternion();
}

Vector3 physics::d3::chophysicsBody::GetLinearVelocity() const
{
	return Vector3();
}

void physics::d3::chophysicsBody::SetLinearVelocity(const Vector3&)
{
}

bool physics::d3::chophysicsBody::IsActive() const
{
	return false;
}
