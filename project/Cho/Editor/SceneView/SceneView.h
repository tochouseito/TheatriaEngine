#pragma once
#include "Editor/BaseEditor/BaseEditor.h"
class SceneView : public BaseEditor
{
public:
	SceneView(EditorManager* editorManager) :
		BaseEditor(editorManager)
	{
	}
	~SceneView()
	{
	}
	void Initialize() override;
	void Update() override;
	void Window() override;
};

