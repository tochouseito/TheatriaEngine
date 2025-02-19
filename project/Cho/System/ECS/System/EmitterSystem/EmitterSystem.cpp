#include "PrecompiledHeader.h"
#include "EmitterSystem.h"
#include"SystemState/SystemState.h"

void EmitterSystem::Initialize(EntityManager& entityManager, ComponentManager& componentManager)
{
	for (Entity entity : entityManager.GetActiveEntities()) {
		std::optional<std::reference_wrapper<EmitterComponent>> emitterComp = componentManager.GetComponent<EmitterComponent>(entity);
		if (emitterComp) {
			EmitterComponent& emitter = emitterComp.value();
			UpdateMatrix(emitter);
		}
	}
}

void EmitterSystem::Update(EntityManager& entityManager, ComponentManager& componentManager)
{
	for (Entity entity : entityManager.GetActiveEntities()) {
		std::optional<std::reference_wrapper<EmitterComponent>> emitterComp = componentManager.GetComponent<EmitterComponent>(entity);
		if (emitterComp) {
			EmitterComponent& emitter = emitterComp.value();
			UpdateMatrix(emitter);
		}
	}
}

void EmitterSystem::UpdateMatrix(EmitterComponent& emitter)
{
	emitter.frequencyTime += DeltaTime();// タイム加算

	// 射出間隔を上回ったら射出許可を出して時間を調整
	if (emitter.frequency <= emitter.frequencyTime) {
		emitter.frequencyTime = 0.0f;
		emitter.emit = 1;
	}
	else {
		// 射出間隔を上回ってないので、許可は出せない
		emitter.emit = 1;
	}

	// 転送
	TransferMatrix(emitter);
}

void EmitterSystem::TransferMatrix(EmitterComponent& emitter)
{
	emitter.constData->position = emitter.position;
	emitter.constData->radius = emitter.radius;
	emitter.constData->count = emitter.count;
	emitter.constData->frequency = emitter.frequency;
	emitter.constData->frequencyTime = emitter.frequencyTime;
	emitter.constData->emit = emitter.emit;
}
