#include "PrecompiledHeader.h"
#include "PhysicsSystem.h"
#include"SystemState/SystemState.h"

void PhysicsSystem::Update(EntityManager& entityManager, ComponentManager& componentManager) {
    for (Entity entity : entityManager.GetActiveEntities()) {
		std::optional<std::reference_wrapper<PhysicsComponent>> physicsComp = componentManager.GetComponent<PhysicsComponent>(entity);
		std::optional<std::reference_wrapper<TransformComponent>> transformComp = componentManager.GetComponent<TransformComponent>(entity);
        if (physicsComp && transformComp) {
			PhysicsComponent& physics = physicsComp.value();
			TransformComponent& transform = transformComp.value();
            // 物理計算: 位置の更新
            physics.velocity.x += physics.acceleration.x * DeltaTime();
            transform.translation.x += physics.velocity.x * DeltaTime();
            // 他の軸も同様に処理
        }
    }
}
