#include "PrecompiledHeader.h"
#include "FileController.h"
#include"imgui.h"

void FileController::Initialize()
{
}

void FileController::Update()
{
    if (ImGui::MenuItem("OpenEngineFile", "Ctrl+O")) { 
        OpenExplorerInCurrentDirectory();
    }
    if (ImGui::MenuItem("OpenGameFile")) {
        std::string path = ProjectRoot() + "\\" + ProjectName();
        OpenExplorerInDirectory(path);
    }
    if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Save action */ }
}

void FileController::OpenExplorerInCurrentDirectory() {
    // 現在の作業ディレクトリを取得
    char currentPath[MAX_PATH];
    if (GetCurrentDirectoryA(MAX_PATH, currentPath)) {
        // 現在の作業ディレクトリをエクスプローラーで開く
        ShellExecuteA(NULL, "open", currentPath, NULL, NULL, SW_SHOW);
    } else {
        std::cerr << "Error: Could not get current directory." << std::endl;
    }
}

void FileController::OpenExplorerInDirectory(const std::string& directoryPath) {
    ShellExecuteA(NULL, "open", directoryPath.c_str(), NULL, NULL, SW_SHOW);
}
