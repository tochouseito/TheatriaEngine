#include "PrecompiledHeader.h"
#include "MainMenu.h"
#include"imgui.h"
#include"ChoMath.h"

#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"D3D12/RTVManager/RTVManager.h"
#include"D3D12/DrawExecution/DrawExecution.h"

// SceneManager
#include"Scene/SceneManager/SceneManager.h"

#include"ECS/GameObject/GameObject.h"

#include"Editor/EditorManager/EditorManager.h"

// C++
#include<string>

void MainMenu::Initialize(
    ResourceViewManager* rvManager,
    RTVManager* rtvManager,
    DrawExecution* drawExe,
    EntityManager* entityManager,
    ComponentManager* componentManager,
    SystemManager* systemManager,
    PrefabManager* prefabManager,
    SceneManager* sceneManager,
    EditorManager* editManager
)
{
    // D3D12
    rvManager_ = rvManager;
    rtvManager_ = rtvManager;
    drawExe_ = drawExe;

    // ECS
    entityManager_ = entityManager;
    componentManager_ = componentManager;
    systemManager_ = systemManager;
    prefabManager_ = prefabManager;

    // SceneManager
    sceneManager_ = sceneManager;

    editManager_ = editManager;

    fileController = std::make_unique<FileController>();
    fileController->Initialize();
}

void MainMenu::Update()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        BackWindow();
    }
}

void MainMenu::BackWindow()
{
    // ビューポート全体をカバーするドックスペースを作成
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos); // 次のウィンドウの位置をメインビューポートの位置に設定
    ImGui::SetNextWindowSize(viewport->Size); // 次のウィンドウのサイズをメインビューポートのサイズに設定
    ImGui::SetNextWindowViewport(viewport->ID); // ビューポートIDをメインビューポートに設定

    // タイトルバーを削除し、リサイズや移動を防止し、背景のみとするウィンドウフラグを設定
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_MenuBar;

    // ウィンドウの丸みとボーダーをなくして、シームレスなドッキング外観にする
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("DockSpace Window", nullptr, window_flags); // ドックスペースとして機能する新しいウィンドウを開始
    ImGui::PopStyleVar(2); // 先ほどプッシュしたスタイル変数を2つポップする

    // メニューバー
    MenuBar();

    // ウィンドウ内にドックスペースを作成
    ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    ImGui::End(); // ドックスペースウィンドウを終了
}

void MainMenu::MenuBar()
{
    // メニューバー
    if (ImGui::BeginMenuBar()) {

        // ファイルメニュー
        FileMenu();

        // 編集メニュー
        EditMenu();

        // レイアウトメニューの追加(Imguiのレイアウト)
        LayoutMenu();

        // Engine情報を表示
        EngineInfoMenu();

        ImGui::EndMenuBar(); // メニューバーを終了
    }
}

void MainMenu::FileMenu()
{
    if (ImGui::BeginMenu("File")) {
        fileController->Update();
        ImGui::EndMenu();
    }
}

void MainMenu::EditMenu()
{
    if (ImGui::BeginMenu("Edit"))
    {
        // 編集アクションをここに追加

        if (ImGui::MenuItem("Item 3")) { /* Item 3の処理 */ }

        ImGui::EndMenu(); // 「Edit」メニューを終了
    }
}

void MainMenu::LayoutMenu()
{
    static bool showStyleEditor = false;
    static bool showMetricsWindow = false;
    static bool showDemoImGui = false;
    // ドッキング解除を無効にするフラグ
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;

    if (ImGui::BeginMenu("Layout")) {
        if (ImGui::MenuItem("Show Style Editor")) {
            showStyleEditor = true;
        }
        if (ImGui::MenuItem("Show Metrics")) {
            showMetricsWindow = true;
        }
        if (ImGui::MenuItem("Show DemoWindow")) {
            showDemoImGui = true;
        }
        ImGui::EndMenu();
    }

    if (showStyleEditor) {
        ImGui::Begin("Style Editor", &showStyleEditor,windowFlags);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
    if (showMetricsWindow) {
        ImGui::ShowMetricsWindow(&showMetricsWindow);
    }
    if (showDemoImGui) {
        ImGui::ShowDemoWindow(&showDemoImGui);
    }
}

void MainMenu::EngineInfoMenu()
{
    if (ImGui::BeginMenu("EngineInfo")) {

        // Viewの使用数
        ImGui::SeparatorText("Descriptor");

        {
            std::string overlay = "C.S.U.View:" + std::to_string(rvManager_->GetNowIndex()) + " / " + std::to_string(ResourceViewManager::GetMaxIndex());

            ImGui::ProgressBar(
                static_cast<float>(rvManager_->GetNowIndex()) / static_cast<float>(ResourceViewManager::GetMaxIndex()),
                ImVec2(-1.0f, 0.0f),
                overlay.c_str()
            );
        }

        {
            std::string overlay = "RTV:" + std::to_string(rtvManager_->GetNowIndex()) + " / " + std::to_string(RTVManager::GetMaxIndex());

            ImGui::ProgressBar(
                static_cast<float>(rtvManager_->GetNowIndex()) / static_cast<float>(RTVManager::GetMaxIndex()),
                ImVec2(-1.0f, 0.0f),
                overlay.c_str()
            );
        }

        // フレームレートを表示
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Text("fps: %.1f", NowFrameRate());
        ImGui::Text("DeltaTime: %.5f", DeltaTime());

        ImGui::SeparatorText("EngineCurrentPath");
        std::string currentPath = fs::current_path().string();
        ImGui::Text("Current Path: %s", currentPath.c_str()); // パスを表示

        ImGui::EndMenu();
    }
}