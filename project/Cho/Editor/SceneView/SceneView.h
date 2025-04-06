#pragma once
#include "Editor/BaseEditor/BaseEditor.h"
class DebugCamera;
class SceneView : public BaseEditor
{
public:
	SceneView(EditorManager* editorManager,DebugCamera* debugCamera) :
		BaseEditor(editorManager), m_DebugCamera(debugCamera)
	{
	}
	~SceneView()
	{
	}
	void Initialize() override;
	void Update() override;
	void Window() override;
private:
	DebugCamera* m_DebugCamera = nullptr;
};

