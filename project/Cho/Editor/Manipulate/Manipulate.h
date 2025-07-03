#pragma once
#include "Editor/BaseEditor/BaseEditor.h"

// 前方宣言
class DebugCamera;

class Manipulate : public BaseEditor
{
public:
	Manipulate(EditorManager* editorManager,DebugCamera* dCamera) :
		BaseEditor(editorManager), m_dCamera(dCamera)
	{

	}
	~Manipulate()
	{

	}
	// BaseEditor を介して継承されました
	void Initialize() override;
	void Update() override;
	void Window() override;
private:
	DebugCamera* m_dCamera = nullptr;
};

