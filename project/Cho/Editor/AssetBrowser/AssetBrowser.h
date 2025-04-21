#pragma once
#include "Editor/BaseEditor/BaseEditor.h"
#include <filesystem>
class AssetBrowser : public BaseEditor
{
public:
	AssetBrowser(EditorManager* editorManager) :
		BaseEditor(editorManager)
	{
	}
	~AssetBrowser()
	{
	}
	void Initialize() override;
	void Update() override;
	void Window() override;
private:
	void DrawDirectoryTree(const std::filesystem::path& path);
	void DrawAssetGrid(const std::filesystem::path& path);
	const char* GetIconForEntry(const std::filesystem::directory_entry& entry);

	std::filesystem::path currentPath = "";
};

