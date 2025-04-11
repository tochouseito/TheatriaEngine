#pragma once
#include "GameCore/ECS/ECSManager.h"
// マルチコンポーネントシステムマネージャー
class MultiSystemManager
{
public:
	// Constructor
	MultiSystemManager() = default;
	// Destructor
	~MultiSystemManager() = default;
	// システムを登録する
	void RegisterSystem(std::unique_ptr<ECSManager::IMultiSystem> system,SystemState state);
	// システムを初期化する
	void StartAll(ECSManager* ecs);
	// システムを更新する
	void UpdateAll(ECSManager* ecs);
	// システムをクリーンアップする
	void EndAll(ECSManager* ecs);
private:
	// 初期化システムのリスト
	std::vector<std::unique_ptr<ECSManager::IMultiSystem>> m_InitializeSystems;
	// 更新システムのリスト
	std::vector<std::unique_ptr<ECSManager::IMultiSystem>> m_UpdateSystems;
	// クリーンアップシステムのリスト
	std::vector<std::unique_ptr<ECSManager::IMultiSystem>> m_FinalizeSystems;
};

