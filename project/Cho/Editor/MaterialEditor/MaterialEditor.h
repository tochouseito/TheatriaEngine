#pragma once
#include "Editor/BaseEditor/BaseEditor.h"
class MaterialEditor : public BaseEditor
{
public:
	MaterialEditor(EditorManager* editorManager) :
		BaseEditor(editorManager)
	{
	}
	~MaterialEditor()
	{
	}
	void Initialize() override;
	void Update() override;
	void Window() override;
};

