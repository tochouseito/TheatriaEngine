#include "PrecompiledHeader.h"
#include "BaseLight.h"
#include"Scene/SceneManager/SceneManager.h"
#include"ECS/ComponentManager/ComponentManager.h"

BaseLight::BaseLight()
{
}

BaseLight::~BaseLight()
{
	Destroy();
}

void BaseLight::Create(const std::string& objectName, LightType type, SceneManager* sceneManager, ComponentManager* compManager, InputManager* inputManager)
{
	createCount = true;
	sceneManager_ = sceneManager;
	compManager_ = compManager;
	input = inputManager;

	ObjectType objectType = ObjectType::DirectionalLight;
	switch (type)
	{
	case Directional:
		objectType = ObjectType::DirectionalLight;
		// Object生成
		name = sceneManager_->AddGameObject(objectName, objectType);
		// Component生成
		sceneManager_->GetGameObject(name)->AddComponent(DirectionalLightComponent());
		dirLight = &compManager_->GetComponent<DirectionalLightComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
		break;
	case Point:
		objectType = ObjectType::PointLight;
		// Object生成
		name = sceneManager_->AddGameObject(objectName, objectType);
		// Component生成
		sceneManager_->GetGameObject(name)->AddComponent(PointLightComponent());
		pointLight = &compManager_->GetComponent<PointLightComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
		break;
	case Spot:
		objectType = ObjectType::SpotLight;
		// Object生成
		name = sceneManager_->AddGameObject(objectName, objectType);
		// Component生成
		sceneManager_->GetGameObject(name)->AddComponent(SpotLightComponent());
		spotLight = &compManager_->GetComponent<SpotLightComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
		break;
	default:
		break;
	}
}

void BaseLight::Destroy()
{
	if (!createCount) { return; }
	sceneManager_->DeleteGameObject(name);
}

void BaseLight::GetGameObject(const std::string& objectName, LightType type, SceneManager* sceneManager, ComponentManager* compManager, InputManager* inputManager)
{
	sceneManager_ = sceneManager;
	compManager_ = compManager;
	input = inputManager;

	ObjectType objectType = ObjectType::DirectionalLight;
	switch (type)
	{
	case Directional:
		objectType = ObjectType::DirectionalLight;
		// Object取得
		name = sceneManager_->GetGameObject(objectName)->GetName();
		// Component取得
		dirLight = &compManager_->GetComponent<DirectionalLightComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
		if (!dirLight) {
			assert(0 && "Not found DirectionalLightComponent");
		}
		dirLight->active = true;
		break;
	case Point:
		objectType = ObjectType::PointLight;
		// Object取得
		name = sceneManager_->GetGameObject(objectName)->GetName();
		// Component取得
		pointLight = &compManager_->GetComponent<PointLightComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
		if (!pointLight) {
			assert(0 && "Not found PointLightComponent");
		}
		pointLight->active = true;
		break;
	case Spot:
		objectType = ObjectType::SpotLight;
		// Object取得
		name = sceneManager_->GetGameObject(objectName)->GetName();
		// Component取得
		spotLight = &compManager_->GetComponent<SpotLightComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
		if (!spotLight) {
			assert(0 && "Not found SpotLightComponent");
		}
		spotLight->active = true;
		break;
	default:
		break;
	}
}

void BaseLight::SetDirection(const Vector3& direction)
{
	if (dirLight) {
		dirLight->direction = direction;
		return;
	}
	if (pointLight) {
		return;
	}
	if (spotLight) {
		spotLight->direction = direction;
		return;
	}
}
