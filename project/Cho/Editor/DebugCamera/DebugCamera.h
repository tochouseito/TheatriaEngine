#pragma once
#include "Core/Utility/Components.h"
#include "Core/Utility/CompBufferData.h"
class EditorManager;
class DebugCamera
{
public:
	DebugCamera(EditorManager* editorManager)
		:m_pEditorManager(editorManager)
	{

	}
	~DebugCamera()
	{

	}
	void Initialize();
	void Update();
private:
	EditorManager* m_pEditorManager = nullptr;
	CameraComponent m_CameraComponent;
	BUFFER_DATA_VIEWPROJECTION m_ViewProjection;
};

