#include "PrecompiledHeader.h"
#include "BaseObject.h"
#include"Scene/SceneManager/SceneManager.h"
#include"ECS/ComponentManager/ComponentManager.h"

BaseObject::BaseObject()
{
	
}

BaseObject::~BaseObject()
{
	Destroy();
}

void BaseObject::Create(const std::string& objectName,SceneManager* sceneManager, ComponentManager* compManager, InputManager* inputManager, bool isInstance, uint32_t numInstance)
{
	createCount = true;
	sceneManager_ = sceneManager;
	compManager_ = compManager;
	input = inputManager;

	// Object生成
	name = sceneManager_->AddGameObject(objectName,ObjectType::Object);

	{
		// Component生成
		TransformComponent tfComp;
		if (isInstance) { 
			tfComp.isInstance = true;
			tfComp.instanceCount = numInstance;
		}
		sceneManager_->GetGameObject(name)->AddComponent(tfComp);
		MeshComponent meshComp;
		sceneManager_->GetGameObject(name)->AddComponent(meshComp);
		MaterialComponent materialComp;
		sceneManager_->GetGameObject(name)->AddComponent(materialComp);
		RenderComponent renderComp;
		sceneManager_->GetGameObject(name)->AddComponent(renderComp);
		AnimationComponent animationComp;
		sceneManager_->GetGameObject(name)->AddComponent(animationComp);
	}

	transform = &compManager_->GetComponent<TransformComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
	mesh = &compManager_->GetComponent<MeshComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
	material = &compManager_->GetComponent<MaterialComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
	render = &compManager_->GetComponent<RenderComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
	animation = &compManager_->GetComponent<AnimationComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
	audio = sceneManager_->GetAudioManagerPtr();
}

void BaseObject::Destroy()
{
	if (!createCount) { return; }
	sceneManager_->DeleteGameObject(name);
}

void BaseObject::GetGameObject(const std::string& objectName, SceneManager* sceneManager, ComponentManager* compManager, InputManager* inputManager)
{
	sceneManager_ = sceneManager;
	compManager_ = compManager;
	input = inputManager;

	name = sceneManager_->GetGameObject(objectName)->GetName();

	transform = &compManager_->GetComponent<TransformComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
	mesh = &compManager_->GetComponent<MeshComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
	material = &compManager_->GetComponent<MaterialComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
	render = &compManager_->GetComponent<RenderComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
	animation = &compManager_->GetComponent<AnimationComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();

	render->visible = true;
}

void BaseObject::SetModel(const std::string& modelName)
{
	mesh->meshesName = modelName;
}
