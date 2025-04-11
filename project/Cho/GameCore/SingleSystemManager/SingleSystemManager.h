#pragma once
#include "GameCore/ECS/ECSManager.h"
class SingleSystemManager
{
public:
	// Constructor
	SingleSystemManager() = default;
	// Destructor
	~SingleSystemManager() = default;
	// システムを登録する
	void RegisterSystem(std::unique_ptr<ECSManager::ISystem> system,SystemState state);
	// システムを初期化する
	void StartAll(ECSManager* ecs);
	// システムを更新する
	void UpdateAll(ECSManager* ecs);
	// システムをクリーンアップする
	void EndAll(ECSManager* ecs);
private:
	std::vector<std::unique_ptr<ECSManager::ISystem>> m_InitializeSystems;// 初期化システムのリスト
	std::vector<std::unique_ptr<ECSManager::ISystem>> m_UpdateSystems;// システムのリスト
	std::vector<std::unique_ptr<ECSManager::ISystem>> m_FinalizeSystems;// クリーンアップシステムのリスト
};

