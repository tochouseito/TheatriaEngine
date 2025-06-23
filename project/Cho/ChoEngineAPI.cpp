#include "pch.h"
#define ENGINECREATE_FUNCTION
#define USE_CHOENGINE_SCRIPT
#include "ChoEngineAPI.h"
#include "Main/ChoEngine.h"
#include "EngineCommand/EngineCommands.h"

// メンバ定義
namespace ChoSystem
{
	SceneManagerAPI sceneManager;
}

CHO_API Engine* Cho::CreateEngine(RuntimeMode mode)
{
    return new ChoEngine(mode);
}

CHO_API void Cho::DestroyEngine(Engine* engine)
{
	delete engine;
}

CHO_API void Cho::SetEngine(Engine* engine)
{
	g_Engine = static_cast<ChoEngine*>(engine);
}

CHO_API bool ChoSystem::SaveGameParameter(const std::wstring& filePath, const std::string& group, const std::string& item, const std::string& dataName, const GameParameterVariant& value)
{
	return Cho::FileSystem::SaveGameParameter(filePath, group, item, dataName, value);
}

CHO_API bool ChoSystem::LoadGameParameter(const std::wstring& filePath, const std::string& group, const std::string& item, const std::string& dataName, GameParameterVariant& outValue)
{
	return Cho::FileSystem::LoadGameParameter(filePath, group, item, dataName, outValue);
}

// ゲームオブジェクト取得
CHO_API GameObject* ChoSystem::FindGameObjectByName(std::wstring_view name)
{
	GameCore* gameCore = g_Engine->GetEngineCommand()->GetGameCore();
	GameObject* result = gameCore->GetObjectContainer()->GetGameObjectByName(name.data());
	return result;
}

CHO_API GameObject* ChoSystem::CloneGameObject(const GameObject* srcObj, Vector3 generatePosition)
{
	EngineCommand* engineCommand = g_Engine->GetEngineCommand();
	std::unique_ptr<CloneObjectCommand> command = std::make_unique<CloneObjectCommand>(srcObj->GetCurrentSceneName(),srcObj->GetID().value());
	command->Execute(engineCommand);
	GameObject* newObject = engineCommand->GetGameCore()->GetObjectContainer()->GetGameObject(command->GetObjectID());
	// CurrendSceneのコピー
	newObject->SetCurrentSceneName(srcObj->GetCurrentSceneName());
	// Componentを取得
	TransformComponent* transform = engineCommand->GetGameCore()->GetECSManager()->GetComponent<TransformComponent>(newObject->GetEntity());
	if (newObject->GetType() == ObjectType::ParticleSystem)
	{
		transform->scale.Zero();
	}
	// 初期位置を設定
	if (transform)
	{
		transform->position = generatePosition;
	}
	engineCommand->GetGameCore()->AddGameGenerateObject(newObject->GetID().value());
	return newObject;
}

CHO_API void ChoSystem::DestroyGameObject(std::optional<uint32_t> id)
{
	EngineCommand* engineCommand = g_Engine->GetEngineCommand();
	if (!engineCommand) { return; }
	GameCore* gameCore = engineCommand->GetGameCore();
	if (!gameCore) { return; }
	gameCore->RemoveGameInitializedID(id.value());
	// 生成されたオブジェクトを削除
	std::unique_ptr<DeleteObjectCommand> command = std::make_unique<DeleteObjectCommand>(id.value());
	command->Execute(engineCommand);
}

CHO_API float ChoSystem::DeltaTime()
{
	return Timer::GetDeltaTime();
}

void ChoSystem::SceneManagerAPI::LoadScene(const std::wstring& sceneName)
{
	g_Engine->GetEngineCommand()->GetGameCore()->GetSceneManager()->LoadScene(sceneName);
}

void ChoSystem::SceneManagerAPI::UnloadScene(const std::wstring& sceneName)
{
	g_Engine->GetEngineCommand()->GetGameCore()->GetSceneManager()->UnLoadScene(sceneName);
}

void ChoSystem::SceneManagerAPI::ChangeMainScene(const std::wstring& sceneName)
{
	g_Engine->GetEngineCommand()->GetGameCore()->GetSceneManager()->ChangeMainScene(sceneName);
}
