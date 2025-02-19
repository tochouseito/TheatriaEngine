#include "PrecompiledHeader.h"
#include "EditorManager.h"

// ImGui
#include"imgui.h"

#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"D3D12/RTVManager/RTVManager.h"
#include"D3D12/DrawExecution/DrawExecution.h"

#include"Input/InputManager.h"

// SceneManager
#include"Scene/SceneManager/SceneManager.h"

#include"ECS/GameObject/GameObject.h"

void EditorManager::Initialize(
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
)
{
    // デバッグカメラを作成
    debugCamera = std::make_unique<DebugCamera>();
    debugCamera->Initialize(componentManager,systemManager,inputManager);

	// MainMenu
	mainMenu = std::make_unique<MainMenu>();
	mainMenu->Initialize(
        rvManager,
        rtvManager,
        drawExe,
        entityManager,
        componentManager,
        systemManager,
        prefabManager,
        sceneManager,
        this
        );

    // SceneView
    sceneView = std::make_unique<SceneView>();
    sceneView->Initialize(drawExe->GetDebugFinalRenTexInd(), win, rvManager, inputManager, debugCamera.get());

    // PopupMenu
    popupMenu = std::make_unique<PopupMenu>();
    popupMenu->Initialize(
        rvManager,
        rtvManager,
        drawExe,
        entityManager,
        componentManager,
        systemManager,
        prefabManager,
        sceneManager,
        this,
        scriptManager
    );

	// FileView
	fileView = std::make_unique<FileView>();
    fileView->Initialize(this, rvManager, texLoader,modelLoader);

    // InfoView
    infoView = std::make_unique<InfoView>();
    infoView->Initialize(
        rvManager,
        rtvManager,
        drawExe,
        texLoader,
        entityManager,
        componentManager,
        systemManager,
        prefabManager,
        sceneManager,
        this,
        scriptManager
    );

    // ObjectsList
    objectsList = std::make_unique<ObjectsList>();
    objectsList->Initialize(
        rvManager,
        rtvManager,
        drawExe,
        entityManager,
        componentManager,
        systemManager,
        prefabManager,
        sceneManager,
        this
    );

    toolBar = std::make_unique<ToolBar>();
    toolBar->Initialize(
        rvManager,
        rtvManager,
        drawExe,
        entityManager,
        componentManager,
        systemManager,
        prefabManager,
        sceneManager,
        this,
        scriptManager
    );

    // WinApp
    win_ = win;

    // D3D12
    rvManager_ = rvManager;
    rtvManager_ = rtvManager;
    drawExe_ = drawExe;

    // ECS
    entityManager_ = entityManager;
    componentManager_ = componentManager;
    systemManager_ = systemManager;
    prefabManager_ = prefabManager;

    // Input
	inputManager_ = inputManager;

    // SceneManager
    sceneManager_ = sceneManager;

    // ScriptManager
    scriptManager_ = scriptManager;
}

void EditorManager::Update()
{
    // 全体のImGuiウィンドウ
    //UpdateMainWindow();

    // MainMenu
    mainMenu->Update();

    // SceneView
    sceneView->Update(drawExe_->GetFinalRenderTexIndex());

    // PopupMenu
    popupMenu->Update(sceneView->IsWindowHovered());

    // FileView
    fileView->Update();

    // InfoView
    infoView->Update();

    // ObjectsList
    objectsList->Update();

    // ToolBar
    toolBar->Update();

    // DebugCamera
    debugCamera->Update(sceneView->IsWindowHovered());

    ImGuiIO& io = ImGui::GetIO();
    Vector2 vec = inputManager_->CheckAndWarpMouse();
    if (vec.x != 0.0f || vec.y != 0.0f) {
        io.MousePos.x += vec.x;
        io.MousePos.y += vec.y;
    }
    if (!sceneView->IsWindowHovered()) {
        ShowCursor(true);
    }
}

void EditorManager::CreateObject(const ObjectType& type)
{
    std::string name = "NewGameObject";

    // 入力された名前を使用してオブジェクトを追加
    name = sceneManager_->AddGameObject(name,type);
    SetSelectedGOName(name);
    SetSelectedGO(sceneManager_->GetGameObject(name));
}

void EditorManager::UpdateFiles()
{
    fileView->UpdateFiles();
}

void EditorManager::UpdateMainWindow()
{
	// ウィンドウの位置を指定する
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	// ウィンドウのサイズを指定する
	ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_Always);

	// フラグでウィンドウがフォーカスされても前面に持ってこないように設定
	ImGui::Begin(
		"Cho Editor",
		nullptr,
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus
	);
	// ウィンドウの中に表示するUI要素
	ImGui::End();
}
