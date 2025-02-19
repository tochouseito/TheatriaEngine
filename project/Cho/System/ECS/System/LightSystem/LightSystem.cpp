#include "PrecompiledHeader.h"
#include "LightSystem.h"

// D3D12
#include "D3D12/ResourceViewManager/ResourceViewManager.h"

// ChoMath
#include"ChoMath.h"

void LightSystem::Initialize(EntityManager& entityManager, ComponentManager& componentManager)
{
	for (Entity entity : entityManager.GetActiveEntities()) {
		std::optional<std::reference_wrapper<DirectionalLightComponent>> dirLightCompo = componentManager.GetComponent<DirectionalLightComponent>(entity);
		if (dirLightCompo) {
			DirectionalLightComponent& dirLight = dirLightCompo.value();
			UpdateDirLight(dirLight);
			continue;
		}
		std::optional<std::reference_wrapper<PointLightComponent>> pointLightCompo = componentManager.GetComponent<PointLightComponent>(entity);
		if (pointLightCompo) {
			PointLightComponent& pointLight = pointLightCompo.value();
			UpdatePointLight(pointLight);
			continue;
		}
		std::optional<std::reference_wrapper<SpotLightComponent>> spotLightCompo = componentManager.GetComponent<SpotLightComponent>(entity);
		if (spotLightCompo) {
			SpotLightComponent& spotLight = spotLightCompo.value();
			UpdateSpotLight(spotLight);
			continue;
		}
	}
}

void LightSystem::Update(EntityManager& entityManager, ComponentManager& componentManager)
{
	for (Entity entity : entityManager.GetActiveEntities()) {
		std::optional<std::reference_wrapper<DirectionalLightComponent>> dirLightCompo = componentManager.GetComponent<DirectionalLightComponent>(entity);
		if (dirLightCompo) {
			DirectionalLightComponent& dirLight = dirLightCompo.value();
			//if (dirLight.active) {
				UpdateDirLight(dirLight);
			//}
			continue;
		}
		std::optional<std::reference_wrapper<PointLightComponent>> pointLightCompo = componentManager.GetComponent<PointLightComponent>(entity);
		if (pointLightCompo) {
			PointLightComponent& pointLight = pointLightCompo.value();
			//if (pointLight.active) {
				UpdatePointLight(pointLight);
			//}
			continue;
		}
		std::optional<std::reference_wrapper<SpotLightComponent>> spotLightCompo = componentManager.GetComponent<SpotLightComponent>(entity);
		if (spotLightCompo) {
			SpotLightComponent& spotLight = spotLightCompo.value();
			//if (spotLight.active) {
				UpdateSpotLight(spotLight);
			//}
			continue;
		}
	}
}

void LightSystem::UpdateDirLight(DirectionalLightComponent& dirLight)
{
	TransferDirLight(dirLight);
}

void LightSystem::UpdatePointLight(PointLightComponent& pointLight)
{
	TransferPointLight(pointLight);
}

void LightSystem::UpdateSpotLight(SpotLightComponent& spotLight)
{
	TransferSpotLight(spotLight);
}

void LightSystem::TransferDirLight(DirectionalLightComponent& dirLight)
{
	PunctualLightData* lightData = rvManager_->GetLightConstData();
	lightData->dirLights[dirLight.index].active = dirLight.active;
	if (!dirLight.active) {
		return;
	}
	lightData->dirLights[dirLight.index].color = dirLight.color;
	lightData->dirLights[dirLight.index].direction = dirLight.direction;
	lightData->dirLights[dirLight.index].intensity = dirLight.intensity;
}

void LightSystem::TransferPointLight(PointLightComponent& pointLight)
{
	PunctualLightData* lightData = rvManager_->GetLightConstData();
	lightData->pointLights[pointLight.index].active = pointLight.active;
	if (!pointLight.active) {
		return;
	}
	lightData->pointLights[pointLight.index].color = pointLight.color;
	lightData->pointLights[pointLight.index].intensity = pointLight.intensity;
	lightData->pointLights[pointLight.index].position = pointLight.position;
	lightData->pointLights[pointLight.index].radius = pointLight.radius;
}

void LightSystem::TransferSpotLight(SpotLightComponent& spotLight)
{
	PunctualLightData* lightData = rvManager_->GetLightConstData();
	lightData->spotLights[spotLight.index].active = spotLight.active;
	if (!spotLight.active) {
		return;
	}
	lightData->spotLights[spotLight.index].color = spotLight.color;
	lightData->spotLights[spotLight.index].intensity = spotLight.intensity;
	lightData->spotLights[spotLight.index].position = spotLight.position;
	lightData->spotLights[spotLight.index].direction = spotLight.direction;
	lightData->spotLights[spotLight.index].distance = spotLight.distance;
	lightData->spotLights[spotLight.index].decay = spotLight.decay;
	lightData->spotLights[spotLight.index].cosAngle = std::cos(ChoMath::Pi()/spotLight.cosAngle);
	lightData->spotLights[spotLight.index].cosFalloffStart = spotLight.cosFalloffStart;
}

