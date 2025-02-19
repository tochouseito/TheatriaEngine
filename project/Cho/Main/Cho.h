#pragma once

// 全体で共有するもの
#include"SystemState/SystemState.h"

// Test用
#include <mutex>

// System
class WinApp;
class ResourceLeakChecker;
class FrameRate;
class DXGIFactory;
class D3DDevice;
class D3DCommand;
class D3DSwapChain;
class ResourceViewManager;
class RTVManager;
class DSVManager;
class DrawExecution;

// ThreadManager
class ThreadManager;

// GraphicsSystem
class GraphicsSystem;

// Loader
class TextureLoader;
class ModelLoader;
class MeshLoader;

// Script
class ScriptManager;

// ECS
class EntityManager;
class ComponentManager;
class SystemManager;
class PrefabManager;

// Scene
class SceneManager;

// 汎用機能
class InputManager;
class AudioManager;
class ImGuiManager;

// Editor
class EditorManager;

// Starter
class StartSetting;

// Json
class JsonFileLoader;

// Save
class SaveManager;

// C++
#include<memory>

class Cho
{
public:
	/// <summary>
	/// 稼働
	/// </summary>
	static void Operation();

	static inline ResourceViewManager* GetResourceViewManagerPtr() { return resourceViewManager.get(); }

	static inline ComponentManager* ComponentPtr() { return componentManager.get(); }


private:// メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	static void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	static void Finalize();

	/// <summary>
	/// 更新前処理
	/// </summary>
	static void PreUpdate();

	/// <summary>
	/// 更新
	/// </summary>
	static void Update();

	/// <summary>
	/// 更新後処理
	/// </summary>
	static void PostUpdate();

	/// <summary>
	/// 描画前処理
	/// </summary>
	static void PreDraw();

	/// <summary>
	/// 描画
	/// </summary>
	static void Draw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

#ifdef _DEBUG
	/// <summary>
	/// 描画前処理
	/// </summary>
	static void DebugPreDraw();

	/// <summary>
	/// 描画
	/// </summary>
	static void DebugDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void DebugPostDraw();
#endif // _DEBUG


	/// <summary>
	/// 読み込み
	/// </summary>
	static void Load();

	/// <summary>
	/// スターター
	/// </summary>
	static void StartSetUp();

	/// <summary>
	/// 保存
	/// </summary>
	static void Save();

	static void SystemStateEvent();

	static void SelectGameProject();

private:// メンバ変数

	/*共有するもの（シングルトンクラス）*/
	static SystemState& systemState;

	/*ウィンドウズアプリケーション*/
	static std::unique_ptr <WinApp> win;

	/*フレーム*/
	static std::unique_ptr<FrameRate> frameRate;

	/*DirectX*/
	static std::unique_ptr<ResourceLeakChecker> resourceLeakChecker;
	static std::unique_ptr<DXGIFactory> dxgiFactory;
	static std::unique_ptr<D3DDevice> d3dDevice;
	static std::unique_ptr<D3DCommand> d3dCommand;
	static std::unique_ptr<D3DSwapChain> d3dSwapChain;
	static std::unique_ptr<ResourceViewManager> resourceViewManager;
	static std::unique_ptr<RTVManager> rtvManager;
	static std::unique_ptr<DSVManager> dsvManager;
	static std::unique_ptr<DrawExecution> drawExecution;

	/*ThreadManager*/
	static std::unique_ptr<ThreadManager> threadManager;

	/*GraphicsSystem*/
	static std::unique_ptr<GraphicsSystem> graphicsSystem;

	/*Loader*/
	static std::unique_ptr<TextureLoader> textureLoader;
	static std::unique_ptr<MeshLoader> meshLoader;
	static std::unique_ptr<ModelLoader> modelLoader;

	/*Script*/
	static std::unique_ptr<ScriptManager> scriptManager;

	/*ECS*/
	static std::unique_ptr<EntityManager> entityManager;
	static std::unique_ptr<ComponentManager> componentManager;
	static std::unique_ptr<SystemManager> systemManager;
	static std::unique_ptr<PrefabManager> prefabManager;

	/*Scene*/
	static std::unique_ptr<SceneManager> sceneManager;

	/*汎用機能*/
	static std::unique_ptr<InputManager> inputManager;
	static std::unique_ptr<AudioManager> audioManager;
	static std::unique_ptr<ImGuiManager> imguiManager;

	/*Editor*/
	static std::unique_ptr<EditorManager> editorManager;

	/*Starter*/
	static std::unique_ptr<StartSetting> startSetting;

	/*Json*/
	static std::unique_ptr<JsonFileLoader> jsonFileLoader;

	/*Save*/
	static std::unique_ptr<SaveManager> saveManager;

	// ThreadTest用
	static bool isLoadTaskEnd;
	static std::mutex loadTaskMutex;
};

