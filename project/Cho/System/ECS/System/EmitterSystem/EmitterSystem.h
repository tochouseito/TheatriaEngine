#pragma once

// ComponentManager
#include"ECS/ComponentManager/ComponentManager.h"

class EmitterSystem
{
public:
	void Initialize(EntityManager& entityManager, ComponentManager& componentManager);

	void Update(EntityManager& entityManager, ComponentManager& componentManager);
private:
	// 更新
	void UpdateMatrix(EmitterComponent& emitter);

	void TransferMatrix(EmitterComponent& emitter);
};

