#pragma once

// ComponentManager
#include"ECS/ComponentManager/ComponentManager.h"

class PhysicsSystem {
public:
    void Update(EntityManager& entityManager, ComponentManager& componentManager);
};

