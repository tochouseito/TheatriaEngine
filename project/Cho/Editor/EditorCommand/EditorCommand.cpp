#include "pch.h"
#include "EditorCommand.h"
#include "Cho/Resources/ResourceManager/ResourceManager.h"
#include "Cho/Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Cho/GameCore/GameCore.h"
#include "Platform/FileSystem/FileSystem.h"
#include "GameCore/SingleSystemManager/SingleSystemManager.h"
#include "GameCore/Systems/EditorSystems.h"
#include "GameCore/Systems/MultiSystems.h"

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

void EditorCommand::SaveProjectFile(const std::wstring& projectName)
{
	for (auto& scene : m_GameCoreCommand->GetSceneManagerPtr()->GetScenes().GetVector())
	{
		// シーンファイルを保存
		Cho::FileSystem::SaveSceneFile(
			L"GameProjects/" + projectName,
			scene.get(),
			m_GameCoreCommand->GetObjectContainerPtr(),
			m_GameCoreCommand->GetECSManagerPtr()
			);
	}
}

void EditorCommand::GenerateScript(const std::string& scriptName)
{
	FileSystem::ScriptProject::GenerateScriptFiles(scriptName);
	// プロジェクトファイルを更新
	FileSystem::ScriptProject::UpdateVcxproj();
	// スクリプトコンテナに追加
	ScriptContainer* scriptContainer = m_ResourceManager->GetScriptContainer();
	scriptContainer->AddScriptData(scriptName);
}

void EditorCommand::GameRun()
{
	m_GameCoreCommand->GetGameCorePtr()->GameRun();
}

void EditorCommand::GameStop()
{
	m_GameCoreCommand->GetGameCorePtr()->GameStop();
}

void EditorCommand::UpdateEditorScene()
{
	// ゲームが再生していたらスキップ
	if (m_GameCoreCommand->GetGameCorePtr()->IsRunning())
	{
		return;
	}
	m_pSingleSystem->UpdateAll(m_GameCoreCommand->GetECSManagerPtr());
	m_pMultiSystem->UpdateAll(m_GameCoreCommand->GetECSManagerPtr());
}

void EditorCommand::CreateSystem()
{
	// シングルシステムの生成
	std::unique_ptr<ECSManager::ISystem> transformSystem = std::make_unique<TransformEditorSystem>(m_GameCoreCommand->GetECSManagerPtr(), m_ResourceManager, m_ResourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pSingleSystem->RegisterSystem(std::move(transformSystem),SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> cameraSystem = std::make_unique<CameraEditorSystem>(m_GameCoreCommand->GetECSManagerPtr(), m_ResourceManager, m_ResourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pSingleSystem->RegisterSystem(std::move(cameraSystem), SystemState::Update);
	// マルチシステムの生成
	std::unique_ptr<ECSManager::IMultiSystem> lineRendererSystem = std::make_unique<LineRendererSystem>(m_GameCoreCommand->GetECSManagerPtr(), m_ResourceManager);
	m_pMultiSystem->RegisterSystem(std::move(lineRendererSystem), SystemState::Update);
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

void AddScriptComponent::Execute(EditorCommand* edit)
{
	edit->GetGameCoreCommandPtr()->AddScriptComponent(m_Entity, edit->GetResourceManagerPtr());
}

void AddScriptComponent::Undo(EditorCommand* edit)
{
	edit;
}

void AddLineRendererComponent::Execute(EditorCommand* edit)
{
	uint32_t mapID = edit->GetResourceManagerPtr()->GetIntegrationData(IntegrationDataType::Line)->GetMapID();
	// LineRendererComponentを追加
	edit->GetGameCoreCommandPtr()->AddLineRendererComponent(m_Entity, edit->GetResourceManagerPtr(),mapID);
}

void AddLineRendererComponent::Undo(EditorCommand* edit)
{
	edit;
}
