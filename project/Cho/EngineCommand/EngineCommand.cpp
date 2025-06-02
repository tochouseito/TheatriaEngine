#include "pch.h"
#include "EngineCommand.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "GameCore/GameCore.h"
#include "Platform/FileSystem/FileSystem.h"
#include "Core/Utility/EffectStruct.h"
#include "EngineCommand/EngineCommands.h"

EngineCommand::EngineCommand(GameCore* gameCore, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine,InputManager* input):
	m_GameCore(gameCore),
	m_ResourceManager(resourceManager),
	m_GraphicsEngine(graphicsEngine),
	m_InputManager(input)
{
	
}

GameObject* EngineCommand::GetSelectedObject()
{
	if (!m_SelectedObjectName.has_value())
	{
		return nullptr;
	}
	return &m_GameCore->GetObjectContainer()->GetGameObjectByName(m_SelectedObjectName.value());
}

// エディタの選択中オブジェクトをセット
void EngineCommand::SetSelectedObject(const std::optional<std::wstring>& name)
{
	if (name.has_value() && name.value().empty())
	{
		m_SelectedObjectName = std::nullopt;
	}
	else
	{
		m_SelectedObjectName = name;
	}
}

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

D3D12_GPU_DESCRIPTOR_HANDLE EngineCommand::GetTextureHandle(const std::wstring& name)
{
	return m_ResourceManager->GetBuffer<PixelBuffer>(m_ResourceManager->GetTextureManager()->GetTextureData(name)->bufferIndex)->GetSRVGpuHandle();
}

void EngineCommand::SaveProjectFile()
{
	Cho::FileSystem::SaveProject(m_GameCore->GetSceneManager(), m_GameCore->GetObjectContainer(), m_GameCore->GetECSManager(), m_ResourceManager);
}

void EngineCommand::GenerateScript(const std::string& scriptName)
{
	Cho::FileSystem::ScriptProject::GenerateScriptFiles(scriptName);
	// プロジェクトファイルを更新
	Cho::FileSystem::ScriptProject::UpdateVcxproj();
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
