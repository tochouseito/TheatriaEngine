#pragma once
#include "GameCore/ECS/ECSManager.h"
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "Core/Utility/CompBufferData.h"
#include "GameCore/ScriptContext/ScriptContext.h"

class ResourceManager;
struct ScriptContext;

enum SystemPriority
{
	TransformSystem=0,
	CameraSystem,
	ScriptSystem,
};

// Transform初期化
class TransformInitializeSystem : public ECSManager::System<TransformComponent>
{
public:
	TransformInitializeSystem(ECSManager* ecs)
		: ECSManager::System<TransformComponent>([this](Entity e, TransformComponent& transform)
			{
				e;
				Start(transform);
			}),
		m_ECS(ecs)
	{
	}
	~TransformInitializeSystem() = default;
private:
	void Start(TransformComponent& transform);
	ECSManager* m_ECS = nullptr;
};
// Transform更新
class TransformUpdateSystem : public ECSManager::System<TransformComponent>
{
public:
	TransformUpdateSystem(ECSManager* ecs, ResourceManager* resourceManager, IStructuredBuffer* integrationBuffer)
		: ECSManager::System<TransformComponent>([this](Entity e, TransformComponent& transform)
			{
				e;
				transform;
				priorityUpdate(m_pECS);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager)
	{
		m_pIntegrationBuffer = dynamic_cast<StructuredBuffer<BUFFER_DATA_TF>*>(integrationBuffer);
	}
	~TransformUpdateSystem() = default;

private:
	// 優先度順に更新する
	void priorityUpdate(ECSManager* ecs);
	// 更新
	void UpdateComponent(Entity e, TransformComponent& transform);
	// 転送
	void TransferMatrix(TransformComponent& transform);

	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	StructuredBuffer<BUFFER_DATA_TF>* m_pIntegrationBuffer = nullptr;
};
// Transform終了処理
class TransformFinalizeSystem : public ECSManager::System<TransformComponent>
{
public:
	TransformFinalizeSystem(ECSManager* ecs)
		: ECSManager::System<TransformComponent>([this](Entity e, TransformComponent& transform)
			{
				e;
				Finalize(transform);
			}),
		m_ECS(ecs)
	{
	}
	~TransformFinalizeSystem() = default;
private:
	void Finalize(TransformComponent& transform);
	ECSManager* m_ECS = nullptr;
};
// Camera更新System
class CameraUpdateSystem : public ECSManager::System<TransformComponent, CameraComponent>
{
public:
	CameraUpdateSystem(ECSManager* ecs, ResourceManager* resourceManager, IStructuredBuffer* integrationBuffer)
		: ECSManager::System<TransformComponent, CameraComponent>([this](Entity e, TransformComponent& transform, CameraComponent& camera)
			{
				e;
				UpdateMatrix(transform, camera);
			}),
		m_ECS(ecs), m_pResourceManager(resourceManager)
	{
		m_pIntegrationBuffer = dynamic_cast<StructuredBuffer<BUFFER_DATA_TF>*>(integrationBuffer);
	}
	~CameraUpdateSystem() = default;
private:
	void UpdateMatrix(TransformComponent& transform, CameraComponent& camera);
	void TransferMatrix(TransformComponent& transform, CameraComponent& camera);

	ECSManager* m_ECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	StructuredBuffer<BUFFER_DATA_TF>* m_pIntegrationBuffer = nullptr;
};
// スクリプト初期化システム
class ScriptInitializeSystem : public ECSManager::System<ScriptComponent>
{
public:
	ScriptInitializeSystem(ECSManager* ecs)
		: ECSManager::System<ScriptComponent>([this](Entity e, ScriptComponent& script)
			{
				e;
				Start(script);
			}),
		m_ECS(ecs)
	{
	}
	~ScriptInitializeSystem() = default;
private:
	void LoadScript(ScriptComponent& script);
	void Start(ScriptComponent& script);
	void StartScript(ScriptComponent& script);
	ScriptContext MakeScriptContext(Entity entity, ECSManager* ecs);
	ECSManager* m_ECS = nullptr;
};
// スクリプト更新システム
class ScriptUpdateSystem : public ECSManager::System<ScriptComponent>
{
public:
	ScriptUpdateSystem(ECSManager* ecs)
		: ECSManager::System<ScriptComponent>([this](Entity e, ScriptComponent& script)
			{
				e;
				UpdateScript(script);
			}),
		m_ECS(ecs)
	{
	}
	~ScriptUpdateSystem() = default;
private:
	void UpdateScript(ScriptComponent& script);
	ScriptContext MakeScriptContext(Entity entity, ECSManager* ecs);
	ECSManager* m_ECS = nullptr;
};
// スクリプト終了システム
class ScriptFinalizeSystem : public ECSManager::System<ScriptComponent>
{
public:
	ScriptFinalizeSystem(ECSManager* ecs)
		: ECSManager::System<ScriptComponent>([this](Entity e, ScriptComponent& script)
			{
				e;
				FinalizeScript(script);
			}),
		m_ECS(ecs)
	{
	}
	~ScriptFinalizeSystem() = default;
private:
	void FinalizeScript(ScriptComponent& script);
	ECSManager* m_ECS = nullptr;
};