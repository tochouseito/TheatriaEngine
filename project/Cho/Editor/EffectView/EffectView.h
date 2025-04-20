#pragma once
#include "Editor/BaseEditor/BaseEditor.h"
class EffectView : public BaseEditor
{
public:
	EffectView(EditorManager* editorManager) :
		BaseEditor(editorManager)
	{
	}
	~EffectView()
	{
	}
	void Initialize() override;
	void Update() override;
	void Window() override;
};

