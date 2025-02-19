#pragma once
#include"Scene\SceneFactory\SceneFactory.h"
#include"ECS/GameObject/GameObject.h"

// C++
#include<string>
#include<unordered_map>
#include<memory>
#include <regex>

class ResourceViewManager;

// ECS
class EntityManager;
class ComponentManager;
class SystemManager;
class PrefabManager;

// 汎用機能
class InputManager;
class AudioManager;

class SceneManager
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(
		ResourceViewManager* rvManager,
		EntityManager* entityManager,
		ComponentManager* componentManager,
		SystemManager* systemManager,
		PrefabManager* prefabManager,
		InputManager* inputManager,
		AudioManager* audioManager
	);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();


	/*シーン切り替え*/
	void ChangeScene(const std::string& sceneName);

public:
	// ゲームオブジェクトの追加
	std::string AddGameObject(const std::string& objectName, const ObjectType& type);

	// ゲームオブジェクトリストの取得
	const std::unordered_map<std::string, std::unique_ptr<GameObject>>& GetGameObjects() const {
		return gameObjects;
	}

	// ひとつのゲームオブジェクトを取得
	GameObject* GetGameObject(const std::string& name) {
		if (!gameObjects.contains(name)) {
			assert(0 && "Not found GameObject name");
		}
		GameObject* result = gameObjects[name].get();
		return result;
	}

	// ゲームオブジェクトの削除
	void DeleteGameObject(const std::string& name);

	std::string GameObjectListRename(const std::string& newName, const std::string& deleteName);

	ComponentManager* GetCompPtr()const { return componentManager_; }
	ResourceViewManager* GetRVManagerPtr()const { return rvManager_; }
	InputManager* GetInputManagerPtr()const { return inputManager_; }
	AudioManager* GetAudioManagerPtr()const { return audioManager_; }

	void SetNowCamera(const Entity& id) { entityManager_->SetCameraID(id); }
	Entity GetNowCamera() { return entityManager_->GetNowCameraEntity(); }

	// 仮のEffect作成関数
	void CreateEffect(EffectNode& effectNode);

	// シーン追加
	//void AddScene(const std::string& sceneName, BaseScene* scene);

private:
	// ユニークな名前を生成する関数
	std::string GenerateUniqueName(
		const std::unordered_map<std::string, std::unique_ptr<GameObject>>& objects,
		const std::string& baseName
	);
private:
	/*今のシーン*/
	BaseScene* scene_ = nullptr;
	/*次のシーン*/
	BaseScene* nextScene_ = nullptr;
	/*シーンファクトリー*/
	AbstractSceneFactory* sceneFactory_ = nullptr;

	// ResourceManager
	ResourceViewManager* rvManager_ = nullptr;

	/*ECS*/
	EntityManager* entityManager_ = nullptr;
	ComponentManager* componentManager_ = nullptr;
	SystemManager* systemManager_ = nullptr;
	PrefabManager* prefabManager_ = nullptr;

	/*汎用機能*/
	InputManager* inputManager_ = nullptr;
	AudioManager* audioManager_ = nullptr;

	// 現在のDirectionalLightの数
	uint32_t directionalLightCount = 0;

	// 現在のPointLightの数
	uint32_t pointLightCount = 0;

	// 現在のSpotLightの数
	uint32_t spotLightCount = 0;

	/*シーンコンテナ*/
	std::unordered_map<std::string, std::unique_ptr<BaseScene>> scenes;
	
	std::unordered_map<std::string, std::unique_ptr<GameObject>> gameObjects;
};

