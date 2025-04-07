#pragma once
#include "Editor/BaseEditor/BaseEditor.h"
class Toolbar;
class MainMenu : public BaseEditor
{
public:
	MainMenu(EditorManager* editorManager,Toolbar* toolbar):
		BaseEditor(editorManager), m_Toolbar(toolbar)
	{

	}
	~MainMenu()
	{

	}
	void Initialize() override;
	void Update() override;
	void Window() override;
private:
	void MenuBar();
	void FileMenu();
	void EditMenu();
	void LayoutMenu();
	void EngineInfoMenu();
	void HelpMenu();
	void PopupScriptName();

	Toolbar* m_Toolbar = nullptr;
	bool m_OpenScriptPopup = false;
};

