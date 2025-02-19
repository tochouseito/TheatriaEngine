#pragma once
// ComponentManager
#include"ECS/ComponentManager/ComponentManager.h"

#include"ConstantData/ModelData.h"

class D3DCommand;
class ResourceViewManager;
class GraphicsSystem;
class AnimationSystem
{
public:
	void Initialize(EntityManager& entityManager, ComponentManager& componentManager);

	void Update(EntityManager& entityManager, ComponentManager& componentManager);

	void SetPtrs(D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem);

private:

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes,const float& time);
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, const float& time);
	Scale CalculateValue(const std::vector<KeyframeScale>& keyframes, const float& time);

	void timeUpdate(AnimationComponent& animation,ModelData* model);

	void ApplyAnimation(AnimationComponent& animation, ModelData* model);
	void SkeletonUpdate(AnimationComponent& animation, ModelData* model);
	void SkinClusterUpdate(AnimationComponent& animation, ModelData* model);

	void ApplySkinning(AnimationComponent& animation, ModelData* model);

	void ResourceTransition();
private:
	D3DCommand* d3dCommand_ = nullptr;
	ResourceViewManager* rvManager_ = nullptr;
	GraphicsSystem* graphicsSystem_ = nullptr;
};

