#pragma once
#include "Editor/BaseEditor/BaseEditor.h"
class EffectEditor : public BaseEditor
{
public:
	EffectEditor(EditorManager* editorManager) :
		BaseEditor(editorManager)
	{
	}
	~EffectEditor()
	{
	}
	void Initialize() override;
	void Update() override;
	void Window() override;
private:
	void ControlWindow();
};

