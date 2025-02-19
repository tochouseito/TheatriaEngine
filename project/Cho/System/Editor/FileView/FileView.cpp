#include "PrecompiledHeader.h"
#include"FileView.h"
#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"Load/TextureLoader/TextureLoader.h"
#include"Load/ModelLoader/ModelLoader.h"
#include"imgui.h"
#include"Editor/EditorManager/EditorManager.h"
#include"SystemState/SystemState.h"

void FileView::Initialize(EditorManager* editManager, ResourceViewManager* rvManager, TextureLoader* texLoader, ModelLoader* modelLoader)
{
    rvManager_ = rvManager;
    texLoader_ = texLoader;
    modelLoader_ = modelLoader;
    editManager_ = editManager;

    selectedFile.clear();
}

// 毎フレーム呼ばれる更新処理
void FileView::Update() {

    static bool exit = false;

    // 移動を無効にするフラグ
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;

    // フォルダビューウィンドウ
    ImGui::Begin("Folder Hierarchy", nullptr, windowFlags);
    if (exit) {
        ShowFolderHierarchy(ProjectRoot() + "\\" + ProjectName() + "\\Assets", selectedFolder);
    }
    ImGui::End();

    // ファイルビューウィンドウ
    ImGui::Begin("File View", nullptr, windowFlags);
    if (exit) {
        if (selectedFolder != currentDirectory) {
            currentDirectory = selectedFolder;
            files = GetFilesInDirectory(currentDirectory);
        }
        ShowFileViewWithDirectories();
    }
    ImGui::End();

    if (!exit) {
        std::string projectName = ProjectName();
        std::string projectRoot = ProjectRoot();

        if (projectName == "" || projectRoot == "") { return; }

        currentDirectory = projectRoot + "\\" + projectName + "\\" + "Assets\\";

        selectedFolder = currentDirectory;

        files = GetFilesInDirectory(currentDirectory);
        exit = true;
    }
}

void FileView::UpdateFiles()
{
    files = GetFilesInDirectory(currentDirectory);
}

// 指定されたディレクトリ内のファイルを取得
std::vector<std::string> FileView::GetFilesInDirectory(const std::string& directory) {
    std::vector<std::string> fileList;
    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            fileList.push_back(entry.path().generic_string());
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error accessing directory: " << e.what() << std::endl;
    }
    return fileList;
}
// ディレクトリ対応のファイルブラウザを表示
void FileView::ShowFileViewWithDirectories() {
    const float iconSize = 64.0f;   // アイコンのサイズ
    int itemsPerRow = 4;            // 1行に表示するアイコンの数
    int itemIndex = 0;              // アイテムのインデックス

    // ディレクトリとファイルをアイコンで表示
    for (const auto& file : files) {
        fs::path filePath(file);
        std::string fileName = filePath.filename().string();  // ファイル名のみを取得

        // 1行に表示するアイコン数に合わせて配置
        if (itemIndex > 0 && itemIndex % itemsPerRow != 0) {
            ImGui::SameLine();
        }

        ImGui::BeginGroup(); // アイコンとファイル名を一つのグループとしてまとめる

        if (fs::is_directory(filePath)) {
            // ディレクトリ用アイコン (単純にボタンとして表示)
            ImGui::Button((fileName + "/").c_str(), ImVec2(iconSize, iconSize));
        } else {
            // ファイル用アイコンの表示
            auto handle = rvManager_->GetHandle(texLoader_->GetTexture(fileName).rvIndex).GPUHandle;

            if (handle.ptr != 0) {
                // テクスチャアイコンを表示
                ImGui::Image((void*)(intptr_t)handle.ptr, ImVec2(iconSize, iconSize));
            } else {
                // テクスチャがない場合は代替ボタンを表示
                ImGui::Button(fileName.c_str(), ImVec2(iconSize, iconSize));
            }
        }

        // アイコンの下にファイル名を表示
        ImGui::TextWrapped("%s", fileName.c_str());

        ImGui::EndGroup(); // グループを終了

        itemIndex++;
    }
}

// フォルダ階層を表示して選択

void FileView::ShowFolderHierarchy(const std::string& rootPath, std::string& selectedPath) {
    // 再帰関数でフォルダ階層を表示
    std::function<void(const fs::path&)> displayFolder = [&](const fs::path& folderPath) {
        std::string folderName = folderPath.filename().string();
        ImGuiTreeNodeFlags nodeFlags = (folderPath == selectedPath) ? ImGuiTreeNodeFlags_Selected : 0;

        if (ImGui::TreeNodeEx(folderName.c_str(), nodeFlags)) {
            if (ImGui::IsItemClicked()) {
                selectedPath = folderPath.generic_string(); // スラッシュを `/` に統一
            }

            for (const auto& entry : fs::directory_iterator(folderPath)) {
                if (entry.is_directory()) {
                    displayFolder(entry.path());
                }
            }
            ImGui::TreePop();
        } else if (ImGui::IsItemClicked()) {
            selectedPath = folderPath.generic_string(); // スラッシュを `/` に統一
        }
        };

    fs::path root(rootPath);
    if (fs::exists(root) && fs::is_directory(root)) {
        displayFolder(root);
    } else {
        ImGui::Text("Invalid root path.");
    }
}