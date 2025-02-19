#pragma once

// C++
#include <fstream>
#include <filesystem>

// Json
#include <json.hpp>

class ScriptManager;
class EntityManager;
class ComponentManager;
class PrefabManager;
class SceneManager;

class JsonFileLoader
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(
		ScriptManager* scriptManager,
		EntityManager* entityManager,
		ComponentManager* componentManager,
		PrefabManager* prefabManager,
		SceneManager* sceneManager
	);

	// ImGuiのスタイルを指定したファイルに保存する関数
	void SaveStyle(const char* filename);

	// 指定したファイルからImGuiのスタイルを読み込む関数
	void LoadStyle(const char* filename);

	// プロジェクトの「Resources/ImGuiStyle」ディレクトリにスタイルを保存する関数
	void SaveStyleToProject();

	// プロジェクトの「Resources/ImGuiStyle/style.json」からスタイルを読み込む関数
	void LoadStyleFromProject();

	void LoadProject();

private:
	ScriptManager* scriptManager_ = nullptr;
	EntityManager* entityManager_ = nullptr;
	ComponentManager* componentManager_ = nullptr;
	PrefabManager* prefabManager_ = nullptr;
	SceneManager* sceneManager_ = nullptr;
};

