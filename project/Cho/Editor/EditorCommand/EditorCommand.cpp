#include "pch.h"
#include "EditorCommand.h"
#include "Cho/Resources/ResourceManager/ResourceManager.h"
#include "Cho/Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Cho/GameCore/GameCore.h"

void AddGameObjectCommand::Execute(EditorCommand* edit)
{
	edit->GetGameCorePtr()->GetSceneManager()->AddGameObject();
}

void AddGameObjectCommand::Undo(EditorCommand* edit)
{
	edit;
}

void AddTransformComponent::Execute(EditorCommand* edit)
{
	edit->GetGameCorePtr()->GetSceneManager()->AddTransformComponent(m_Entity);
}

void AddTransformComponent::Undo(EditorCommand* edit)
{
	edit;
}

void AddMeshComponent::Execute(EditorCommand* edit)
{
	edit->GetGameCorePtr()->GetSceneManager()->AddMeshComponent(m_Entity);
}

void AddMeshComponent::Undo(EditorCommand* edit)
{
	edit;
}

void AddCameraComponent::Execute(EditorCommand* edit)
{
	edit->GetGameCorePtr()->GetSceneManager()->AddCameraComponent(m_Entity);
}

void AddCameraComponent::Undo(EditorCommand* edit)
{
	edit;
}

void SetMainCamera::Execute(EditorCommand* edit)
{
	m_PreCameraID = edit->GetGameCorePtr()->GetSceneManager()->SetMainCamera(m_SetCameraID);
}

void SetMainCamera::Undo(EditorCommand* edit)
{
	edit;
}

void AddRenderComponent::Execute(EditorCommand* edit)
{
	edit->GetGameCorePtr()->GetSceneManager()->AddRenderComponent(m_Entity);
}

void AddRenderComponent::Undo(EditorCommand* edit)
{
	edit;
}

// レンダリングテクスチャのハンドルを取得
D3D12_GPU_DESCRIPTOR_HANDLE EditorCommand::GetSceneTextureHandle()
{
	return m_ResourceManager->GetSUVDHeap()->GetGpuHandle(m_ResourceManager->GetBufferManager()->GetColorBuffer(m_GraphicsEngine->GetSceneTextureBufferID())->GetSUVHandleIndex());
}
