#pragma once

#include <filesystem>
#include <string>
#include <iostream>

namespace fs = std::filesystem;

// D3D12
class ResourceViewManager;
class RTVManager;
class DrawExecution;

// ECS
class EntityManager;
class ComponentManager;
class SystemManager;
class PrefabManager;

// GameObject
class GameObject;

// SceneManager
class SceneManager;

// EditorManager
class EditorManager;

// JSON
class JsonFileLoader;

class StartSetting
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(
		ResourceViewManager* rvManager,
		RTVManager* rtvManager,
		DrawExecution* drawExe,
		EntityManager* entityManager,
		ComponentManager* componentManager,
		SystemManager* systemManager,
		PrefabManager* prefabManager,
		SceneManager* sceneManager,
		EditorManager* editorManager
	);

	bool IsProject();

	void SelectedProject();

	bool IsNew() { return createNewProject; }

	void CreateProject();

	void LoadProject(JsonFileLoader* jsonLoader);

	std::string GetProjectName()const {
		return projectName;
	}

	std::string GetProjectRoot()const { return projectRoot; }

	void FindProjects();

private:

	std::string projectName = "";
	fs::path projectPath = "";

	bool createNewProject = true;        // 新しいプロジェクトを作るかどうか
	std::string projectRoot = "C:\\ChoProject"; // プロジェクトのルートディレクトリ
	std::vector<std::string> projectList;     // 見つかったプロジェクトのリスト

	bool isProject = false;

	// プロジェクト名の入力用バッファ
	char projectNameBuffer[256] = ""; // 必要なバッファサイズに調整
};

