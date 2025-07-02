#pragma once
#include "Core/Utility/Components.h"

// 開発中につき基底クラスのEngineは継承していない

// 使用可能 : box2d
// 使用不可 : BulletPhysics,ChoPhysics,ChoPhysics2D

//class PhysicsEngine
//{
//
//};

class GameWorld;
class ContactListener2D : public b2ContactListener
{
public:
	ContactListener2D(ECSManager* ecs, GameWorld* gameWorld):
		m_pECS(ecs), m_pGameWorld(gameWorld)
	{
	}
	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;
private:
	void CollisionEnter(const Entity& self, const Entity& other);
	void CollisionExit(const Entity& self, const Entity& other);

	ECSManager* m_pECS = nullptr;
	GameWorld* m_pGameWorld = nullptr;	
};
class RayCastCallback : public b2RayCastCallback
{
public:
	RayCastCallback(GameWorld* gameWorld, std::string tag) :
		m_pGameWorld(gameWorld), m_Tag(tag)
	{
	}

	float ReportFixture(b2Fixture* a_Fixture, const b2Vec2& a_Point,
		const b2Vec2& a_Normal, float a_Fraction) override;

	bool hit = false;
	b2Vec2 point;
	b2Vec2 normal;
	float fraction = 1.0f;
	b2Fixture* fixture = nullptr;
	b2Body* body = nullptr;
private:
	GameWorld* m_pGameWorld = nullptr;
	std::string m_Tag;
};
