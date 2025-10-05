#pragma once
#include "Editor/BaseEditor/BaseEditor.h"

class Console : public BaseEditor
{
public:



	Console(EditorManager* editorManager) :
		BaseEditor(editorManager)
	{
	}
	~Console()
	{
	}
	void Initialize() override;
	void Update() override;
	void Window() override;
};

