#pragma once
#include "Editor/BaseEditor/BaseEditor.h"
class MainMenu : public BaseEditor
{
public:
	MainMenu(EditorManager* editorManager):
		BaseEditor(editorManager)
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
};

