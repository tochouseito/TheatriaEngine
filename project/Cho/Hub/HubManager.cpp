#include "pch.h"
#include "HubManager.h"
#include <imgui.h>
#include "Platform/FileSystem/FileSystem.h"
#include "Core/ChoLog/ChoLog.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "GameCore/GameCore.h"
#include "Editor/EditorManager/EditorManager.h"
#include "EngineCommand/EngineCommand.h"
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
    if (m_IsGameRuntime)
    {
        std::wstring selectedProjectName = L"RefLaser";
        // プロジェクトの読み込み
        FileSystem::LoadProjectFolder(selectedProjectName, m_pEngineCommand);
        // プロジェクトのパスを保存
        FileSystem::ScriptProject::LoadProjectPath(selectedProjectName);
        // ブランチを取得
        GetCurrentBranch();
        // プロジェクト選択後、Hubを終了
        m_IsRun = false; // プロジェクト選択後、Hubを終了
    }
}

void HubManager::Window()
{
	RenderFullScreenUI();
	ShowSidebar();
	ShowMainContent();
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
				// デフォルトのシーンを作成
                GameScene scene = m_pEngineCommand->GetGameCore()->GetSceneManager()->CreateDefaultScene();
                // エディタにセット、ロード
                m_pEngineCommand->GetEditorManager()->ChangeEditingScene(scene.GetName());
                // デフォルトのシーンを保存
                // std::filesystem::path projectPath = std::filesystem::path(L"GameProjects") / name;
                // プロジェクト名を保存
                FileSystem::m_sProjectName = name;
				// プロジェクトフォルダを作成
                FileSystem::ScriptProject::GenerateSolutionAndProject();
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
            // プロジェクトの読み込み
			FileSystem::LoadProjectFolder(selectedProjectName, m_pEngineCommand);
			// プロジェクトのパスを保存
			FileSystem::ScriptProject::LoadProjectPath(selectedProjectName);
			// ブランチを取得
            GetCurrentBranch();
			// プロジェクト選択後、Hubを終了
			m_IsRun = false; // プロジェクト選択後、Hubを終了
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

void HubManager::GetCurrentBranch()
{
	std::string gitFolder = "GameProjects/" + ConvertString(FileSystem::m_sProjectName) + "/.git";
	std::filesystem::path gitPath(gitFolder);
	m_GitHeadPath = gitPath / "HEAD";
    m_LastBranch = ReadCurrentBranch();
}

bool HubManager::CheckBranchChanged()
{
	// ブランチが変更されたかどうかを確認
	// .gitが存在しない場合は常に探して見つかったらそれをlastBranchにする
    if (m_LastBranch.empty())
    {
		m_LastBranch = ReadCurrentBranch();
        return false;
    }
    std::string currentBranch = ReadCurrentBranch();
    if (currentBranch != m_LastBranch)
    {
        m_LastBranch = currentBranch;
        return true;
    }
    return false;
}

void HubManager::ReloadProject()
{
    // プロジェクトの読み込み
    FileSystem::LoadProjectFolder(FileSystem::m_sProjectName, m_pEngineCommand);
    // ブランチを取得
    GetCurrentBranch();
    // プロジェクト選択後、Hubを終了
    m_IsRun = false; // プロジェクト選択後、Hubを終了
}
