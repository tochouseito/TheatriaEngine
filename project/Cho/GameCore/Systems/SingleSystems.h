#pragma once
#include "GameCore/ECS/ECSManager.h"
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "Core/Utility/CompBufferData.h"
#include "GameCore/ScriptAPI/ScriptAPI.h"

class ResourceManager;
class GameCoreCommand;
class ScriptContainer;

enum SystemPriority
{
	TransformSystem=0,
	CameraSystem,
	ScriptSystem,
};

// Transform初期化
class TransformInitializeSystem : public ECSManager::System<TransformComponent>
{
public:
	TransformInitializeSystem(ECSManager* ecs)
		: ECSManager::System<TransformComponent>([this](Entity e, TransformComponent& transform)
			{
				e;
				Start(transform);
			}),
		m_ECS(ecs)
	{
	}
	~TransformInitializeSystem() = default;
private:
	void Start(TransformComponent& transform);
	ECSManager* m_ECS = nullptr;
};
// Transform更新
class TransformUpdateSystem : public ECSManager::System<TransformComponent>
{
public:
	TransformUpdateSystem(ECSManager* ecs, ResourceManager* resourceManager, IStructuredBuffer* integrationBuffer)
		: ECSManager::System<TransformComponent>([this](Entity e, TransformComponent& transform)
			{
				e;
				transform;
				priorityUpdate(m_pECS);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager)
	{
		m_pIntegrationBuffer = dynamic_cast<StructuredBuffer<BUFFER_DATA_TF>*>(integrationBuffer);
	}
	~TransformUpdateSystem() = default;

private:
	// 優先度順に更新する
	void priorityUpdate(ECSManager* ecs);
	// 更新
	void UpdateComponent(Entity e, TransformComponent& transform);
	// 転送
	void TransferMatrix(TransformComponent& transform);

	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	StructuredBuffer<BUFFER_DATA_TF>* m_pIntegrationBuffer = nullptr;
};
// Transform終了処理
class TransformFinalizeSystem : public ECSManager::System<TransformComponent>
{
public:
	TransformFinalizeSystem(ECSManager* ecs)
		: ECSManager::System<TransformComponent>([this](Entity e, TransformComponent& transform)
			{
				e;
				Finalize(e,transform);
			}),
		m_ECS(ecs)
	{
	}
	~TransformFinalizeSystem() = default;
private:
	void Finalize(Entity entity,TransformComponent& transform);
	ECSManager* m_ECS = nullptr;
};
// Camera更新System
class CameraUpdateSystem : public ECSManager::System<TransformComponent, CameraComponent>
{
public:
	CameraUpdateSystem(ECSManager* ecs, ResourceManager* resourceManager, IStructuredBuffer* integrationBuffer)
		: ECSManager::System<TransformComponent, CameraComponent>([this](Entity e, TransformComponent& transform, CameraComponent& camera)
			{
				e;
				UpdateMatrix(transform, camera);
			}),
		m_ECS(ecs), m_pResourceManager(resourceManager)
	{
		m_pIntegrationBuffer = dynamic_cast<StructuredBuffer<BUFFER_DATA_TF>*>(integrationBuffer);
	}
	~CameraUpdateSystem() = default;
private:
	void UpdateMatrix(TransformComponent& transform, CameraComponent& camera);
	void TransferMatrix(TransformComponent& transform, CameraComponent& camera);

	ECSManager* m_ECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	StructuredBuffer<BUFFER_DATA_TF>* m_pIntegrationBuffer = nullptr;
};
// スクリプト初期化システム
class ScriptInitializeSystem : public ECSManager::System<ScriptComponent>
{
public:
	ScriptInitializeSystem(ECSManager* ecs,ResourceManager* resourceManager)
		: ECSManager::System<ScriptComponent>([this](Entity e, ScriptComponent& script)
			{
				e;
				Start(script);
			}),
		m_ECS(ecs), m_pResourceManager(resourceManager)
	{
	}
	~ScriptInitializeSystem() = default;
private:
	void LoadScript(ScriptComponent& script);
	void Start(ScriptComponent& script);
	void StartScript(ScriptComponent& script);
	ScriptContext MakeScriptContext(Entity entity);
	ECSManager* m_ECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
};
// スクリプト更新システム
class ScriptUpdateSystem : public ECSManager::System<ScriptComponent>
{
public:
	ScriptUpdateSystem(ECSManager* ecs,ResourceManager* resourceManager)
		: ECSManager::System<ScriptComponent>([this](Entity e, ScriptComponent& script)
			{
				e;
				UpdateScript(script);
			}),
		m_ECS(ecs), m_pResourceManager(resourceManager)
	{
	}
	~ScriptUpdateSystem() = default;
private:
	void UpdateScript(ScriptComponent& script);
	ScriptContext MakeScriptContext(Entity entity);
	ECSManager* m_ECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
};
// スクリプト終了システム
class ScriptFinalizeSystem : public ECSManager::System<ScriptComponent>
{
public:
	ScriptFinalizeSystem(ECSManager* ecs)
		: ECSManager::System<ScriptComponent>([this](Entity e, ScriptComponent& script)
			{
				e;
				FinalizeScript(script);
			}),
		m_ECS(ecs)
	{
	}
	~ScriptFinalizeSystem() = default;
private:
	void FinalizeScript(ScriptComponent& script);
	ECSManager* m_ECS = nullptr;
};

class Rigidbody2DInitSystem : public ECSManager::System<TransformComponent, Rigidbody2DComponent>
{
public:
	Rigidbody2DInitSystem(ECSManager* ecs, b2World* world)
		: ECSManager::System<TransformComponent, Rigidbody2DComponent>(
			[this](Entity e, TransformComponent& transform, Rigidbody2DComponent& rb)
			{
				e;
				CreateBody(transform, rb);
			}),
		m_ECS(ecs), m_World(world)
	{
	}

	~Rigidbody2DInitSystem() = default;

private:
	void CreateBody(TransformComponent& transform, Rigidbody2DComponent& rb)
	{
		if (rb.runtimeBody != nullptr) return;

		b2BodyDef bodyDef;
		bodyDef.type = rb.bodyType;
		bodyDef.gravityScale = rb.gravityScale;
		bodyDef.fixedRotation = rb.fixedRotation;
		bodyDef.position = b2Vec2(transform.translation.x, transform.translation.y);
		rb.runtimeBody = m_World->CreateBody(&bodyDef);
		rb.runtimeBody->SetAwake(true);

		// Transformと同期（optional）
		transform.translation.x = rb.runtimeBody->GetPosition().x;
		transform.translation.y = rb.runtimeBody->GetPosition().y;
	}

	ECSManager* m_ECS = nullptr;
	b2World* m_World = nullptr;
};

class Rigidbody2DUpdateSystem : public ECSManager::System<TransformComponent, Rigidbody2DComponent>
{
public:
	Rigidbody2DUpdateSystem(ECSManager* ecs, b2World* world)
		: ECSManager::System<TransformComponent, Rigidbody2DComponent>(
			[this](Entity e, TransformComponent& transform, Rigidbody2DComponent& rb)
			{
				e;
				SyncFromPhysics(transform, rb);
			}),
		m_ECS(ecs), m_World(world)
	{
	}

	~Rigidbody2DUpdateSystem() = default;

	void Update(ECSManager* ecs) override
	{
		// Step は一回だけ呼ぶ（エンティティループより前）
		StepSimulation();

		// いつもの処理（b2Body -> Transform同期）
		ECSManager::System<TransformComponent, Rigidbody2DComponent>::Update(ecs);
	}
private:
	void StepSimulation()
	{
		constexpr float timeStep = 1.0f / 60.0f;
		constexpr int velocityIterations = 6;
		constexpr int positionIterations = 2;
		m_World->Step(timeStep, velocityIterations, positionIterations);
	}

	void SyncFromPhysics(TransformComponent& transform, Rigidbody2DComponent& rb)
	{
		if (rb.runtimeBody == nullptr) return;

		const b2Vec2& pos = rb.runtimeBody->GetPosition();
		transform.translation.x = pos.x;
		transform.translation.y = pos.y;

		float angle = rb.runtimeBody->GetAngle(); // radians
		transform.rotation = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0, 0, 1), angle);
	}

	ECSManager* m_ECS = nullptr;
	b2World* m_World = nullptr;
};
class Rigidbody2DResetSystem : public ECSManager::System<TransformComponent, Rigidbody2DComponent>
{
public:
	Rigidbody2DResetSystem(ECSManager* ecs, b2World* world)
		: ECSManager::System<TransformComponent, Rigidbody2DComponent>(
			[this](Entity e, TransformComponent& transform, Rigidbody2DComponent& rb)
			{
				e;
				Reset(transform, rb);
				ResetCollider<BoxCollider2DComponent>(e);
				//ResetCollider<CircleCollider2DComponent>(e);
			}),
		m_ECS(ecs), m_World(world)
	{
	}

	~Rigidbody2DResetSystem() = default;

private:
	void Reset(TransformComponent& transform, Rigidbody2DComponent& rb)
	{
		transform;
		// Bodyがあるなら削除
		if (rb.runtimeBody)
		{
			m_World->DestroyBody(rb.runtimeBody);
			rb.runtimeBody = nullptr;
		}
	}
	template<typename ColliderT>
	void ResetCollider(Entity e)
	{
		ColliderT* col = m_ECS->GetComponent<ColliderT>(e);
		if (col && col->runtimeFixture)
		{
			col->runtimeFixture = nullptr;
		}
	}

	ECSManager* m_ECS = nullptr;
	b2World* m_World = nullptr;
};
class BoxCollider2DInitSystem : public ECSManager::System<TransformComponent, Rigidbody2DComponent, BoxCollider2DComponent>
{
public:
	BoxCollider2DInitSystem(ECSManager* ecs, b2World* world)
		: ECSManager::System<TransformComponent, Rigidbody2DComponent, BoxCollider2DComponent>(
			[this](Entity e, TransformComponent& transform, Rigidbody2DComponent& rb, BoxCollider2DComponent& box)
			{
				e;
				CreateFixture(transform, rb, box);
			}),
		m_ECS(ecs), m_World(world)
	{
	}

	~BoxCollider2DInitSystem() = default;

private:
	void CreateFixture(const TransformComponent& transform, Rigidbody2DComponent& rb, BoxCollider2DComponent& box)
	{
		transform;
		if (!rb.runtimeBody || box.runtimeFixture != nullptr) return;

		b2PolygonShape shape;
		shape.SetAsBox(box.width / 2.0f, box.height / 2.0f, b2Vec2(box.offsetX, box.offsetY), 0.0f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = box.density;
		fixtureDef.friction = box.friction;
		fixtureDef.restitution = box.restitution;

		box.runtimeFixture = rb.runtimeBody->CreateFixture(&fixtureDef);
	}

	ECSManager* m_ECS = nullptr;
	b2World* m_World = nullptr;
};