#include "pch.h"
#include "AssetBrowser.h"
#include "Editor/EditorManager/EditorManager.h"
#include "Core/Utility/FontCode.h"
#include "OS/Windows/WinApp/WinApp.h"
#include "Platform/FileSystem/FileSystem.h"

void AssetBrowser::Initialize()
{

}

void AssetBrowser::Update()
{
	Window();
}

void AssetBrowser::Window()
{
	// 移動を無効にするフラグ
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;
	ImGui::Begin("AssetBrowser", nullptr, windowFlags);

	ImGui::Text("%s %s", ICON_MATERIAL_FOLDER, currentPath.filename().string().c_str());
    ImGui::Separator();
    ImGui::Columns(2);
    std::wstring projectPath = L"GameProjects/" + FileSystem::m_sProjectName;
    DrawDirectoryTree(ConvertString(projectPath));
    ImGui::NextColumn();
    if (!currentPath.empty())
    {
        DrawAssetGrid(currentPath);
    }

    ImGui::Columns(1);

	ImGui::End();
}


void AssetBrowser::DrawDirectoryTree(const std::filesystem::path& path)
{
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (!entry.is_directory()) continue;

        std::string label = std::string(ICON_MATERIAL_FOLDER) + " " + entry.path().filename().string();
        bool opened = ImGui::TreeNode(label.c_str());

        if (ImGui::IsItemClicked())
            currentPath = entry.path();

        if (opened)
        {
            DrawDirectoryTree(entry.path());
            ImGui::TreePop();
        }
    }
}

void AssetBrowser::DrawAssetGrid(const std::filesystem::path& path)
{
    constexpr float iconSize = 64.0f;
    constexpr float padding = 8.0f;
    float cellSize = iconSize + padding;
    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = static_cast<int>(panelWidth / cellSize);
    if (columnCount < 1) columnCount = 1;

    ImGui::Columns(columnCount, nullptr, false);

    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        std::string icon = GetIconForEntry(entry);

        ImGui::Button(icon.c_str(), ImVec2(iconSize, iconSize));
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            if (entry.is_directory())
                currentPath = entry.path();
            else
                ; // OpenAsset(entry.path()); // 外部処理へ
        }

        ImGui::TextWrapped("%s", entry.path().filename().string().c_str());
        ImGui::NextColumn();
    }

    ImGui::Columns(1);
}

const char* AssetBrowser::GetIconForEntry(const std::filesystem::directory_entry& entry)
{
    if (entry.is_directory()) return ICON_MATERIAL_FOLDER;

    std::string ext = entry.path().extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });


    if (ext == ".obj" || ext == ".gltf" || ext == ".glb") return ICON_MATERIAL_MODEL;
    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp") return ICON_MATERIAL_IMAGE;
    if (ext == ".wav" || ext == ".mp3" || ext == ".ogg") return ICON_MATERIAL_AUDIO;
    if (ext == ".effect") return ICON_MATERIAL_EFFECT;
    if (ext == ".param" || ext == ".json") return ICON_MATERIAL_SETTING;
    if (ext == ".cpp" || ext == ".h") return ICON_MATERIAL_SCRIPT;
    if (ext == ".scene") return ICON_MATERIAL_SCENE;

    return ICON_MATERIAL_FILE;
}