#pragma once
#include "GameCore/ECS/ECSManager.h"
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "Core/Utility/CompBufferData.h"
#include "Resources/ResourceManager/ResourceManager.h"

class ResourceManager;

class LineRendererSystem : public ECSManager::MultiComponentSystem<LineRendererComponent>
{
	public:
		LineRendererSystem() : ECSManager::MultiComponentSystem<LineRendererComponent>(
			[&](Entity e, std::vector<LineRendererComponent>& lines)
			{
				for (LineRendererComponent& line : lines)
				{
					UpdateComponent(e, line);
				}
			},
			[&](Entity e, std::vector<LineRendererComponent>& lines)
			{
				for (LineRendererComponent& line : lines)
				{
					InitializeComponent(e, line);
				}
			},
			[&](Entity e, std::vector<LineRendererComponent>& lines)
			{
				for (LineRendererComponent& line : lines)
				{
					FinalizeComponent(e, line);
				}
			})
	{
	}
	~LineRendererSystem() = default;
private:
	void InitializeComponent([[maybe_unused]] Entity e, [[maybe_unused]] LineRendererComponent& line)
	{
	}
	void UpdateComponent([[maybe_unused]] Entity e, LineRendererComponent& line)
	{
		// 転送
		TransferMatrix(line);
	}
	void TransferMatrix(LineRendererComponent& lineRenderer)
	{
		// 転送
		uint32_t index = lineRenderer.mapID.value() * 2;
		{// 始点
			BUFFER_DATA_LINE bufferData;
			bufferData.position = lineRenderer.line.start;
			bufferData.color = lineRenderer.line.color;
			m_pIntegrationBuffer->UpdateData(bufferData, index);
		}
		{// 終点
			BUFFER_DATA_LINE bufferData;
			bufferData.position = lineRenderer.line.end;
			bufferData.color = lineRenderer.line.color;
			m_pIntegrationBuffer->UpdateData(bufferData, index + 1);
		}
	}
	void FinalizeComponent([[maybe_unused]] Entity e, [[maybe_unused]] LineRendererComponent& line)
	{
	}

	ResourceManager* m_pResourceManager = nullptr;
	VertexBuffer<BUFFER_DATA_LINE>* m_pIntegrationBuffer = nullptr;
};


