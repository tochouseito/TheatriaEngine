#pragma once
#include <memory>
#include <imgui.h>
#include "EngineCommand/EngineCommand.h"
#include "Editor/MainMenu/MainMenu.h"
#include "Editor/Toolbar/Toolbar.h"
#include "Editor/DebugCamera/DebugCamera.h"
#include "Editor/SceneView/SceneView.h"
#include "Editor/GameView/GameView.h"
#include "Editor/Hierarchy/Hierarchy.h"
#include "Editor/Inspector/Inspector.h"
#include "Editor/AssetBrowser/AssetBrowser.h"
#include "Editor/Console/Console.h"
#include "Editor/EffectEditor/EffectEditor.h"
#include "Editor/EffectView/EffectView.h"

enum WorkSpaceType
{
	SceneEdit = 0,
	EffectEdit,
};

class EngineCommand;
class InputManager;
class EditorManager
{
public:
	EditorManager(EngineCommand* engineCommand,InputManager* inputManager) :
		m_EngineCommand(engineCommand), m_InputManager(inputManager)
	{
		m_Toolbar = std::make_unique<Toolbar>(this);
		m_MainMenu = std::make_unique<MainMenu>(this,m_Toolbar.get());
		m_DebugCamera = std::make_unique<DebugCamera>(this);
		m_SceneView = std::make_unique<SceneView>(this, m_DebugCamera.get());
		m_GameView = std::make_unique<GameView>(this);
		m_Hierarchy = std::make_unique<Hierarchy>(this);
		m_Inspector = std::make_unique<Inspector>(this);
		m_AssetBrowser = std::make_unique<AssetBrowser>(this);
		m_Console = std::make_unique<Console>(this);
		m_EffectEditor = std::make_unique<EffectEditor>(this);
		m_EffectView = std::make_unique<EffectView>(this);
	}
	~EditorManager()
	{
	}
	void Initialize();
	void Update();
	EngineCommand* GetEngineCommand() { return m_EngineCommand; }
	InputManager* GetInputManager() { return m_InputManager; }
	void SetWorkSpaceType(const std::string& typeName);
private:
	EngineCommand* m_EngineCommand = nullptr;
	InputManager* m_InputManager = nullptr;
	std::unique_ptr<Toolbar> m_Toolbar = nullptr;
	std::unique_ptr<MainMenu> m_MainMenu = nullptr;
	std::unique_ptr<DebugCamera> m_DebugCamera = nullptr;
	std::unique_ptr<SceneView> m_SceneView = nullptr;
	std::unique_ptr<GameView> m_GameView = nullptr;
	std::unique_ptr<Hierarchy> m_Hierarchy = nullptr;
	std::unique_ptr<Inspector> m_Inspector = nullptr;
	std::unique_ptr<AssetBrowser> m_AssetBrowser = nullptr;
	std::unique_ptr<Console> m_Console = nullptr;
	std::unique_ptr<EffectEditor> m_EffectEditor = nullptr;
	std::unique_ptr<EffectView> m_EffectView = nullptr;

	// ワークスペース
	WorkSpaceType m_WorkSpaceType = WorkSpaceType::SceneEdit;
};

