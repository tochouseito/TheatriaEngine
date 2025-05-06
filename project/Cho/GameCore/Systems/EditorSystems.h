#pragma once
#include "GameCore/ECS/ECSManager.h"
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "Core/Utility/CompBufferData.h"
#include "GameCore/ScriptAPI/ScriptAPI.h"

class ResourceManager;
class GraphicsEngine;
class EngineCommand;

// エディタのTransform更新システム
class TransformEditorSystem : public ECSManager::System<TransformComponent>
{
public:
	TransformEditorSystem(ECSManager* ecs, ResourceManager* resourceManager, IStructuredBuffer* integrationBuffer)
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
	~TransformEditorSystem() = default;

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
// エディタのカメラ更新システム
class CameraEditorSystem : public ECSManager::System<TransformComponent, CameraComponent>
{
public:
	CameraEditorSystem(ECSManager* ecs, ResourceManager* resourceManager, IStructuredBuffer* integrationBuffer)
		: ECSManager::System<TransformComponent, CameraComponent>([this](Entity e, TransformComponent& transform, CameraComponent& camera)
			{
				e;
				UpdateMatrix(transform, camera);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager)
	{
		m_pIntegrationBuffer = dynamic_cast<StructuredBuffer<BUFFER_DATA_TF>*>(integrationBuffer);
	}
	~CameraEditorSystem() = default;
private:
	void UpdateMatrix(TransformComponent& transform, CameraComponent& camera);
	void TransferMatrix(TransformComponent& transform, CameraComponent& camera);
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	StructuredBuffer<BUFFER_DATA_TF>* m_pIntegrationBuffer = nullptr;
};

// マテリアルの更新システム
class MaterialEditorSystem : public ECSManager::System<MaterialComponent>
{
public:
	MaterialEditorSystem(ECSManager* ecs, ResourceManager* resourceManager, IStructuredBuffer* integrationBuffer)
		: ECSManager::System<MaterialComponent>([this](Entity e, MaterialComponent& material)
			{
				e;
				TransferComponent(material);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager)
	{
		m_pIntegrationBuffer = dynamic_cast<StructuredBuffer<BUFFER_DATA_MATERIAL>*>(integrationBuffer);
	}
	~MaterialEditorSystem() = default;
private:
	void TransferComponent(const MaterialComponent& material);
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	StructuredBuffer<BUFFER_DATA_MATERIAL>* m_pIntegrationBuffer = nullptr;
};
// エミッターの更新システム
class EmitterEditorUpdateSystem : public ECSManager::System<EmitterComponent>
{
public:
	EmitterEditorUpdateSystem(ECSManager* ecs, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine)
		: ECSManager::System<EmitterComponent>([this](Entity e, EmitterComponent& emitter)
			{
				e;
				UpdateEmitter(emitter);
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager), m_pGraphicsEngine(graphicsEngine)
	{
	}
	~EmitterEditorUpdateSystem() = default;
private:
	void UpdateEmitter(EmitterComponent& emitter);
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	GraphicsEngine* m_pGraphicsEngine = nullptr;
};

// エフェクトEditorの更新システム
class EffectEditorUpdateSystem : public ECSManager::System<EffectComponent>
{
public:
	EffectEditorUpdateSystem(ECSManager* ecs, EngineCommand* engineCommand)
		: ECSManager::System<EffectComponent>([this](Entity e, EffectComponent& effect)
			{
				e;
				UpdateEffect(effect);
			}),
		m_pECS(ecs), m_pEngineCommand(engineCommand)
	{
	}
	~EffectEditorUpdateSystem() = default;
private:
	void UpdateEffect(EffectComponent& effect);
	ECSManager* m_pECS = nullptr;
	EngineCommand* m_pEngineCommand = nullptr;
};
