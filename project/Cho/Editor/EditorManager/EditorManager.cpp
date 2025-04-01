#include "pch.h"
#include "EditorManager.h"

void EditorManager::Initialize()
{
	// MainMenu
	m_MainMenu->Initialize();
	// SceneView
	m_SceneView->Initialize();

}

void EditorManager::Update()
{
	// MainMenu
	m_MainMenu->Update();
	// SceneView
	m_SceneView->Update();
}
