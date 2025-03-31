#pragma once
#include "GameCore/ECS/ECSManager.h"
#include "ChoMath.h"
#include <unordered_set>
class IntegrationBuffer;
class SystemManager
{
public:
    void RegisterSystem(std::unique_ptr<ECSManager::ISystem> system)
    {
        m_Systems.emplace_back(std::move(system));
    }

    void UpdateAll(ECSManager* ecs)
    {
        for (auto& system : m_Systems)
        {
            system->Update(ecs);
        }
    }

private:
    std::vector<std::unique_ptr<ECSManager::ISystem>> m_Systems;
};

// TransformSystem
class ObjectSystem : public ECSManager::System<TransformComponent>
{
public:
	ObjectSystem(ECSManager* ecs,IntegrationBuffer* integrationBuffer)
		: ECSManager::System<TransformComponent>([this](TransformComponent&) {/* unused */})
		, m_ECS(ecs), m_IntegrationBuffer(integrationBuffer)
	{
	}

	void Update(ECSManager* ecs) override;

private:
	ECSManager* m_ECS = nullptr;
	IntegrationBuffer* m_IntegrationBuffer = nullptr;

	void UpdateMatrix(TransformComponent& transform, TransformComponent* parent = nullptr);
	void TransferMatrix(TransformComponent& transform);
	void UpdateRecursive(Entity entity, std::unordered_set<Entity>& updated);
};

// CameraSystem
class CameraSystem : public ECSManager::System<TransformComponent,CameraComponent>
{
public:
	CameraSystem(ECSManager* ecs, IntegrationBuffer* integrationBuffer)
		: ECSManager::System<TransformComponent,CameraComponent>([this](TransformComponent& transform,CameraComponent& camera)
			{
				UpdateMatrix(transform, camera);
			}),
			m_ECS(ecs), m_IntegrationBuffer(integrationBuffer)
	{
	}
	~CameraSystem() = default;
private:
	void UpdateMatrix(TransformComponent& transform, CameraComponent& camera);
	void TransferMatrix(TransformComponent& transform, CameraComponent& camera);

	ECSManager* m_ECS = nullptr;
	IntegrationBuffer* m_IntegrationBuffer = nullptr;
};