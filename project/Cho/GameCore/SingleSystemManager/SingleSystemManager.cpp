#include "pch.h"
#include "SingleSystemManager.h"

// システムを登録する
void SingleSystemManager::RegisterSystem(std::unique_ptr<ECSManager::ISystem> system,SystemState state)
{
	switch (state)
	{
	case Initialize:
		m_InitializeSystems.emplace_back(std::move(system));
		break;
	case Update:
		m_UpdateSystems.emplace_back(std::move(system));
		break;
	case Finalize:
		m_FinalizeSystems.emplace_back(std::move(system));
		break;
	default:
		break;
	}
}

void SingleSystemManager::StartAll(ECSManager* ecs)
{
	std::ranges::sort(m_InitializeSystems, [](const auto& a, const auto& b) {
		return a->GetPriority() < b->GetPriority();
		});

	for (auto& system : m_InitializeSystems)
	{
		if (system->IsEnabled())
		{
			system->Update(ecs);
		}
	}
}

// システムを更新する
void SingleSystemManager::UpdateAll(ECSManager* ecs)
{
	std::ranges::sort(m_UpdateSystems, [](const auto& a, const auto& b) {
		return a->GetPriority() < b->GetPriority();
		});

	for (auto& system : m_UpdateSystems)
	{
		if (system->IsEnabled())
		{
			system->Update(ecs);
		}
	}
}

void SingleSystemManager::EndAll(ECSManager* ecs)
{
	std::ranges::sort(m_FinalizeSystems, [](const auto& a, const auto& b) {
		return a->GetPriority() < b->GetPriority();
		});
	for (auto& system : m_FinalizeSystems)
	{
		if (system->IsEnabled())
		{
			system->Update(ecs);
		}
	}
}

