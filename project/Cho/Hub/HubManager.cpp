#include "pch.h"
#include "HubManager.h"
#include <imgui.h>
#include "Platform/FileSystem/FileSystem.h"
#include "Core/ChoLog/ChoLog.h"
using namespace Cho;

void HubManager::Initialize()
{
}

void HubManager::Update()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        Window();
    }
}

void HubManager::Window()
{
	RenderFullScreenUI();
	ShowSidebar();
	ShowMainContent();

    //// ビューポート全体をカバーするドックスペースを作成
    //ImGuiViewport* viewport = ImGui::GetMainViewport();
    //ImGui::SetNextWindowPos(viewport->Pos); // 次のウィンドウの位置をメインビューポートの位置に設定
    //ImGui::SetNextWindowSize(viewport->Size); // 次のウィンドウのサイズをメインビューポートのサイズに設定
    //ImGui::SetNextWindowViewport(viewport->ID); // ビューポートIDをメインビューポートに設定

    //// タイトルバーを削除し、リサイズや移動を防止し、背景のみとするウィンドウフラグを設定
    //ImGuiWindowFlags window_flags =
    //    ImGuiWindowFlags_NoTitleBar |
    //    ImGuiWindowFlags_NoCollapse |
    //    ImGuiWindowFlags_NoResize |
    //    ImGuiWindowFlags_NoMove |
    //    ImGuiWindowFlags_NoBringToFrontOnFocus |
    //    ImGuiWindowFlags_NoNavFocus |
    //    ImGuiWindowFlags_MenuBar;

    //// ウィンドウの丸みとボーダーをなくして、シームレスなドッキング外観にする
    //ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    //ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    //ImGui::Begin("Hub DockSpace Window", nullptr, window_flags); // ドックスペースとして機能する新しいウィンドウを開始
    //ImGui::PopStyleVar(2); // 先ほどプッシュしたスタイル変数を2つポップする

    //// ウィンドウ内にドックスペースを作成
    //ImGuiID dockspace_id = ImGui::GetID("HubDockspace");
    //ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    //ImGui::End(); // ドックスペースウィンドウを終了
}

void HubManager::RenderFullScreenUI()
{
    // ウィンドウフラグ
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar;

    // ウィンドウサイズ = ディスプレイサイズ
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    if (ImGui::Begin("FullScreenUI", nullptr, window_flags))
    {
        // メニューバー
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::MenuItem("Open Project");
                ImGui::MenuItem("Exit");
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // DockSpace（中央のレイアウト分割領域）
        ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f),
            ImGuiDockNodeFlags_PassthruCentralNode |
            ImGuiDockNodeFlags_NoDockingInCentralNode);
    }
    ImGui::End();

    ImGui::PopStyleVar(2);
}

void HubManager::ShowSidebar()
{
    ImGui::Begin("Sidebar");

    ImGui::Text("Projects");

    static char newProjectName[256] = "";
    static bool showErrorPopup = false;
    static std::string errorMessage;

    ImGui::Text("New Project Name:");
    ImGui::InputText("##NewProject", newProjectName, sizeof(newProjectName));

    if (ImGui::Button("Create New Project"))
    {
		std::wstring name = ConvertString(newProjectName);
        if (!name.empty())
        {
            bool created = FileSystem::CreateNewProjectFolder(name);
            if (created)
            {
                std::wcout << L"プロジェクト作成成功: " << name << std::endl;
				m_IsRun = false; // プロジェクト作成後、Hubを終了
            } else
            {
                // エラー発生（すでに存在・作成失敗など）
                errorMessage = "プロジェクト作成に失敗しました。同名のプロジェクトが存在するか、権限がありません。";
                ImGui::OpenPopup("ErrorPopup");
            }
        }
    }

    // エラーポップアップ
    if (ImGui::BeginPopupModal("ErrorPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextWrapped("%s", errorMessage.c_str());
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::Separator();

    static std::vector<std::wstring> projects = FileSystem::GetProjectFolders();
    static int selectedIndex = -1;

    for (int i = 0; i < projects.size(); ++i)
    {
		std::string utf8Name = ConvertString(projects[i]);
        if (ImGui::Selectable(utf8Name.c_str(), selectedIndex == i))
        {
            selectedIndex = i;
            std::wstring selectedProjectName = projects[i];
        }
    }

    ImGui::End();
}


void HubManager::ShowMainContent()
{
    ImGui::Begin("MainView");
    ImGui::Text("Welcome to ChoHub!");
    // 中央の画面など
    ImGui::End();
}
