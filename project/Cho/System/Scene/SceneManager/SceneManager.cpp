#include "PrecompiledHeader.h"
#include "SceneManager.h"
#include <assert.h>
#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"ECS/EntityManager/EntityManager.h"
#include"ECS/ComponentManager/ComponentManager.h"
#include"ECS/System/SystemManager/SystemManager.h"

void SceneManager::Initialize(
	ResourceViewManager* rvManager,
	EntityManager* entityManager,
	ComponentManager* componentManager,
	SystemManager* systemManager,
	PrefabManager* prefabManager,
	InputManager* inputManager,
	AudioManager* audioManager
)
{
	// シーンファクトリーの生成
	sceneFactory_ = new SceneFactory();

	rvManager_ = rvManager;

	// ECS
	entityManager_ = entityManager;
	componentManager_ = componentManager;
	systemManager_ = systemManager;
	prefabManager_ = prefabManager;

	// 汎用機能
	inputManager_ = inputManager;
	audioManager_ = audioManager;
}

void SceneManager::Finalize()
{
	delete sceneFactory_;
	/*最後のシーンの終了と解放*/
	if (scene_) {
		scene_->Finalize();
	}
	delete scene_;
}

void SceneManager::Update()
{
	if (nextScene_) {
		/*旧シーンの終了*/
		if (scene_) {
			scene_->Finalize();
			delete scene_;
		}
		/*シーン切り替え*/
		scene_ = nextScene_;
		nextScene_ = nullptr;
		/*シーンマネージャをセット*/
		scene_->SetSceneManager(this);
		/*次のシーンを初期化する*/
		scene_->Initialize();
		systemManager_->Start(*entityManager_, *componentManager_);
	}
	if (!scene_) { return; }
	scene_->Update();
}

void SceneManager::Draw()
{
	if (!scene_) { return; }
	scene_->Draw();
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	/*次のシーン生成*/
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}

std::string SceneManager::AddGameObject(const std::string& objectName, const ObjectType& type)
{
	std::string newName = GenerateUniqueName(gameObjects, objectName);

	// 新しいGameObjectを作成してマップに追加
	gameObjects[newName] = std::make_unique<GameObject>();

	GameObject* gameObject = gameObjects[newName].get();
	gameObject->SetName(newName);
	gameObject->SetObjectType(type);
	gameObject->SetManager(entityManager_, componentManager_);
	gameObject->CreateEntity();

	switch (type)
	{
	case ObjectType::Camera:
		SetNowCamera(gameObject->GetEntityID());
		break;
	case ObjectType::DirectionalLight:
		if (directionalLightCount >= kDirLightNum) {
			assert(0&&" can't Add DirectionalLight ");
		}
		directionalLightCount++;
		break;
	case ObjectType::SpotLight:
		if (spotLightCount >= kSpotLightNum) {
			assert(0 && " can't Add SpotLight ");
		}
		spotLightCount++;
		break;
	case ObjectType::PointLight:
		if (pointLightCount >= kPointLightNum) {
			assert(0 && " can't Add PointLight ");
		}
		pointLightCount++;
		break;
	default:
		break;
	}

	return newName;
}

void SceneManager::DeleteGameObject(const std::string& name)
{
	if (!gameObjects.contains(name)) {
#ifdef _DEBUG
		SystemState::ShowWarning("Nothing found to delete gameObject");
#endif // _DEBUG
		return;
	}

	GameObject* gameObject = gameObjects[name].get();
	switch (gameObject->GetObjectType())
	{
	case ObjectType::DirectionalLight:
		if (directionalLightCount <= 0) {
#ifdef _DEBUG
			SystemState::ShowWarning(" can't Delete DirectionalLight ");
#endif // _DEBUG
			return;
		}
		directionalLightCount--;
		break;
	case ObjectType::SpotLight:
		if (spotLightCount <= 0) {
#ifdef _DEBUG
			SystemState::ShowWarning(" can't Delete SpotLight ");
#endif // DEBUG
			return;
		}
		spotLightCount--;
		break;
	case ObjectType::PointLight:
		if (pointLightCount <= 0) {
#ifdef _DEBUG
			SystemState::ShowWarning(" can't Delete PointLight ");
#endif // DEBUG
			return;
		}
		pointLightCount--;
		break;
	default:
		break;
	}
	gameObjects.erase(name);
}

std::string SceneManager::GameObjectListRename(const std::string& newName, const std::string& deleteName)
{
	if (newName == deleteName) {
		return newName;
	}
	// 古い名前が存在するか確認
	auto it = gameObjects.find(deleteName);
	if (!gameObjects.contains(deleteName)) {
		//std::cerr << "Error: GameObject with name \"" << deleteName << "\" not found.\n";
		assert(0);
	}

	std::string NewName = GenerateUniqueName(gameObjects, newName);

	// オブジェクトを一時保存して、古い名前を削除
	auto object = std::move(it->second);
	gameObjects.erase(it);

	// 新しい名前で挿入
	gameObjects[NewName] = std::move(object);
	GameObject* gameObject = gameObjects[NewName].get();
	gameObject->SetName(NewName);

	return NewName;
}

void SceneManager::CreateEffect(EffectNode& effectNode)
{
	rvManager_->CreateEffectNodeResource(effectNode);
}

// ユニークな名前を生成する関数
std::string SceneManager::GenerateUniqueName(
	const std::unordered_map<std::string, std::unique_ptr<GameObject>>& objects,
	const std::string& baseName) {
	std::string newName = baseName;
	int counter = 1;

	// 正規表現で "(番号)" を検出
	std::regex numberRegex(R"(^(.*) \((\d+)\)$)");
	std::smatch match;

	if (std::regex_match(baseName, match, numberRegex)) {
		// "baseName (番号)" の場合
		newName = match[1];                 // ベース名（括弧前の部分）
		counter = std::stoi(match[2]) + 1; // 現在の番号を取得して次から判定
	} else {
		// 括弧がない場合、すでに重複していないならそのまま返す
		if (!objects.contains(baseName)) {
			return baseName;
		}
	}

	// 名前が重複している間、新しい名前に番号を追加してチェック
	while (objects.contains(newName + " (" + std::to_string(counter) + ")")) {
		counter++;
	}

	return newName + " (" + std::to_string(counter) + ")";
}
