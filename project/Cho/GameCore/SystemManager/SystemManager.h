#pragma once
#include "GameCore/ECS/ECSManager.h"
#include "ChoMath.h"
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "Core/Utility/CompBufferData.h"
#include "GameCore/ScriptContext/ScriptContext.h"
#include <unordered_set>
class ResourceManager;
class SystemManager
{
public:
	// Constructor
	SystemManager() = default;
	// Destructor
	~SystemManager() = default;

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
	ObjectSystem(ECSManager* ecs,ResourceManager* resourceManager,IStructuredBuffer* integrationBuffer)
		: ECSManager::System<TransformComponent>([this,ecs](TransformComponent&) {Run(ecs);})
		, m_pECS(ecs), m_pResourceManager(resourceManager)
	{
		m_pIntegrationBuffer = dynamic_cast<StructuredBuffer<BUFFER_DATA_TF>*>(integrationBuffer);
	}

private:
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	StructuredBuffer<BUFFER_DATA_TF>* m_pIntegrationBuffer = nullptr;

	void Run(ECSManager* ecs);
	void UpdateMatrix(TransformComponent& transform, TransformComponent* parent = nullptr);
	void TransferMatrix(TransformComponent& transform);
	void UpdateRecursive(Entity entity, std::unordered_set<Entity>& updated);
};
// TransformStartSystem
class TransformStartSystem : public ECSManager::System<TransformComponent>
{
public:
	TransformStartSystem(ECSManager* ecs)
		: ECSManager::System<TransformComponent>([this](TransformComponent& transform)
			{
				Start(transform);
			}),
		m_ECS(ecs)
	{
	}
	~TransformStartSystem() = default;
private:
	void Start(TransformComponent& transform);
	ECSManager* m_ECS = nullptr;
};
// TransformCleanupSystem
class TransformCleanupSystem : public ECSManager::System<TransformComponent>
{
public:
	TransformCleanupSystem(ECSManager* ecs)
		: ECSManager::System<TransformComponent>([this](TransformComponent& transform)
			{
				Cleanup(transform);
			}),
		m_ECS(ecs)
	{
	}
	~TransformCleanupSystem() = default;
private:
	void Cleanup(TransformComponent& transform);
	ECSManager* m_ECS = nullptr;
};
// CameraSystem
class CameraSystem : public ECSManager::System<TransformComponent,CameraComponent>
{
public:
	CameraSystem(ECSManager* ecs,ResourceManager* resourceManager, IStructuredBuffer* integrationBuffer)
		: ECSManager::System<TransformComponent,CameraComponent>([this](TransformComponent& transform,CameraComponent& camera)
			{
				UpdateMatrix(transform, camera);
			}),
		m_ECS(ecs), m_pResourceManager(resourceManager)
	{
		m_pIntegrationBuffer = dynamic_cast<StructuredBuffer<BUFFER_DATA_TF>*>(integrationBuffer);
	}
	~CameraSystem() = default;
private:
	void UpdateMatrix(TransformComponent& transform, CameraComponent& camera);
	void TransferMatrix(TransformComponent& transform, CameraComponent& camera);

	ECSManager* m_ECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	StructuredBuffer<BUFFER_DATA_TF>* m_pIntegrationBuffer = nullptr;
};

// ScriptUpdateSystem
class ScriptUpdateSystem : public ECSManager::System<ScriptComponent>
{
public:
	ScriptUpdateSystem(ECSManager* ecs)
		: ECSManager::System<ScriptComponent>([this](ScriptComponent& script)
			{
				Update(script);
			}),
		m_ECS(ecs)
	{
	}
	~ScriptUpdateSystem() = default;
private:
	void Update(ScriptComponent& script);
	ScriptContext MakeScriptContext(Entity entity, ECSManager* ecs)
	{
		ScriptContext ctx;

		ctx.transform = ecs->GetComponent<TransformComponent>(entity);
		ctx.camera = ecs->GetComponent<CameraComponent>(entity);
		ctx.meshFilter = ecs->GetComponent<MeshFilterComponent>(entity);
		ctx.meshRenderer = ecs->GetComponent<MeshRendererComponent>(entity);
		ctx.script = ecs->GetComponent<ScriptComponent>(entity);

		return ctx;
	}
	ECSManager* m_ECS = nullptr;
};
// ScriptStartSystem
class ScriptStartSystem : public ECSManager::System<ScriptComponent>
{

public:
	ScriptStartSystem(ECSManager* ecs)
		: ECSManager::System<ScriptComponent>([this](ScriptComponent& script)
			{
				Start(script);
			}),
		m_ECS(ecs)
	{
	}
	~ScriptStartSystem() = default;
private:
	void Start(ScriptComponent& script);
	void LoadScript(ScriptComponent& script);
	void StartScript(ScriptComponent& script);
	ScriptContext MakeScriptContext(Entity entity, ECSManager* ecs);
	ECSManager* m_ECS = nullptr;
};
// ScriptCleanupSystem
class ScriptCleanupSystem : public ECSManager::System<ScriptComponent>
{
public:
	ScriptCleanupSystem(ECSManager* ecs)
		: ECSManager::System<ScriptComponent>([this](ScriptComponent& script)
			{
				Cleanup(script);
			}),
		m_ECS(ecs)
	{
	}
	~ScriptCleanupSystem() = default;
private:
	void Cleanup(ScriptComponent& script);
	ECSManager* m_ECS = nullptr;
};
// エディタの更新
class EditorUpdateSystem : public ECSManager::System<TransformComponent>
{
public:
	EditorUpdateSystem(ECSManager* ecs, ResourceManager* resourceManager, IStructuredBuffer* integrationBuffer)
		: ECSManager::System<TransformComponent>([this,ecs](TransformComponent&)
			{
				Run(ecs);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager)
	{
		m_pIntegrationBuffer = dynamic_cast<StructuredBuffer<BUFFER_DATA_TF>*>(integrationBuffer);
	}
	~EditorUpdateSystem() = default;
private:
	void Run(ECSManager* ecs);
	void UpdateMatrix(TransformComponent& transform, TransformComponent* parent = nullptr);
	void TransferMatrix(TransformComponent& transform);
	void UpdateRecursive(Entity entity, std::unordered_set<Entity>& updated);

	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	StructuredBuffer<BUFFER_DATA_TF>* m_pIntegrationBuffer = nullptr;
};
// エディタのカメラの更新
class EditorCameraSystem : public ECSManager::System<TransformComponent, CameraComponent>
{
public:
	EditorCameraSystem(ECSManager* ecs, ResourceManager* resourceManager, IStructuredBuffer* integrationBuffer)
		: ECSManager::System<TransformComponent, CameraComponent>([this](TransformComponent& transform, CameraComponent& camera)
			{
				UpdateMatrix(transform, camera);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager)
	{
		m_pIntegrationBuffer = dynamic_cast<StructuredBuffer<BUFFER_DATA_TF>*>(integrationBuffer);
	}
	~EditorCameraSystem() = default;
private:
	void UpdateMatrix(TransformComponent& transform, CameraComponent& camera);
	void TransferMatrix(TransformComponent& transform, CameraComponent& camera);
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	StructuredBuffer<BUFFER_DATA_TF>* m_pIntegrationBuffer = nullptr;
};
// ライン描画
class LineRendererSystem : public ECSManager::System<LineRendererComponent>
{
public:
	LineRendererSystem(ECSManager* ecs, ResourceManager* resourceManager)
		: ECSManager::System<LineRendererComponent>([this](LineRendererComponent& line)
			{
				Update(line);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager)
	{
		// バッファの作成
		uint32_t index = m_pResourceManager->CreateVertexBuffer<BUFFER_DATA_LINE>(kMaxLineCount * 2);
		m_pLineBuffer = dynamic_cast<VertexBuffer<BUFFER_DATA_LINE>*>(m_pResourceManager->GetBuffer<IVertexBuffer>(index));
	}
	~LineRendererSystem() = default;
private:
	void Update(LineRendererComponent& line);
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	// ラインのバッファ
	VertexBuffer<BUFFER_DATA_LINE>* m_pLineBuffer = nullptr;
	// 最大ライン数
	static const uint32_t kMaxLineCount = 1000;
};