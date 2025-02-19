#include "PrecompiledHeader.h"
#include "BaseCamera.h"
#include"Scene/SceneManager/SceneManager.h"
#include"ECS/ComponentManager/ComponentManager.h"

BaseCamera::BaseCamera()
{

}

BaseCamera::~BaseCamera()
{
	Destroy();
}

void BaseCamera::Create(const std::string& cameraName, SceneManager* sceneManager, ComponentManager* compManager, InputManager* inputManager)
{
	createCount = true;
	sceneManager_ = sceneManager;
	compManager_ = compManager;
	input = inputManager;

	// Object生成
	name = sceneManager_->AddGameObject(cameraName,ObjectType::Camera);

	{
		// Component生成
		CameraComponent cameraComp;
		sceneManager_->GetGameObject(name)->AddComponent(cameraComp);
		
	}
	camera = &compManager_->GetComponent<CameraComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
}

void BaseCamera::Destroy()
{
	if (!createCount) { return; }
	sceneManager_->DeleteGameObject(name);
}
