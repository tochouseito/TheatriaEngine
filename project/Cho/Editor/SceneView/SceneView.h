#pragma once
class EditorManager;
class SceneView
{
public:
	SceneView(EditorManager* editorManager) :
		m_EditorManager(editorManager)
	{
	}
	~SceneView()
	{
	}
	void Initialize();
	void Update();
private:
	void SceneViewWindow();
	void GameViewWindow();

	EditorManager* m_EditorManager = nullptr;
};

