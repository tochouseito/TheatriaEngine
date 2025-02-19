#pragma once

// C++
#include <fstream>
#include <filesystem>

class ScriptManager;
class EntityManager;
class ComponentManager;
class PrefabManager;
class SceneManager;

// JSON
class JsonFileLoader;

class SaveManager
{
public:

	void Initialize(
		ScriptManager* scriptManager,
		EntityManager* entityManager,
		ComponentManager* componentManager,
		PrefabManager* prefabManager,
		SceneManager* sceneManager
	);

	void Save(JsonFileLoader* jsonLoad);

private:

private:
	ScriptManager* scriptManager_ = nullptr;
	EntityManager* entityManager_ = nullptr;
	ComponentManager* componentManager_ = nullptr;
	PrefabManager* prefabManager_ = nullptr;
	SceneManager* sceneManager_ = nullptr;
};

