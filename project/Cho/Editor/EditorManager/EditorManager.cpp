#include "pch.h"
#include "EditorManager.h"

void EditorManager::Initialize()
{
	// Toolbar
	m_Toolbar->Initialize();
	// MainMenu
	m_MainMenu->Initialize();
	// DebugCamera
	m_DebugCamera->Initialize();
	// SceneView
	m_SceneView->Initialize();
	// GameView
	m_GameView->Initialize();
	// Hierarchy
	m_Hierarchy->Initialize();
	// Inspector
	m_Inspector->Initialize();
	// AssetBrowser
	m_AssetBrowser->Initialize();
	// Console
	m_Console->Initialize();
	// EffectView
	m_EffectView->Initialize();
	// EffectEditor
	m_EffectEditor->Initialize();
}

void EditorManager::Update()
{
	// MainMenu
	m_MainMenu->Update();
	// AssetBrowser
	m_AssetBrowser->Update();
	// Console
	m_Console->Update();
	switch (m_WorkSpaceType)
	{
	case SceneEdit:
		// SceneView
		m_SceneView->Update();
		// GameView
		m_GameView->Update();
		// Hierarchy
		m_Hierarchy->Update();
		// Inspector
		m_Inspector->Update();
		break;
	case EffectEdit:
		// EffectView
		m_EffectView->Update();
		// EffectEditor
		m_EffectEditor->Update();
		break;
	default:
		break;
	}
}

void EditorManager::SetWorkSpaceType(const std::string& typeName)
{
	if (typeName == "SceneEdit") { m_WorkSpaceType = WorkSpaceType::SceneEdit; }
	if (typeName == "EffectEdit") { m_WorkSpaceType = WorkSpaceType::EffectEdit; }
}
