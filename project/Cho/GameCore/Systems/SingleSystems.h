#pragma once
#include "GameCore/ECS/ECSManager.h"
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "Core/Utility/CompBufferData.h"
#include "GameCore/ScriptAPI/ScriptAPI.h"
#include "Platform/Timer/Timer.h"

class ResourceManager;
class GraphicsEngine;
class GameCoreCommand;
class ScriptContainer;
class ObjectContainer;
class InputManager;

enum SystemPriority
{
	TransformSystem=0,
	CameraSystem,
	ScriptSystem,
};

// Transform初期化
class TransformInitializeSystem : public ECSManager::System<TransformComponent>
{
	friend class GameCore;
public:
	TransformInitializeSystem(ECSManager* ecs)
		: ECSManager::System<TransformComponent>([this](Entity e, TransformComponent& transform)
			{
				e;
				Log::Write(LogLevel::Info, "TransformInitializeSystem");
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
				//priorityUpdate(m_pECS);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager)
	{
		m_pIntegrationBuffer = dynamic_cast<StructuredBuffer<BUFFER_DATA_TF>*>(integrationBuffer);
	}
	~TransformUpdateSystem() = default;
	void Update(ECSManager* ecs) override
	{
		// 一度だけ通る処理
		// 優先度順に更新
		priorityUpdate(ecs);
	}
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
	void Finalize(Entity entity, TransformComponent& transform);
private:
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
// スクリプトインスタンス生成システム
class ScriptGenerateInstanceSystem : public ECSManager::System<ScriptComponent>
{
	friend class GameCore;
public:
	ScriptGenerateInstanceSystem(ObjectContainer* objectContainer, InputManager* input, ECSManager* ecs, ResourceManager* resourceManager)
		: ECSManager::System<ScriptComponent>([this](Entity e, ScriptComponent& script)
			{
				e;
				InstanceGenerate(script);
			}),
		m_ECS(ecs), m_pResourceManager(resourceManager), m_pInputManager(input), m_pObjectContainer(objectContainer)
	{
	}
	~ScriptGenerateInstanceSystem() = default;
private:
	void InstanceGenerate(ScriptComponent& script);
	ECSManager* m_ECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	InputManager* m_pInputManager = nullptr;
	ObjectContainer* m_pObjectContainer = nullptr;
};

// スクリプト初期化システム
class ScriptInitializeSystem : public ECSManager::System<ScriptComponent>
{
	friend class GameCore;
public:
	ScriptInitializeSystem(ObjectContainer* objectContainer,InputManager* input,ECSManager* ecs,ResourceManager* resourceManager)
		: ECSManager::System<ScriptComponent>([this](Entity e, ScriptComponent& script)
			{
				e;
				StartScript(script);
			}),
		m_ECS(ecs), m_pResourceManager(resourceManager), m_pInputManager(input), m_pObjectContainer(objectContainer)
	{
	}
	~ScriptInitializeSystem() = default;
private:
	void StartScript(ScriptComponent& script);
	ECSManager* m_ECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	InputManager* m_pInputManager = nullptr;
	ObjectContainer* m_pObjectContainer = nullptr;
};
// スクリプト更新システム
class ScriptUpdateSystem : public ECSManager::System<ScriptComponent>
{
public:
	ScriptUpdateSystem(ObjectContainer* objectContainer, InputManager* input,ECSManager* ecs,ResourceManager* resourceManager)
		: ECSManager::System<ScriptComponent>([this](Entity e, ScriptComponent& script)
			{
				e;
				UpdateScript(script);
			}),
		m_ECS(ecs), m_pResourceManager(resourceManager), m_pInputManager(input), m_pObjectContainer(objectContainer)
	{
	}
	~ScriptUpdateSystem() = default;
private:
	void UpdateScript(ScriptComponent& script);
	ECSManager* m_ECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	InputManager* m_pInputManager = nullptr;
	ObjectContainer* m_pObjectContainer = nullptr;
};
// スクリプト終了システム
class ScriptFinalizeSystem : public ECSManager::System<ScriptComponent>
{
	friend class GameCore;
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
	friend class GameCore;
public:
	Rigidbody2DInitSystem(ECSManager* ecs, b2World* world)
		: ECSManager::System<TransformComponent, Rigidbody2DComponent>(
			[this](Entity e, TransformComponent& transform, Rigidbody2DComponent& rb)
			{
				e;
				CreateBody(e,transform, rb);
			}),
		m_ECS(ecs), m_World(world)
	{
	}

	~Rigidbody2DInitSystem() = default;
private:
	void CreateBody(Entity e, TransformComponent& transform, Rigidbody2DComponent& rb)
	{
		if (rb.runtimeBody != nullptr) return;
		e;
		b2BodyDef bodyDef;
		bodyDef.userData.pointer = static_cast<uintptr_t>(rb.selfObjectID.value());
		bodyDef.type = rb.bodyType;
		bodyDef.gravityScale = rb.gravityScale;
		bodyDef.fixedRotation = rb.fixedRotation;
		bodyDef.position = b2Vec2(transform.position.x, transform.position.y);
		float angleZ = ChoMath::DegreesToRadians(transform.degrees).z;
		bodyDef.angle = angleZ;
		rb.runtimeBody = m_World->CreateBody(&bodyDef);
		rb.runtimeBody->SetAwake(true);
		rb.world = m_World;
		rb.velocity.Initialize();

		// Transformと同期（optional）
		transform.position.x = rb.runtimeBody->GetPosition().x;
		transform.position.y = rb.runtimeBody->GetPosition().y;
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
		float timeStep = Timer::GetDeltaTime();
		constexpr int velocityIterations = 6;
		constexpr int positionIterations = 2;
		m_World->Step(timeStep, velocityIterations, positionIterations);
	}

	void SyncFromPhysics(TransformComponent& transform, Rigidbody2DComponent& rb)
	{
		if (rb.runtimeBody == nullptr) return;

		if (rb.requestedPosition)
		{
			if (rb.fixedRotation)
			{
				rb.runtimeBody->SetTransform(*rb.requestedPosition, ChoMath::DegreesToRadians(transform.degrees).z);
			} else
			{
				rb.runtimeBody->SetTransform(*rb.requestedPosition, rb.runtimeBody->GetAngle());
			}
			rb.requestedPosition.reset();
		}
		const b2Vec2& pos = rb.runtimeBody->GetPosition();
		transform.position.x = pos.x;
		transform.position.y = pos.y;

		b2Vec2 velocity = rb.runtimeBody->GetLinearVelocity();
		rb.velocity.x = velocity.x;
		rb.velocity.y = velocity.y;

		Vector3 radians = {};
		if (!rb.fixedRotation)
		{
			radians.z = rb.runtimeBody->GetAngle(); // radians
		} else
		{
			radians.z = ChoMath::DegreesToRadians(transform.degrees).z;
		}
		Vector3 degrees = ChoMath::RadiansToDegrees(radians);
		transform.degrees.z = degrees.z;
		
		//Vector3 radians = ChoMath::DegreesToRadians(transform.degrees);
		//radians.z = rb.runtimeBody->GetAngle(); // radians
		//Vector3 diff = radians - transform.preRot;
		//Quaternion qx = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), diff.x);
		//Quaternion qy = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), diff.y);
		//Quaternion qz = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), diff.z);
		//transform.rotation = transform.rotation * qx * qy * qz;//;
		//transform.rotation = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0, 0, 1), angle);


	}

	ECSManager* m_ECS = nullptr;
	b2World* m_World = nullptr;
};
class Rigidbody2DResetSystem : public ECSManager::System<TransformComponent, Rigidbody2DComponent>
{
	friend class GameCore;
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
		rb.isCollisionStay = false;
		rb.otherObjectID.reset();
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
class CollisionSystem : public ECSManager::System<ScriptComponent,Rigidbody2DComponent>
{
public:
	CollisionSystem(ECSManager* ecs, ResourceManager* resourceManager, InputManager* inputManager,ObjectContainer* objectContainer)
		: ECSManager::System<ScriptComponent, Rigidbody2DComponent>(
			[this](Entity e, ScriptComponent& script, Rigidbody2DComponent& rb)
			{
				e;
				//CollisionEnter(script, rb);
				CollisionStay(script, rb);
			}),
		m_ECS(ecs), m_pResourceManager(resourceManager), m_pInputManager(inputManager), m_pObjectContainer(objectContainer)
	{
	}
	~CollisionSystem() = default;
private:
	void CollisionStay(ScriptComponent& script, Rigidbody2DComponent& rb);
	ECSManager* m_ECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	InputManager* m_pInputManager = nullptr;
	ObjectContainer* m_pObjectContainer = nullptr;
};

class BoxCollider2DInitSystem : public ECSManager::System<TransformComponent, Rigidbody2DComponent, BoxCollider2DComponent>
{
	friend class GameCore;
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

		// 面積を計算
		float area = ComputePolygonArea(&shape);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = rb.mass / area;
		fixtureDef.friction = box.friction;
		fixtureDef.restitution = box.restitution;
		fixtureDef.isSensor = box.isSensor;

		box.runtimeFixture = rb.runtimeBody->CreateFixture(&fixtureDef);
	}
	float ComputePolygonArea(const b2PolygonShape* shape)
	{
		float area = 0.0f;
		const int count = shape->m_count;
		const b2Vec2* verts = shape->m_vertices;

		for (int i = 0; i < count; ++i)
		{
			const b2Vec2& a = verts[i];
			const b2Vec2& b = verts[(i + 1) % count];
			area += a.x * b.y - a.y * b.x;
		}
		return 0.5f * std::abs(area);
	}

	ECSManager* m_ECS = nullptr;
	b2World* m_World = nullptr;
};

// BoxCollider2D更新システム
class BoxCollider2DUpdateSystem : public ECSManager::System<TransformComponent, Rigidbody2DComponent, BoxCollider2DComponent>
{
public:
	BoxCollider2DUpdateSystem(ECSManager* ecs, b2World* world)
		: ECSManager::System<TransformComponent, Rigidbody2DComponent, BoxCollider2DComponent>(
			[this](Entity e, TransformComponent& transform, Rigidbody2DComponent& rb, BoxCollider2DComponent& box)
			{
				e;
				UpdateFixture(transform, rb, box);
			}),
		m_ECS(ecs), m_World(world)
	{
	}
	~BoxCollider2DUpdateSystem() = default;
private:
	void UpdateFixture(const TransformComponent& transform, Rigidbody2DComponent& rb, BoxCollider2DComponent& box);
	ECSManager* m_ECS = nullptr;
	b2World* m_World = nullptr;
};

// マテリアルの更新システム
class MaterialUpdateSystem : public ECSManager::System<MaterialComponent>
{
public:
	MaterialUpdateSystem(ECSManager* ecs, ResourceManager* resourceManager, IStructuredBuffer* integrationBuffer)
		: ECSManager::System< MaterialComponent>([this](Entity e, MaterialComponent& material)
			{
				e;
				TransferComponent(material);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager)
	{
		m_pIntegrationBuffer = dynamic_cast<StructuredBuffer<BUFFER_DATA_MATERIAL>*>(integrationBuffer);
	}
	~MaterialUpdateSystem() = default;
private:
	void TransferComponent(const MaterialComponent& material);
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	StructuredBuffer<BUFFER_DATA_MATERIAL>* m_pIntegrationBuffer = nullptr;
};
// エミッター更新システム
class EmitterUpdateSystem : public ECSManager::System<EmitterComponent>
{
public:
	EmitterUpdateSystem(ECSManager* ecs, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine)
		: ECSManager::System<EmitterComponent>([this](Entity e, EmitterComponent& emitter)
			{
				UpdateEmitter(e,emitter);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager), m_pGraphicsEngine(graphicsEngine)
	{
	}
	~EmitterUpdateSystem() = default;
private:
	void UpdateEmitter(Entity e,EmitterComponent& emitter);
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	GraphicsEngine* m_pGraphicsEngine = nullptr;
};

// パーティクル初期化システム
class ParticleInitializeSystem : public ECSManager::System<ParticleComponent>
{
	friend class GameCore;
public:
	ParticleInitializeSystem(ECSManager* ecs, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine)
		: ECSManager::System<ParticleComponent>([this](Entity e, ParticleComponent& particle)
			{
				e;
				InitializeParticle(particle);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager), m_pGraphicsEngine(graphicsEngine)
	{
	}
	~ParticleInitializeSystem() = default;
private:
	void InitializeParticle(ParticleComponent& particle);
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	GraphicsEngine* m_pGraphicsEngine = nullptr;
};

// パーティクル更新システム
class ParticleUpdateSystem : public ECSManager::System<EmitterComponent,ParticleComponent>
{
public:
	ParticleUpdateSystem(ECSManager* ecs, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine)
		: ECSManager::System<EmitterComponent, ParticleComponent>([this](Entity e, EmitterComponent& emitter, ParticleComponent& particle)
			{
				e;
				UpdateParticle(emitter, particle);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager), m_pGraphicsEngine(graphicsEngine)
	{
	}
	~ParticleUpdateSystem() = default;
private:
	void UpdateParticle(EmitterComponent& emitter, ParticleComponent& particle);
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	GraphicsEngine* m_pGraphicsEngine = nullptr;
};

// UI更新システム
class UIUpdateSystem : public ECSManager::System<UISpriteComponent>
{
public:
	UIUpdateSystem(ECSManager* ecs, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine,IStructuredBuffer* integrationBuffer)
		: ECSManager::System<UISpriteComponent>([this](Entity e, UISpriteComponent& uiSprite)
			{
				UpdateUI(e,uiSprite);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager), m_pGraphicsEngine(graphicsEngine)
	{
		m_pIntegrationBuffer = dynamic_cast<StructuredBuffer<BUFFER_DATA_UISPRITE>*>(integrationBuffer);
	}
	~UIUpdateSystem() = default;
private:
	void UpdateUI(Entity e,UISpriteComponent& uiSprite);
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	GraphicsEngine* m_pGraphicsEngine = nullptr;
	StructuredBuffer<BUFFER_DATA_UISPRITE>* m_pIntegrationBuffer = nullptr;
};

// ライト更新システム
class LightUpdateSystem : public ECSManager::System<LightComponent,TransformComponent>
{
public:
	LightUpdateSystem(ECSManager* ecs, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine)
		: ECSManager::System<LightComponent, TransformComponent>([this](Entity e, LightComponent& light,TransformComponent& transform)
			{
				UpdateLight(e, light, transform);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager), m_pGraphicsEngine(graphicsEngine)
	{
	}
	~LightUpdateSystem() = default;
private:
	void UpdateLight(Entity e, LightComponent& light,TransformComponent& transform);
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	GraphicsEngine* m_pGraphicsEngine = nullptr;
};
struct ModelData;
// アニメーション初期化システム
class AnimationInitializeSystem : public ECSManager::System<AnimationComponent>
{
	public:
	AnimationInitializeSystem(ECSManager* ecs, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine)
		: ECSManager::System<AnimationComponent>([this](Entity e, AnimationComponent& animation)
			{
				e;
				InitializeAnimation(animation);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager), m_pGraphicsEngine(graphicsEngine)
	{
	}
	~AnimationInitializeSystem() = default;
private:
	void InitializeAnimation(AnimationComponent& animation);
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	GraphicsEngine* m_pGraphicsEngine = nullptr;
};
// アニメーション更新システム
class AnimationUpdateSystem : public ECSManager::System<AnimationComponent>
{
	public:
	AnimationUpdateSystem(ECSManager* ecs, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine)
		: ECSManager::System<AnimationComponent>([this](Entity e, AnimationComponent& animation)
			{
				e;
				UpdateAnimation(animation);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager), m_pGraphicsEngine(graphicsEngine)
	{
	}
	~AnimationUpdateSystem() = default;
private:
	void UpdateAnimation(AnimationComponent& animation);

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, const float& time);
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, const float& time);
	Scale CalculateValue(const std::vector<KeyframeScale>& keyframes, const float& time);
	void timeUpdate(AnimationComponent& animation, ModelData* model);
	void ApplyAnimation(AnimationComponent& animation, ModelData* model);
	void SkeletonUpdate(AnimationComponent& animation, ModelData* model);
	void SkinClusterUpdate(AnimationComponent& animation, ModelData* model);
	void ApplySkinning(AnimationComponent& animation, ModelData* model);
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	GraphicsEngine* m_pGraphicsEngine = nullptr;
};	
// アニメーション終了システム
class AnimationFinalizeSystem : public ECSManager::System<AnimationComponent>
{
	public:
	AnimationFinalizeSystem(ECSManager* ecs, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine)
		: ECSManager::System<AnimationComponent>([this](Entity e, AnimationComponent& animation)
			{
				e;
				FinalizeAnimation(animation);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager), m_pGraphicsEngine(graphicsEngine)
	{
	}
	~AnimationFinalizeSystem() = default;
private:
	void FinalizeAnimation(AnimationComponent& animation);
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	GraphicsEngine* m_pGraphicsEngine = nullptr;
};

