#include "PrecompiledHeader.h"
#include "SystemManager.h"
#include"Graphics/Pipeline/Pipeline.h"

SystemManager::~SystemManager()
{
    //scriptSystem.Cleanup(*entityManager_,*componentManager_);
}

void SystemManager::Initialize(D3DCommand* d3dCommand, DrawExecution* drawExe, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem, TextureLoader* texLoader, EntityManager* entityManager, ComponentManager* componentManager)
{
    spriteSystem.SetPtrs(rvManager, texLoader);
    particleSystem.SetPtrs(d3dCommand, rvManager, graphicsSystem);
    animationSystem.SetPtrs(d3dCommand, rvManager, graphicsSystem);
    effectSystem.SetPtrs(d3dCommand, rvManager, graphicsSystem);
	renderSystem.SetDrawExecution(drawExe);
    lightSystem.SetPtrs(rvManager);
    entityManager_ = entityManager;
    componentManager_ = componentManager;
}

void SystemManager::Start(EntityManager& entityManager, ComponentManager& componentManager)
{
    //scriptSystem.Start(entityManager, componentManager);
    transformSystem.Initialize(entityManager, componentManager);
    particleSystem.Initialize(entityManager, componentManager);
}

void SystemManager::Update(EntityManager& entityManager, ComponentManager& componentManager, float deltaTime) {
    if (run) {
        //scriptSystem.Update(entityManager, componentManager);
        cameraSystem.Update(entityManager, componentManager);
        materialSystem.Update(entityManager, componentManager);
        physicsSystem.Update(entityManager, componentManager);
        transformSystem.Update(entityManager, componentManager);
        emitterSystem.Update(entityManager, componentManager);
        particleSystem.Update(entityManager, componentManager);
		effectSystem.Update(entityManager, componentManager);
		animationSystem.Update(entityManager, componentManager);
        spriteSystem.Update(entityManager, componentManager);
		lightSystem.Update(entityManager, componentManager);
    } else {
        cameraSystem.UpdateEditor(entityManager, componentManager);
        materialSystem.Update(entityManager, componentManager);
        transformSystem.UpdateEditor(entityManager, componentManager);
        emitterSystem.Update(entityManager, componentManager);
        particleSystem.Update(entityManager, componentManager);
        spriteSystem.Update(entityManager, componentManager);
    }
    deltaTime;
}

void SystemManager::GBufferDraw(EntityManager& entityManager, ComponentManager& componentManager, D3DCommand* d3dCommand, ResourceViewManager* rvManager,DSVManager* dsvManager, GraphicsSystem* graphicsSystem, TextureLoader* texLoad)
{
    renderSystem.GBufferRender(PSOMode::GBuffer, entityManager, componentManager, d3dCommand, rvManager, dsvManager,graphicsSystem, texLoad);
}

void SystemManager::GBufferMixDraw(EntityManager& entityManager, ComponentManager& componentManager, D3DCommand* d3dCommand, ResourceViewManager* rvManager,DSVManager* dsvManager, GraphicsSystem* graphicsSystem, TextureLoader* texLoad)
{
    //renderSystem.GBufferRender(PSOMode::GBufferMix, entityManager, componentManager, d3dCommand, rvManager, dsvManager, graphicsSystem, texLoad);
    renderSystem.GBufferRender(PSOMode::Dif_ToonLighting, entityManager, componentManager, d3dCommand, rvManager, dsvManager,graphicsSystem, texLoad);
}

void SystemManager::Draw(
    EntityManager& entityManager,
    ComponentManager& componentManager,
    D3DCommand* d3dCommand,
    ResourceViewManager* rvManager,
    GraphicsSystem* graphicsSystem,
    TextureLoader* texLoad
)
{
    renderSystem.Render(PSOMode::Demo,entityManager, componentManager,d3dCommand,rvManager,graphicsSystem,texLoad);
}
void SystemManager::PostDraw(EntityManager& entityManager, ComponentManager& componentManager, D3DCommand* d3dCommand, ResourceViewManager* rvManager)
{
	renderSystem.PostRender(entityManager, componentManager, d3dCommand, rvManager);
}
void SystemManager::DebugCameraInit(CameraComponent& camera)
{
	cameraSystem.InitDebugCamera(camera);
}
void SystemManager::DebugCameraUpdate(CameraComponent& camera)
{
    cameraSystem.UpdateDebugCamera(camera);
}
void SystemManager::DebugGBufferMixDraw(EntityManager& entityManager, ComponentManager& componentManager, D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem, TextureLoader* texLoad, const CameraComponent& camera)
{
	renderSystem.DebugRenderGBufferMix(entityManager, componentManager, d3dCommand, rvManager, graphicsSystem, texLoad,camera);
}
void SystemManager::DebugDraw(
    EntityManager& entityManager,
    ComponentManager& componentManager,
    D3DCommand* d3dCommand,
    ResourceViewManager* rvManager,
    GraphicsSystem* graphicsSystem,
    TextureLoader* texLoad,
    const CameraComponent& camera
)
{
    renderSystem.DebugRender(entityManager, componentManager, d3dCommand, rvManager, graphicsSystem, texLoad,camera);
}

void SystemManager::LoadScripts()
{
    //scriptSystem.LoadScripts(*entityManager_, *componentManager_);
}

void SystemManager::UnLoadScripts()
{
    //scriptSystem.UnLoadDLL(*entityManager_, *componentManager_);
}
