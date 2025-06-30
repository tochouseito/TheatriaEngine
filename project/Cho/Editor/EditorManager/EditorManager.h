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
#include "Editor/EffectHierarchy/EffectHierarchy.h"

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
	EditorManager(EngineCommand* engineCommand,InputManager* inputManager);
	~EditorManager()
	{
	}
	void Initialize();
	void Update();
	EngineCommand* GetEngineCommand() { return m_EngineCommand; }
	InputManager* GetInputManager() { return m_InputManager; }
	void SetWorkSpaceType(const std::string& typeName);
	WorkSpaceType GetWorkSpaceType() { return m_WorkSpaceType; }

	GameObject* GetSelectedGameObject() const { return m_SelectedGameObject; }
	void SetSelectedGameObject(GameObject* gameObject) { m_SelectedGameObject = gameObject; }

	// EffectEditor
	std::optional<uint32_t> GetEffectEntity() const { return m_EffectEntity; }
	void SetEffectEntity(std::optional<uint32_t> entity) { m_EffectEntity = entity; }
	std::optional<uint32_t> GetEffectNodeID() const { return m_EffectNodeID; }
	void SetEffectNodeIndex(std::optional<uint32_t> nodeID) { m_EffectNodeID = nodeID; }
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
	std::unique_ptr<EffectHierarchy> m_EffectHierarchy = nullptr;

	// 選択中のオブジェクト
	GameObject* m_SelectedGameObject = nullptr;
	// 編集中のエフェクトEntity
	std::optional<uint32_t> m_EffectEntity = std::nullopt;
	// 選択中のEffectNode
	std::optional<uint32_t> m_EffectNodeID = std::nullopt;

	// ワークスペース
	WorkSpaceType m_WorkSpaceType = WorkSpaceType::SceneEdit;
};

