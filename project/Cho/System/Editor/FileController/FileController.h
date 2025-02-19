#pragma once

#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include <string>

class FileController
{
public:
	void Initialize();
	void Update();

private:
    void OpenExplorerInCurrentDirectory();
	void OpenExplorerInDirectory(const std::string& directoryPath);
};

