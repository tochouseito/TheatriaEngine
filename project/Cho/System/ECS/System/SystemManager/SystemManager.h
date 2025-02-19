#pragma once

// ComponentManager
#include"ECS/ComponentManager/ComponentManager.h"

#include"ECS/System/PhysicsSystem/PhysicsSystem.h"
#include"ECS/System/RenderSystem/RenderSystem.h"
#include"ECS/System/CameraSystem/CameraSystem.h"
#include"ECS/System/ScriptSystem/ScriptSystem.h"
#include"ECS/System/TransformSystem/TransformSystem.h"
#include"ECS/System/SpriteSystem/SpriteSystem.h"
#include"ECS/System/MaterialSystem/MaterialSystem.h"
#include"ECS/System/ParticleSystem/ParticleSystem.h"
#include"ECS/System/EmitterSystem/EmitterSystem.h"
#include"ECS/System/AnimationSystem/AnimationSystem.h"
#include"ECS/System/LightSystem/LightSystem.h"
#include"ECS/System/EffectSystem/EffectSystem.h"

#include"PSOMode.h"

class D3DCommand;
class ResourceViewManager;
class DSVManager;
class DrawExecution;
class GraphicsSystem;
class TextureLoader;

class SystemManager {
public:
    // デストラクタ
    ~SystemManager();

    void Initialize(D3DCommand* d3dCommand,DrawExecution* drawExe,ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem,TextureLoader* texLoader,EntityManager* entityManager, ComponentManager* componentManager);

    void Start(EntityManager& entityManager, ComponentManager& componentManager);

    void Update(EntityManager& entityManager, ComponentManager& componentManager, float deltaTime);

	void GBufferDraw(
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		DSVManager* dsvManager,
		GraphicsSystem* graphicsSystem,
		TextureLoader* texLoad
	);

	void GBufferMixDraw(
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		DSVManager* dsvManager,
		GraphicsSystem* graphicsSystem,
		TextureLoader* texLoad
	);

    void Draw(
        EntityManager& entityManager,
        ComponentManager& componentManager,
        D3DCommand* d3dCommand,
        ResourceViewManager* rvManager,
        GraphicsSystem* graphicsSystem,
        TextureLoader* texLoad
    );

    void PostDraw(
        EntityManager& entityManager,
        ComponentManager& componentManager,
        D3DCommand* d3dCommand,
        ResourceViewManager* rvManager
    );

    // デバッグカメラ

	void DebugCameraInit(
		CameraComponent& camera
	);

    void DebugCameraUpdate(
        CameraComponent& camera
    );

	void DebugGBufferMixDraw(
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		GraphicsSystem* graphicsSystem,
		TextureLoader* texLoad,
		const CameraComponent& camera
	);

    void DebugDraw(
        EntityManager& entityManager,
        ComponentManager& componentManager,
        D3DCommand* d3dCommand,
        ResourceViewManager* rvManager,
        GraphicsSystem* graphicsSystem,
        TextureLoader* texLoad,
        const CameraComponent& camera
    );

    void PlaySwitch() { run = !run; }
    bool IsPlay() { return run; }

    void LoadScripts();
    void UnLoadScripts();
private:
    PhysicsSystem physicsSystem;
    RenderSystem renderSystem;
    CameraSystem cameraSystem;
    ScriptSystem scriptSystem;
    TransformSystem transformSystem;
    SpriteSystem spriteSystem;
    MaterialSystem materialSystem;
    ParticleSystem particleSystem;
    EmitterSystem emitterSystem;
	AnimationSystem animationSystem;
	LightSystem lightSystem;
	EffectSystem effectSystem;

    EntityManager* entityManager_ = nullptr;
    ComponentManager* componentManager_ = nullptr;

    bool run = true;
};

