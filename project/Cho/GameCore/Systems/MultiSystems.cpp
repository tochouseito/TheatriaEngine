#include "pch.h"
#include "MultiSystems.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "GameCore/IScript/IScript.h"
#include "Platform/FileSystem/FileSystem.h"
#include "Core/ChoLog/ChoLog.h"
using namespace Cho;

void LineRendererSystem::UpdateComponent(Entity e, LineRendererComponent& line)
{
	e;
	// 転送
	TransferMatrix(line);
}

void LineRendererSystem::TransferMatrix(LineRendererComponent& lineRenderer)
{
	// 転送
	{// 始点
		BUFFER_DATA_LINE bufferData;
		m_pIntegrationBuffer->UpdateData(bufferData, lineRenderer.mapID.value());
	}
	{// 終点
	}
}
