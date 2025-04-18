#include "pch.h"
#include "PhysicsEngine.h"
#include "GameCore/ScriptAPI/ScriptAPI.h"
#include "GameCore/ECS/ECSManager.h"

void ContactListener2D::BeginContact(b2Contact* contact)
{
	Entity entityA = static_cast<Entity>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	Entity entityB = static_cast<Entity>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

	NotifyCollision(entityA, entityB);
	NotifyCollision(entityB, entityA);
}

void ContactListener2D::NotifyCollision(Entity self, Entity other)
{
	ScriptComponent* script = m_pECS->GetComponent<ScriptComponent>(self);
	if (script && script->isActive)
	{
		// スクリプトコンテキストを作成
		ScriptContext selfContext(m_pObjectContainer, m_pInputManager, m_pResourceManager, m_pECS, self);
		selfContext.Initialize();
		ScriptContext otherContext(m_pObjectContainer, m_pInputManager, m_pResourceManager, m_pECS, other);
		otherContext.Initialize();
		script->onCollisionEnterFunc(selfContext, otherContext);
	}
}
