#pragma once
#include "GameCore/ECS/ECSManager.h"
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "Core/Utility/CompBufferData.h"
#include "GameCore/ScriptContext/ScriptContext.h"

class ResourceManager;

// LineRendererSystem
class LineRendererSystem : public ECSManager::MultiComponentSystem<LineRendererComponent>
{
public:
	LineRendererSystem(ECSManager* ecs, ResourceManager* resourceManager, IStructuredBuffer* integrationBuffer)
		: ECSManager::MultiComponentSystem<LineRendererComponent>([this](Entity e, std::vector<LineRendererComponent>& lines)
			{
				for (LineRendererComponent& line : lines)
				{
					UpdateComponent(e, line);
				}
			}),
		m_pECS(ecs), m_pResourceManager(resourceManager)
	{
		m_pIntegrationBuffer = resourceManager->GetLineIntegrationBuffer();
	}
	~LineRendererSystem() = default;
private:
	void UpdateComponent(Entity e, LineRendererComponent& line);
	void TransferMatrix(LineRendererComponent& lineRenderer);
	ECSManager* m_pECS = nullptr;
	ResourceManager* m_pResourceManager = nullptr;
	VertexBuffer<BUFFER_DATA_LINE>* m_pIntegrationBuffer = nullptr;
};

