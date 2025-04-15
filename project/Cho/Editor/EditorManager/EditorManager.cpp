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
}

void EditorManager::Update()
{
	// MainMenu
	m_MainMenu->Update();
	// SceneView
	m_SceneView->Update();
	// GameView
	m_GameView->Update();
	// Hierarchy
	m_Hierarchy->Update();
	// Inspector
	m_Inspector->Update();
	// AssetBrowser
	m_AssetBrowser->Update();
	// Console
	m_Console->Update();
}
