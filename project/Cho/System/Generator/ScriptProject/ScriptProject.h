#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <filesystem>
#include <windows.h>
#include <winsock.h>
#include <wingdi.h>
#include <shellapi.h>
#include <ole2.h>
#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp.lib")

namespace fs = std::filesystem;

class ScriptProject {
public:
    static void Update();                     // スクリプトの更新
    static void GenerateSolutionAndProject();
    static void OpenVisualStudio();                                        // Visual Studio を開く
    static void GenerateScriptTemplate(const std::string& scriptName);
    static void LoadScriptDLL();
    static HMODULE GetDLL() { return scriptLibrary; }

private:
    static std::string GenerateGUID();                                     // GUID 生成
    static void UpdateVcxproj(const std::string& projectGuid,const std::string& projectName);
    static void UpdateFilters();
    static std::string FindSolutionPath();

    static std::unordered_map<std::string, std::vector<std::string>> scripts;  // 更新されたスクリプトのリスト
    static HMODULE scriptLibrary;
    static std::string projectGUID;
    static std::string slnPath;
    static std::string vcxprojPath;
    static std::string filtersPath;

    static std::string ConvertToWindowsPath(const std::string& path);
    static bool LoadPDBForDLL(const std::string& dllPath, const std::string& pdbPath);
};

