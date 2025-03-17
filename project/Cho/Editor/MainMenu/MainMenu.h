#pragma once
class EditorManager;
class MainMenu
{
public:
	MainMenu(EditorManager* editorManager):
		m_EditorManager(editorManager)
	{

	}
	~MainMenu()
	{

	}
	void Initialize();
	void Update();
private:
	void BackWindow();
	void MenuBar();
	void FileMenu();
	void EditMenu();
	void LayoutMenu();
	void EngineInfoMenu();
	void HelpMenu();

	EditorManager* m_EditorManager = nullptr;
};

