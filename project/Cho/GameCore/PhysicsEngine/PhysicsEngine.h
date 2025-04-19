#pragma once
#include "Core/Utility/Components.h"

// 開発中につき基底クラスのEngineは継承していない

// 使用可能 : box2d
// 使用不可 : BulletPhysics,ChoPhysics,ChoPhysics2D

//class PhysicsEngine
//{
//
//};

class ECSManager;
class ResourceManager;
class InputManager;
class ObjectContainer;
class ContactListener2D : public b2ContactListener
{
public:
	ContactListener2D(ECSManager* ecs, ResourceManager* resourceManager, InputManager* inputManager, ObjectContainer* objectContainer)
		: m_pResourceManager(resourceManager), m_pInputManager(inputManager), m_pObjectContainer(objectContainer), m_pECS(ecs)
	{
	}
	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;
private:
	void CollisionEnter(ObjectID self, ObjectID other);
	void CollisionExit(ObjectID self, ObjectID other);

	ECSManager* m_pECS = nullptr; // 外部から渡す or シングルトンで取得
	ResourceManager* m_pResourceManager = nullptr;
	InputManager* m_pInputManager = nullptr;
	ObjectContainer* m_pObjectContainer = nullptr;
};
