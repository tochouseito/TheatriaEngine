#pragma once

// ComponentManager
#include"ECS/ComponentManager/ComponentManager.h"

#include"PSOMode.h"

class D3DCommand;
class ResourceViewManager;
class DSVManager;
class DrawExecution;
class GraphicsSystem;
class TextureLoader;
class RenderSystem{
public:
	void GBufferRender(
		PSOMode psoMode,
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		DSVManager* dsvManager,
		GraphicsSystem* graphicsSystem,
		TextureLoader* texLoad
	);

	void Render(
		PSOMode psoMode,
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		GraphicsSystem* graphicsSystem,
		TextureLoader* texLoad
	);

	void DebugRenderGBufferMix(
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		GraphicsSystem* graphicsSystem,
		TextureLoader* texLoad,
		const CameraComponent& camera
	);

	void DebugRender(
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		GraphicsSystem* graphicsSystem,
		TextureLoader* texLoad,
		const CameraComponent& camera
	);

	void PostRender(
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager
	);

	void SetDrawExecution(DrawExecution* drawExecution) { drawExecution_ = drawExecution; }

private:
	void ObjectRender(
		PSOMode psoMode,
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		DSVManager* dsvManager,
		GraphicsSystem* graphicsSystem,
		TextureLoader* texLoad
	);

	void SpriteRender(
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		GraphicsSystem* graphicsSystem,
		TextureLoader* texLoad
	);

	void ParticleRender(
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		GraphicsSystem* graphicsSystem,
		TextureLoader* texLoad
	);

	void EffectRender(
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		GraphicsSystem* graphicsSystem,
		TextureLoader* texLoad
	);

	void MeshShaderRenderTest(
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		GraphicsSystem* graphicsSystem,
		TextureLoader* texLoad
	);

	void ColliderRender(
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		GraphicsSystem* graphicsSystem,
		const CameraComponent& camera
	);

	void DrawLineRender(
		Vector3& start, Vector3& end,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		GraphicsSystem* graphicsSystem,
		const CameraComponent& camera
	);

	void DebugLinesRender(
		std::vector<LineVertex>& lineVertex,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		GraphicsSystem* graphicsSystem,
		const CameraComponent& camera
	);

	void DrawGrid(
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		GraphicsSystem* graphicsSystem,
		const CameraComponent& camera
	);

	void DebugDrawEffect(
		EntityManager& entityManager,
		ComponentManager& componentManager,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		GraphicsSystem* graphicsSystem,
		TextureLoader* texLoad,
		const CameraComponent& camera
	);

	void LoopEffectNodeDraw(
		EffectNode& effectNode,
		EffectComponent& effect,
		D3DCommand* d3dCommand,
		ResourceViewManager* rvManager,
		GraphicsSystem* graphicsSystem,
		TextureLoader* texLoad,
		const CameraComponent& camera
	);

private:
	DrawExecution* drawExecution_=nullptr;
};

