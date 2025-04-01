#pragma once
#include "Editor/BaseEditor/BaseEditor.h"
class AssetBrowser : public BaseEditor
{
public:
	AssetBrowser(EditorManager* editorManager) :
		BaseEditor(editorManager)
	{
	}
	~AssetBrowser()
	{
	}
	void Initialize() override;
	void Update() override;
	void Window() override;
};

