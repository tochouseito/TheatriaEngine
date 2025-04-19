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
		// スクリプトコンテキストを作成
		//ScriptContext selfContext(m_pObjectContainer, m_pInputManager, m_pResourceManager, m_pECS, self);
		//selfContext.Initialize();
		Rigidbody2DComponent* selfRb = m_pECS->GetComponent<Rigidbody2DComponent>(selfObject.GetEntity());
		selfRb->isCollisionStay = true;// 衝突中フラグオン
		//selfRb->otherEntity = other;
		selfRb->otherObjectID = other;
		//ScriptContext otherContext(m_pObjectContainer, m_pInputManager, m_pResourceManager, m_pECS, other);
		//otherContext.Initialize();
		// 相手のゲームオブジェクトを取得
		GameObject& otherObject = m_pObjectContainer->GetGameObject(other);
		if (!otherObject.IsActive()) { return; }
		otherObject.Initialize();// 初期化
		Rigidbody2DComponent* otherRb = m_pECS->GetComponent<Rigidbody2DComponent>(otherObject.GetEntity());
		otherRb->isCollisionStay = true;// 衝突中フラグオン
		//script->onCollisionEnterFunc(selfContext, otherContext);
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
		// スクリプトコンテキストを作成
		//ScriptContext selfContext(m_pObjectContainer, m_pInputManager, m_pResourceManager, m_pECS, self);
		//selfContext.Initialize();
		Rigidbody2DComponent* selfRb = m_pECS->GetComponent<Rigidbody2DComponent>(self);
		selfRb->isCollisionStay = false;// 衝突中フラグオフ
		//selfRb->otherEntity.reset();
		selfRb->otherObjectID = other;
		//ScriptContext otherContext(m_pObjectContainer, m_pInputManager, m_pResourceManager, m_pECS, other);
		//otherContext.Initialize();
		// 相手のゲームオブジェクトを取得
		GameObject& otherObject = m_pObjectContainer->GetGameObject(other);
		if (!otherObject.IsActive()) return;
		otherObject.Initialize();// 初期化
		Rigidbody2DComponent* otherRb = m_pECS->GetComponent<Rigidbody2DComponent>(other);
		otherRb->isCollisionStay = false;// 衝突中フラグオフ
		//script->onCollisionExitFunc(selfContext, otherContext);
		script->onCollisionExitFunc(otherObject);
	}
}
