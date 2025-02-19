#pragma once

// C++
#include<memory>

// Editor
#include"Editor/MainMenu/MainMenu.h"
#include"Editor/PopupMenu/PopupMenu.h"
#include"Editor/FileView/FileView.h"
#include"Editor/SceneView/SceneView.h"
#include"Editor/InfoView/InfoView.h"
#include"Editor/ObjectsList/ObjectsList.h"
#include"Editor/ToolBar/ToolBar.h"

// DebugCamera
#include"Editor/DebugCamera/DebugCamera.h"

#include"ObjectType/ObjectType.h"

struct EffectNode;

// WinApp
class WinApp;

// D3D12
class ResourceViewManager;
class RTVManager;
class DrawExecution;

// Loader
class TextureLoader;
class ModelLoader;

// ScriptManager
class ScriptManager;

// ECS
class EntityManager;
class ComponentManager;
class SystemManager;
class PrefabManager;

// Input
class InputManager;

// GameObject
class GameObject;

// SceneManager
class SceneManager;

class EditorManager
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(
		WinApp* win,
		ResourceViewManager* rvManager,
		RTVManager* rtvManager,
		DrawExecution* drawExe,
		TextureLoader* texLoader,
		ModelLoader* modelLoader,
		EntityManager* entityManager,
		ComponentManager* componentManager,
		SystemManager* systemManager,
		PrefabManager* prefabManager,
		InputManager* inputManager,
		SceneManager* sceneManager,
		ScriptManager* scriptManager
	);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void SetSelectedGO(GameObject* gameObject) { selectedGameObject_ = gameObject; }
	void SetSelectedGOName(const std::string& name) { selectedGamaObjectName_ = name; }
	void SetSelectedEffectNode(EffectNode* selected) { selectedEffectNode_ = selected; }

	GameObject* GetSelectedGO()const { return selectedGameObject_; }
	std::string GetSelectedGOName()const { return selectedGamaObjectName_; }
	EffectNode* GetSelectedEffectNode()const { return selectedEffectNode_; }
	
	void CreateObject(const ObjectType& type);

	void UpdateFiles();

	// debugCameraComponentGetter
	const CameraComponent& GetDebugCameraComp() { return debugCamera->GetCameraComponent(); }

private:// メンバ関数

	void UpdateMainWindow();

private:

	std::unique_ptr<MainMenu> mainMenu = nullptr;
	std::unique_ptr<PopupMenu> popupMenu = nullptr;
	std::unique_ptr<FileView> fileView = nullptr;
	std::unique_ptr<SceneView> sceneView = nullptr;
	std::unique_ptr<InfoView> infoView = nullptr;
	std::unique_ptr<ObjectsList> objectsList = nullptr;
	std::unique_ptr<ToolBar> toolBar = nullptr;
	std::unique_ptr<DebugCamera> debugCamera = nullptr;

	/*WinApp*/
	WinApp* win_ = nullptr;

	/*D3D12*/
	ResourceViewManager* rvManager_ = nullptr;
	RTVManager* rtvManager_ = nullptr;
	DrawExecution* drawExe_ = nullptr;

	/*ECS*/
	EntityManager* entityManager_ = nullptr;
	ComponentManager* componentManager_ = nullptr;
	SystemManager* systemManager_ = nullptr;
	PrefabManager* prefabManager_ = nullptr;
	SceneManager* sceneManager_ = nullptr;

	/*Input*/
	InputManager* inputManager_ = nullptr;

	/*ScriptManager*/
	ScriptManager* scriptManager_ = nullptr;

	std::string selectedGamaObjectName_;
	GameObject* selectedGameObject_ = nullptr;

	EffectNode* selectedEffectNode_ = nullptr;
};

