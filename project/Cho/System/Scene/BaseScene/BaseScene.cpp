#include "PrecompiledHeader.h"
#include "BaseScene.h"
#include"Scene/SceneManager/SceneManager.h"

void BaseScene::Initialize()
{

}

void BaseScene::Finalize()
{
}

void BaseScene::Update()
{
}

void BaseScene::Draw()
{
}

void BaseScene::ChangeScene()
{
	//sceneManager_->SetNextScene
}

std::string BaseScene::CreateGameObject(const std::string& name)
{
	std::string newName;
	newName = sceneManager_->AddGameObject(name,ObjectType::Object);
	TransformComponent tf;
	sceneManager_->GetGameObject(newName)->AddComponent(tf);
	MeshComponent mesh;
	sceneManager_->GetGameObject(newName)->AddComponent(mesh);
	MaterialComponent material;
	sceneManager_->GetGameObject(newName)->AddComponent(material);
	RenderComponent render;
	sceneManager_->GetGameObject(newName)->AddComponent(render);
	AnimationComponent animation;
	sceneManager_->GetGameObject(newName)->AddComponent(animation);

	return newName;
}

std::string BaseScene::CreateCamera(const std::string& name)
{
	std::string newName;
	newName = sceneManager_->AddGameObject(name,ObjectType::Camera);
	CameraComponent camera;
	sceneManager_->GetGameObject(name)->AddComponent(camera);
	return newName;
}


