#include "pch.h"
#include "MainMenu.h"
#include <imgui.h>
#include "Cho/OS/Windows/WinApp/WinApp.h"
#include "Cho/Editor/EditorManager/EditorManager.h"
#include "GameCore/GameObject/GameObject.h"
#include "Platform/FileSystem/FileSystem.h"

void MainMenu::Initialize()
{
}

void MainMenu::Update()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        Window();
        PopupScriptName();
    }
}

void MainMenu::Window()
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
    // ツールバー
    m_Toolbar->Update();

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
    if (ImGui::BeginMenu("ファイル"))
    {
        // プロジェクトの保存
		if (ImGui::MenuItem("プロジェクトの保存"))
		{
			m_EditorCommand->SaveProjectFile(FileSystem::m_sProjectName);
		}
        // スクリプトプロジェクトの生成
		if (ImGui::MenuItem("スクリプトプロジェクトの生成"))
		{
            FileSystem::ScriptProject::GenerateSolutionAndProject();
		}
        ImGui::EndMenu();
    }
}

void MainMenu::EditMenu()
{
    if (ImGui::BeginMenu("追加"))
    {
        // 編集アクションをここに追加
        if (ImGui::MenuItem("3Dオブジェクト"))
        {
            std::unique_ptr<Add3DObjectCommand> add3DObject = std::make_unique<Add3DObjectCommand>();
            m_EditorCommand->ExecuteCommand(std::move(add3DObject));
        }
		if (ImGui::MenuItem("カメラオブジェクト"))
		{
			std::unique_ptr<AddCameraObjectCommand> addCameraObject = std::make_unique<AddCameraObjectCommand>();
			m_EditorCommand->ExecuteCommand(std::move(addCameraObject));
		}

        // メニュー選択でポップアップを開く
        if (ImGui::MenuItem("スクリプト"))
        {
            m_OpenScriptPopup = true;
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

void MainMenu::PopupScriptName()
{
    // スクリプト名バッファ
    static char scriptNameBuffer[64] = "";
    if (m_OpenScriptPopup)
    {
        std::memset(scriptNameBuffer, 0, sizeof(scriptNameBuffer)); // 初期化
        ImGui::OpenPopup("ScriptNamePopup");
        m_OpenScriptPopup = false; // 一度だけ開くように
    }
    if (ImGui::BeginPopupModal("ScriptNamePopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("スクリプト名を入力してください（A-Z, a-z）:");
        ImGui::InputText("##ScriptName", scriptNameBuffer, IM_ARRAYSIZE(scriptNameBuffer),
            ImGuiInputTextFlags_CallbackCharFilter,
            [](ImGuiInputTextCallbackData* data) -> int {
                if ((data->EventChar >= 'a' && data->EventChar <= 'z') ||
                    (data->EventChar >= 'A' && data->EventChar <= 'Z'))
                {
                    return 0;
                }
                return 1;
            });

        if (ImGui::Button("OK"))
        {
            std::string scriptName = scriptNameBuffer;
            m_EditorCommand->GenerateScript(scriptName);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("キャンセル"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
