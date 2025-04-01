#pragma once
#include <memory>
#include <imgui.h>

#include "Editor/EditorCommand/EditorCommand.h"
#include "Editor/MainMenu/MainMenu.h"
#include "Editor/SceneView/SceneView.h"
#include "Editor/GameView/GameView.h"
#include "Editor/Hierarchy/Hierarchy.h"
#include "Editor/Inspector/Inspector.h"
#include "Editor/AssetBrowser/AssetBrowser.h"
#include "Editor/Console/Console.h"

class EditorManager
{
public:
	EditorManager(EditorCommand* editorCommand) :
		m_EditorCommand(editorCommand)
	{
		m_MainMenu = std::make_unique<MainMenu>(this);
		m_SceneView = std::make_unique<SceneView>(this);
		m_GameView = std::make_unique<GameView>(this);
		m_Hierarchy = std::make_unique<Hierarchy>(this);
		m_Inspector = std::make_unique<Inspector>(this);
		m_AssetBrowser = std::make_unique<AssetBrowser>(this);
		m_Console = std::make_unique<Console>(this);
	}
	~EditorManager()
	{

	}
	void Initialize();
	void Update();

	EditorCommand* GetEditorCommand() { return m_EditorCommand; }
private:
	EditorCommand* m_EditorCommand = nullptr;
	std::unique_ptr<MainMenu> m_MainMenu = nullptr;
	std::unique_ptr<SceneView> m_SceneView = nullptr;
	std::unique_ptr<GameView> m_GameView = nullptr;
	std::unique_ptr<Hierarchy> m_Hierarchy = nullptr;
	std::unique_ptr<Inspector> m_Inspector = nullptr;
	std::unique_ptr<AssetBrowser> m_AssetBrowser = nullptr;
	std::unique_ptr<Console> m_Console = nullptr;
};

