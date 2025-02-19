#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

// D3D12
class ResourceViewManager;

// Loader
class TextureLoader;
class ModelLoader;

// EditorManager
class EditorManager;

class FileView {
public:

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(EditorManager* editManager_,ResourceViewManager* rvManager, TextureLoader* texLoader,ModelLoader* modelLoader);

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    void UpdateFiles();

private:
    /// <summary>
    /// ディレクトリ内のファイルを取得
    /// </summary>
    std::vector<std::string> GetFilesInDirectory(const std::string& directory);


    /// <summary>
    /// ファイルブラウザをディレクトリ対応で表示
    /// </summary>
    void ShowFileViewWithDirectories();

    // フォルダ階層を表示して選択
    void ShowFolderHierarchy(const std::string& rootPath, std::string& selectedPath);

private:// メンバ変数
    /*D3D12*/
    ResourceViewManager* rvManager_ = nullptr;

    ModelLoader* modelLoader_ = nullptr;
    TextureLoader* texLoader_ = nullptr;

    /*エディタマネージャ*/
    EditorManager* editManager_ = nullptr;

    std::string currentDirectory;      // 現在のディレクトリ
    std::vector<std::string> files;    // ディレクトリ内のファイルリスト
    std::string selectedFile;          // 選択されたファイル

    std::string selectedFolder;

};

