#include "PrecompiledHeader.h"
#include "BaseSprite.h"
#include"Scene/SceneManager/SceneManager.h"
#include"ECS/ComponentManager/ComponentManager.h"

BaseSprite::BaseSprite()
{
}

BaseSprite::~BaseSprite()
{
	Destroy();
}

void BaseSprite::Create(const std::string& objectName, SceneManager* sceneManager, ComponentManager* compManager, InputManager* inputManager)
{
	createCount = true;

	sceneManager_ = sceneManager;
	compManager_ = compManager;
	input = inputManager;

	// Object生成
	name = sceneManager_->AddGameObject(objectName, ObjectType::Sprite);

	// Component生成
	SpriteComponent spriteComp;
	sceneManager_->GetGameObject(name)->AddComponent(spriteComp);

	sprite = &compManager_->GetComponent<SpriteComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
}

void BaseSprite::Destroy()
{
	if (!createCount) { return; }
	// 今は見えなくするだけ
	sprite->render.visible = false;
	sceneManager_->DeleteGameObject(name);
}

void BaseSprite::SetTexture(const std::string& textureName)
{
	sprite->material.textureID = textureName;
}

void BaseSprite::SetColor(const Color& color)
{
	sprite->material.color = color;
}
