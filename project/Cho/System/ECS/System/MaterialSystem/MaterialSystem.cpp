#include "PrecompiledHeader.h"
#include "MaterialSystem.h"

void MaterialSystem::Initialize(EntityManager& entityManager, ComponentManager& componentManager)
{
	for (Entity entity : entityManager.GetActiveEntities()) {
		std::optional<std::reference_wrapper<MaterialComponent>> materialComp = componentManager.GetComponent<MaterialComponent>(entity);
		if (materialComp) {
			MaterialComponent& material = materialComp.value();
			UpdateMatrix(material);
		}
	}
}

void MaterialSystem::Update(EntityManager& entityManager, ComponentManager& componentManager)
{
	for (Entity entity : entityManager.GetActiveEntities()) {
		std::optional<std::reference_wrapper<MaterialComponent>> materialComp = componentManager.GetComponent<MaterialComponent>(entity);
		if (materialComp) {
			MaterialComponent& material = materialComp.value();
			UpdateMatrix(material);
		}
	}
}

void MaterialSystem::UpdateMatrix(MaterialComponent& material)
{
	material.matUV = MakeAffineMatrix(Vector3(material.uvScale.x, material.uvScale.y, 1.0f), Vector3(0.0f, 0.0f, material.uvRot), Vector3(material.uvPos.x, material.uvPos.y, 0.0f));

	TransferMatrix(material);
}

void MaterialSystem::TransferMatrix(MaterialComponent& material)
{
	material.constData->color = material.color;
	material.constData->enableLighting = material.enableLighting;
	material.constData->matUV = material.matUV;
	material.constData->shininess = material.shininess;
}
