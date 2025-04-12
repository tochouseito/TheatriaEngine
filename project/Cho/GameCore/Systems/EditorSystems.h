#pragma once
#include "GameCore/ECS/ECSManager.h"
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "Core/Utility/CompBufferData.h"
#include "GameCore/ScriptAPI/ScriptAPI.h"

class ResourceManager;

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
