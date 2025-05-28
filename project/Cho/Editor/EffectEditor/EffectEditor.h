#pragma once
#include "Editor/BaseEditor/BaseEditor.h"
#include "Core/Utility/EffectStruct.h"
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
	// RandValue用ImGui
	static bool DragRandVector3(const char* label, RandVector3* v, float v_speed, float v_min, float v_max);
private:
	void ControlWindow();
};

