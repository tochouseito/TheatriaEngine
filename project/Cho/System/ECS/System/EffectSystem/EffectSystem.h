#pragma once

// ComponentManager
#include"ECS/ComponentManager/ComponentManager.h"

class D3DCommand;
class ResourceViewManager;
class GraphicsSystem;
class EffectSystem
{
public:
	void Initialize(EntityManager& entityManager, ComponentManager& componentManager);
	void Update(EntityManager& entityManager, ComponentManager& componentManager);

	void SetPtrs(D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem);
private:

	// 初期化
	void InitMatrix(EffectComponent& effect);

	// 更新
	void UpdateMatrix(EffectComponent& effect);

	// 転送
	void TransferMatrix(EffectComponent& effect);

	void LoopEffectNodeInit(EffectNode& effectNode, EffectComponent& effect);
	//void LoopEffectNodeEmit(EffectNode& effectNode, EffectComponent& effect);
	void LoopEffectNodeUpdate(EffectNode& effectNode, EffectComponent& effect);
	void LoopEffectNodeTransfer(EffectNode& effectNode, EffectComponent& effect);

	void TransferEffectNode(EffectNode& effectNode);

	// Dispatch
	void InitDispatch(EffectNode& effectNode);
	//void EmitDispatch(EffectNode& effectNode, EffectComponent& effect);
	void UpdateDispatch(EffectNode& effectNode, EffectComponent& effect);

private:
	D3DCommand* d3dCommand_ = nullptr;
	ResourceViewManager* rvManager_ = nullptr;
	GraphicsSystem* graphicsSystem_ = nullptr;
};

