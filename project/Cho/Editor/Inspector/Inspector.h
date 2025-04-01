#pragma once
#include "Editor/BaseEditor/BaseEditor.h"
class Inspector : public BaseEditor
{
public:
	Inspector(EditorManager* editorManager) :
		BaseEditor(editorManager)
	{
	}
	~Inspector()
	{
	}
	void Initialize() override;
	void Update() override;
	void Window() override;
};

