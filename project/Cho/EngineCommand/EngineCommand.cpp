#include "pch.h"
#include "EngineCommand.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "GameCore/GameCore.h"
#include "Platform/FileSystem/FileSystem.h"

D3D12_GPU_DESCRIPTOR_HANDLE EngineCommand::GetGameTextureHandle()
{
	// ゲームレンダリングテクスチャのバッファインデックスを取得
	uint32_t bufferIndex = m_GraphicsEngine->GetGameTextureBufferID();
	// ハンドルを取得
	return m_ResourceManager->GetBuffer<ColorBuffer>(bufferIndex)->GetSRVGpuHandle();
}

D3D12_GPU_DESCRIPTOR_HANDLE EngineCommand::GetSceneTextureHandle()
{
	// シーンレンダリングテクスチャのバッファインデックスを取得
	uint32_t bufferIndex = m_GraphicsEngine->GetSceneTextureBufferID();
	// ハンドルを取得
	return m_ResourceManager->GetBuffer<ColorBuffer>(bufferIndex)->GetSRVGpuHandle();
}

D3D12_GPU_DESCRIPTOR_HANDLE EngineCommand::GetEffectEditTextureHandle()
{
	// シーンレンダリングテクスチャのバッファインデックスを取得
	uint32_t bufferIndex = m_GraphicsEngine->GetEffectEditTextureBufferID();
	// ハンドルを取得
	return m_ResourceManager->GetBuffer<ColorBuffer>(bufferIndex)->GetSRVGpuHandle();
}

void EngineCommand::SaveProjectFile(const std::wstring& projectName)
{
	for (auto& scene : m_GameCore->GetSceneManager()->GetScenes().GetVector())
	{
		// シーンファイルを保存
		Cho::FileSystem::SaveSceneFile(
			L"GameProjects/" + projectName,
			scene.get(),
			m_GameCore->GetObjectContainer(),
			m_GameCore->GetECSManager()
		);
	}
}

void EngineCommand::GenerateScript(const std::string& scriptName)
{
	FileSystem::ScriptProject::GenerateScriptFiles(scriptName);
	// プロジェクトファイルを更新
	FileSystem::ScriptProject::UpdateVcxproj();
	// スクリプトコンテナに追加
	m_ResourceManager->GetScriptContainer()->AddScriptData(scriptName);
}

void EngineCommand::GameRun()
{
	m_GameCore->GameRun();
}

void EngineCommand::GameStop()
{
	m_GameCore->GameStop();
}

bool EngineCommand::IsGameRunning() { return m_GameCore->IsRunning(); }

UINT64 EngineCommand::GetGameResolutionX() const
{
	return m_GraphicsEngine->m_ResolutionWidth;
}

UINT EngineCommand::GetGameResolutionY() const
{
	return m_GraphicsEngine->m_ResolutionHeight;
}
