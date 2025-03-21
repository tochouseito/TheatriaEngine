#include "pch.h"
#include "MainMenu.h"
#include <imgui.h>
#include "Cho/OS/Windows/WinApp/WinApp.h"
#include "Cho/Editor/EditorManager/EditorManager.h"

void MainMenu::Initialize()
{
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
    if (ImGui::BeginMenuBar())
    {

        // ファイルメニュー
        FileMenu();

        // 編集メニュー
        EditMenu();

        // レイアウトメニューの追加(Imguiのレイアウト)
        LayoutMenu();

        // Engine情報を表示
        EngineInfoMenu();

		// Helpメニュー
		HelpMenu();

        ImGui::EndMenuBar(); // メニューバーを終了
    }
}

void MainMenu::FileMenu()
{
}

void MainMenu::EditMenu()
{
    if (ImGui::BeginMenu("Edit"))
    {
        // 編集アクションをここに追加

        if (ImGui::MenuItem("Test AddGameObject"))
        {
            std::unique_ptr<AddGameObjectCommand> cmd = std::make_unique<AddGameObjectCommand>();
            m_EditorManager->GetEditorCommand()->ExecuteCommand(std::move(cmd));
        }

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
    if (ImGui::BeginMenu("レイアウト"))
    {
        if (ImGui::MenuItem("Show Style Editor"))
        {
            showStyleEditor = true;
        }
        if (ImGui::MenuItem("Show Metrics"))
        {
            showMetricsWindow = true;
        }
        if (ImGui::MenuItem("Show DemoWindow"))
        {
            showDemoImGui = true;
        }
        ImGui::EndMenu();
    }

    if (showStyleEditor)
    {
        ImGui::Begin("Style Editor", &showStyleEditor, windowFlags);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
    if (showMetricsWindow)
    {
        ImGui::ShowMetricsWindow(&showMetricsWindow);
    }
    if (showDemoImGui)
    {
        ImGui::ShowDemoWindow(&showDemoImGui);
    }
}

void MainMenu::EngineInfoMenu()
{
}

void MainMenu::HelpMenu()
{
    if (ImGui::BeginMenu("Help"))
    {
        if(ImGui::Button("https://tochouseito.github.io/ChoEngine_Docs/"))
        {
			WinApp::OpenWebURL(L"https://tochouseito.github.io/ChoEngine_Docs/");
        }
        
        ImGui::EndMenu();
    }
}
