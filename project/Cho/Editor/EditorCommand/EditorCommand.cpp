#include "pch.h"
#include "EditorCommand.h"
#include "Cho/Resources/ResourceManager/ResourceManager.h"
#include "Cho/Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Cho/GameCore/GameCore.h"

void AddGameObjectCommand::Execute(EditorCommand* edit)
{
	edit->GetGameCoreCommandPtr()->AddGameObject(m_Name,m_Type);
}

void AddGameObjectCommand::Undo(EditorCommand* edit)
{
	edit;
}

void AddTransformComponent::Execute(EditorCommand* edit)
{
	uint32_t mapID = edit->GetResourceManagerPtr()->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
	edit->GetGameCoreCommandPtr()->AddTransformComponent(m_Entity,mapID);
	m_MapID = mapID;
}

void AddTransformComponent::Undo(EditorCommand* edit)
{
	edit;
}

void AddMeshComponent::Execute(EditorCommand* edit)
{
	// MeshComponentを追加
	edit->GetGameCoreCommandPtr()->AddMeshComponent(m_Entity,edit->GetResourceManagerPtr()->GetModelManager());
}

void AddMeshComponent::Undo(EditorCommand* edit)
{
	edit;
}

void AddCameraComponent::Execute(EditorCommand* edit)
{
	edit->GetGameCoreCommandPtr()->AddCameraComponent(m_Entity, edit->GetResourceManagerPtr());
}

void AddCameraComponent::Undo(EditorCommand* edit)
{
	edit;
}

void SetMainCamera::Execute(EditorCommand* edit)
{
	m_PreCameraID = edit->GetGameCoreCommandPtr()->SetMainCamera(m_SetCameraID.value());
}

void SetMainCamera::Undo(EditorCommand* edit)
{
	edit;
}

void AddRenderComponent::Execute(EditorCommand* edit)
{
	edit->GetGameCoreCommandPtr()->AddRenderComponent(m_Entity);
}

void AddRenderComponent::Undo(EditorCommand* edit)
{
	edit;
}

// レンダリングテクスチャのハンドルを取得
D3D12_GPU_DESCRIPTOR_HANDLE EditorCommand::GetSceneTextureHandle()
{
	// シーンレンダリングテクスチャのバッファインデックスを取得
	uint32_t bufferIndex = m_GraphicsEngine->GetSceneTextureBufferID();
	// ハンドルを取得
	return m_ResourceManager->GetBuffer<ColorBuffer>(bufferIndex)->GetSRVGpuHandle();
}
