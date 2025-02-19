#pragma once

// D3D12
class ResourceViewManager;
class RTVManager;
class DrawExecution;

// Loader
class TextureLoader;

// ECS
class EntityManager;
class ComponentManager;
class SystemManager;
class PrefabManager;

// GameObject
class GameObject;

// SceneManager
class SceneManager;

class EditorManager;

class ScriptManager;

class InfoView
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(
		ResourceViewManager* rvManager,
		RTVManager* rtvManager,
		DrawExecution* drawExe,
		TextureLoader* texLoader,
		EntityManager* entityManager,
		ComponentManager* componentManager,
		SystemManager* systemManager,
		PrefabManager* prefabManager,
		SceneManager* sceneManager,
		EditorManager* editManager,
		ScriptManager* scriptManager
	);

	void Update();

private:

	// カスタムImGui
	bool ColoredDragFloat3(const char* label, float* v, float v_speed, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f");

private:
	/*D3D12*/
	ResourceViewManager* rvManager_ = nullptr;
	RTVManager* rtvManager_ = nullptr;
	DrawExecution* drawExe_ = nullptr;

	TextureLoader* texLoader_ = nullptr;

	/*ECS*/
	EntityManager* entityManager_ = nullptr;
	ComponentManager* componentManager_ = nullptr;
	SystemManager* systemManager_ = nullptr;
	PrefabManager* prefabManager_ = nullptr;
	SceneManager* sceneManager_ = nullptr;

	EditorManager* editManager_ = nullptr;
	ScriptManager* scriptManager_ = nullptr;
};

