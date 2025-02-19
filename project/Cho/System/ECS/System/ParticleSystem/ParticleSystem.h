#pragma once

// ComponentManager
#include"ECS/ComponentManager/ComponentManager.h"

class D3DCommand;
class ResourceViewManager;
class GraphicsSystem;
class ParticleSystem
{
public:
	void Initialize(EntityManager& entityManager, ComponentManager& componentManager);

	void Update(EntityManager& entityManager, ComponentManager& componentManager);

	void SetPtrs(D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem);
private:
	void InitMatrix(ParticleComponent& particle);

	// 更新
	void UpdateMatrix(ParticleComponent& particle,EmitterComponent& emitter);

	void TransferMatrix(ParticleComponent& particle);

private:
	D3DCommand* d3dCommand_ = nullptr;
	ResourceViewManager* rvManager_ = nullptr;
	GraphicsSystem* graphicsSystem_ = nullptr;
};

