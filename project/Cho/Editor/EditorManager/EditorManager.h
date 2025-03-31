#pragma once
#include <memory>
#include <imgui.h>

#include "Editor/EditorCommand/EditorCommand.h"
#include "Editor/MainMenu/MainMenu.h"
#include "Editor/SceneView/SceneView.h"

class EditorManager
{
public:
	EditorManager(EditorCommand* editorCommand) :
		m_EditorCommand(editorCommand)
	{
		m_MainMenu = std::make_unique<MainMenu>(this);
		m_SceneView = std::make_unique<SceneView>(this);
	}
	~EditorManager()
	{

	}
	void Initialize();
	void Update();

	EditorCommand* GetEditorCommand() { return m_EditorCommand; }
private:
	EditorCommand* m_EditorCommand = nullptr;
	std::unique_ptr<MainMenu> m_MainMenu = nullptr;
	std::unique_ptr<SceneView> m_SceneView = nullptr;
};

