#include "pch.h"
#include "PhysicsEngine.h"
#include "GameCore/ScriptAPI/ScriptAPI.h"
#include "GameCore/ECS/ECSManager.h"

void ContactListener2D::BeginContact(b2Contact* contact)
{
	Entity entityA = static_cast<Entity>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	Entity entityB = static_cast<Entity>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

	CollisionEnter(entityA, entityB);
	CollisionEnter(entityB, entityA);
}

void ContactListener2D::EndContact(b2Contact* contact)
{
	Entity entityA = static_cast<Entity>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	Entity entityB = static_cast<Entity>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

	CollisionExit(entityA, entityB);
	CollisionExit(entityB, entityA);
}

void ContactListener2D::CollisionEnter(Entity self, Entity other)
{
	ScriptComponent* script = m_pECS->GetComponent<ScriptComponent>(self);
	if (script && script->isActive)
	{
		// スクリプトコンテキストを作成
		ScriptContext selfContext(m_pObjectContainer, m_pInputManager, m_pResourceManager, m_pECS, self);
		selfContext.Initialize();
		Rigidbody2DComponent* selfRb = m_pECS->GetComponent<Rigidbody2DComponent>(self);
		selfRb->isCollisionStay = true;// 衝突中フラグオン
		selfRb->otherEntity = other;
		ScriptContext otherContext(m_pObjectContainer, m_pInputManager, m_pResourceManager, m_pECS, other);
		otherContext.Initialize();
		Rigidbody2DComponent* otherRb = m_pECS->GetComponent<Rigidbody2DComponent>(other);
		otherRb->isCollisionStay = true;// 衝突中フラグオン
		script->onCollisionEnterFunc(selfContext, otherContext);
	}
}

void ContactListener2D::CollisionExit(Entity self, Entity other)
{
	ScriptComponent* script = m_pECS->GetComponent<ScriptComponent>(self);
	if (script && script->isActive)
	{
		// スクリプトコンテキストを作成
		ScriptContext selfContext(m_pObjectContainer, m_pInputManager, m_pResourceManager, m_pECS, self);
		selfContext.Initialize();
		Rigidbody2DComponent* selfRb = m_pECS->GetComponent<Rigidbody2DComponent>(self);
		selfRb->isCollisionStay = false;// 衝突中フラグオフ
		selfRb->otherEntity.reset();
		ScriptContext otherContext(m_pObjectContainer, m_pInputManager, m_pResourceManager, m_pECS, other);
		otherContext.Initialize();
		Rigidbody2DComponent* otherRb = m_pECS->GetComponent<Rigidbody2DComponent>(other);
		otherRb->isCollisionStay = false;// 衝突中フラグオフ
		script->onCollisionExitFunc(selfContext, otherContext);
	}
}
