#include "pch.h"
#include "EditorCommand.h"
#include "Cho/Resources/ResourceManager/ResourceManager.h"
#include "Cho/Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Cho/GameCore/GameCore.h"

void AddMeshFilterComponent::Execute(EditorCommand* edit)
{
	// MeshFilterComponentを追加
	edit->GetGameCoreCommandPtr()->AddMeshFilterComponent(m_Entity,edit->GetResourceManagerPtr()->GetModelManager());
}

void AddMeshFilterComponent::Undo(EditorCommand* edit)
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

void AddMeshRendererComponent::Execute(EditorCommand* edit)
{
	// MeshRendererComponentを追加
	edit->GetGameCoreCommandPtr()->AddMeshRendererComponent(m_Entity);
}

void AddMeshRendererComponent::Undo(EditorCommand* edit)
{
	edit;
}

D3D12_GPU_DESCRIPTOR_HANDLE EditorCommand::GetGameTextureHandle()
{
	// ゲームレンダリングテクスチャのバッファインデックスを取得
	uint32_t bufferIndex = m_GraphicsEngine->GetGameTextureBufferID();
	// ハンドルを取得
	return m_ResourceManager->GetBuffer<ColorBuffer>(bufferIndex)->GetSRVGpuHandle();
}

// レンダリングテクスチャのハンドルを取得
D3D12_GPU_DESCRIPTOR_HANDLE EditorCommand::GetSceneTextureHandle()
{
	// シーンレンダリングテクスチャのバッファインデックスを取得
	uint32_t bufferIndex = m_GraphicsEngine->GetSceneTextureBufferID();
	// ハンドルを取得
	return m_ResourceManager->GetBuffer<ColorBuffer>(bufferIndex)->GetSRVGpuHandle();
}

void Add3DObjectCommand::Execute(EditorCommand* edit)
{
	uint32_t mapID = edit->GetResourceManagerPtr()->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
	GameObject* object = edit->GetGameCoreCommandPtr()->Add3DObject(mapID);
	m_MapID = mapID;
	edit->SetSelectedObject(object);
}

void Add3DObjectCommand::Undo(EditorCommand* edit)
{
	edit;
}

void AddCameraObjectCommand::Execute(EditorCommand* edit)
{
	uint32_t mapID = edit->GetResourceManagerPtr()->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
	GameObject* object = edit->GetGameCoreCommandPtr()->AddCameraObject(mapID,edit->GetResourceManagerPtr());
	m_MapID = mapID;
	edit->SetSelectedObject(object);
}

void AddCameraObjectCommand::Undo(EditorCommand* edit)
{
	edit;
}
