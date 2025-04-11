#include "pch.h"
#include "MultiSystemManager.h"

// システムを登録する
void MultiSystemManager::RegisterSystem(std::unique_ptr<ECSManager::IMultiSystem> system, SystemState state)
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

void MultiSystemManager::StartAll(ECSManager* ecs)
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
void MultiSystemManager::UpdateAll(ECSManager* ecs)
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

void MultiSystemManager::EndAll(ECSManager* ecs)
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
