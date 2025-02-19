#pragma once
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;
class GameProject
{
public:

    static void CreateGameFolder(const std::string& projectName);
};

