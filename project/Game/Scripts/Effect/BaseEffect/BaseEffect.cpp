#include "PrecompiledHeader.h"
#include "BaseEffect.h"

// SceneManager
#include "Scene/SceneManager/SceneManager.h"

BaseEffect::~BaseEffect()
{
	Destroy();
}

void BaseEffect::Create(const std::string& objectName, SceneManager* sceneManager, ComponentManager* compManager, InputManager* inputManager)
{
	createCount = true;
	sceneManager_ = sceneManager;
	compManager_ = compManager;
	input = inputManager;

	// Object生成
	name = sceneManager_->AddGameObject(objectName, ObjectType::Effect);

	// Component生成
	sceneManager_->GetGameObject(name)->AddComponent<EffectComponent>(EffectComponent());

	// Component取得
	effect = &compManager_->GetComponent<EffectComponent>(sceneManager_->GetGameObject(name)->GetEntityID())->get();
}

void BaseEffect::Destroy()
{
	if (!createCount) { return; }
	sceneManager_->DeleteGameObject(name);
}

void BaseEffect::CreateEffectResource()
{
	EffectNode effectNode;
	sceneManager_->CreateEffect(effectNode);
	effect->effectNodes.push_back(effectNode);
	effect->isInit = true;
}

void BaseEffect::Start()
{
	if (effect->isRun) {
		Reset();
		effect->isRun = true;
	}
	else {
		effect->isRun = true;
	}
}

void BaseEffect::Reset()
{
	effect->timeManager->globalTime = 0.0f;
	effect->isInit = true;
	effect->isRun = false;
}

void BaseEffect::SetPosition(const Vector3& pos)
{
	if (effect->effectNodes[0].position.type == PositionType::PositionPVA)
	{
		effect->effectNodes[0].position.pva.value.first = pos;
	}
	else {
		effect->effectNodes[0].position.value = pos;
	}
}
