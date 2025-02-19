#pragma once

// ComponentManager
#include"ECS/ComponentManager/ComponentManager.h"

class ResourceViewManager;

class LightSystem
{
public:
	void Initialize(EntityManager& entityManager, ComponentManager& componentManager);
	void Update(EntityManager& entityManager, ComponentManager& componentManager);

	void SetPtrs(ResourceViewManager* rvManager) { rvManager_ = rvManager; }
private:

	void UpdateDirLight(DirectionalLightComponent& dirLight);
	void UpdatePointLight(PointLightComponent& pointLight);
	void UpdateSpotLight(SpotLightComponent& spotLight);

	void TransferDirLight(DirectionalLightComponent& dirLight);
	void TransferPointLight(PointLightComponent& pointLight);
	void TransferSpotLight(SpotLightComponent& spotLight);

private:// メンバ変数
	ResourceViewManager* rvManager_ = nullptr;
};

