#pragma once
#include <memory>
#include <imgui.h>

#include "Cho/Editor/EditorCommand/EditorCommand.h"
#include "Cho/Editor/MainMenu/MainMenu.h"

class EditorManager
{
public:
	EditorManager(EditorCommand* editorCommand) :
		m_EditorCommand(editorCommand)
	{
		m_MainMenu = std::make_unique<MainMenu>(this);
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
};

