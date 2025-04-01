#pragma once
class EditorCommand;
class EditorManager;
class BaseEditor
{
public:
	BaseEditor(EditorManager* editorManager);
	~BaseEditor()
	{
	}
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Window() = 0;
protected:
	EditorManager* m_EditorManager = nullptr;
	EditorCommand* m_EditorCommand = nullptr;
};

