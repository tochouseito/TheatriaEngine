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

CHO_API Engine* cho::CreateEngine(RuntimeMode mode)
{
    return new ChoEngine(mode);
}

CHO_API void cho::DestroyEngine(Engine* engine)
{
	delete engine;
}

CHO_API void cho::SetEngine(Engine* engine)
{
	g_Engine = static_cast<ChoEngine*>(engine);
}

CHO_API bool ChoSystem::SaveGameParameter(const std::wstring& filePath, const std::string& group, const std::string& item, const std::string& dataName, const GameParameterVariant& value)
{
	return cho::FileSystem::SaveGameParameter(filePath, group, item, dataName, value);
}

CHO_API bool ChoSystem::LoadGameParameter(const std::wstring& filePath, const std::string& group, const std::string& item, const std::string& dataName, GameParameterVariant& outValue)
{
	return cho::FileSystem::LoadGameParameter(filePath, group, item, dataName, outValue);
}

// ゲームオブジェクト取得
CHO_API GameObject* ChoSystem::FindGameObjectByName(const std::wstring& name)
{
	GameCore* gameCore = g_Engine->GetEngineCommand()->GetGameCore();
	GameObject* result = gameCore->GetGameWorld()->GetGameObject(name);
	return result;
}

CHO_API GameObject* ChoSystem::CloneGameObject(const GameObject* srcObj, Vector3 generatePosition)
{
	EngineCommand* engineCommand = g_Engine->GetEngineCommand();
	std::unique_ptr<CloneObjectCommand> command = std::make_unique<CloneObjectCommand>(srcObj->GetHandle());
	command->Execute(engineCommand);
	GameObject* newObject = engineCommand->GetGameCore()->GetGameWorld()->GetGameObject(command->GetDstHandle());
	// CurrendSceneのコピー
	newObject->SetCurrentSceneName(srcObj->GetCurrentSceneName());
	// Componentを取得
	TransformComponent* transform = engineCommand->GetGameCore()->GetECSManager()->GetComponent<TransformComponent>(newObject->GetHandle().entity);
	if (newObject->GetType() == ObjectType::ParticleSystem)
	{
		transform->scale.Zero();
	}
	// 初期位置を設定
	if (transform)
	{
		transform->position = generatePosition;
	}
	// Rigidbodyがあれば初期位置を指定
	Rigidbody3DComponent* rb3d = engineCommand->GetGameCore()->GetECSManager()->GetComponent<Rigidbody3DComponent>(newObject->GetHandle().entity);
	if(transform && rb3d && rb3d->runtimeBody)
	{
		rb3d->runtimeBody->SetTransform(transform->position);
	}
	return newObject;
}

// ゲームオブジェクト削除
CHO_API void ChoSystem::DestroyGameObject(GameObject* obj)
{
	EngineCommand* engineCommand = g_Engine->GetEngineCommand();
	if (!obj) { return; }
	std::unique_ptr<DeleteObjectCommand> command = std::make_unique<DeleteObjectCommand>(obj->GetHandle());
	command->Execute(engineCommand);
}

CHO_API float ChoSystem::DeltaTime()
{
	return Timer::GetDeltaTime();
}

CHO_API uint32_t ChoSystem::ScreenWidth()
{
	EngineCommand* engineCommand = g_Engine->GetEngineCommand();
	return static_cast<uint32_t>(engineCommand->GetGraphicsEngine()->GetResolutionWidth());
}

CHO_API uint32_t ChoSystem::ScreenHeight()
{
	EngineCommand* engineCommand = g_Engine->GetEngineCommand();
	return static_cast<uint32_t>(engineCommand->GetGraphicsEngine()->GetResolutionHeight());
}

CHO_API Marionnette* ChoSystem::GetMarionnettePtr(const std::wstring& name)
{
	GameObject* obj = FindGameObjectByName(name);
	if (!obj) { return nullptr; }
	ScriptComponent* script = g_Engine->GetEngineCommand()->GetGameCore()->GetECSManager()->GetComponent<ScriptComponent>(obj->GetHandle().entity);
	if (!script) { return nullptr; }
	if (script->instance)
	{
		return script->instance;
	}
	else
	{
		//Log::Write(LogLevel::Error, L"Marionnette instance is nullptr for object: " + name);
		return nullptr;
	}
}

void ChoSystem::SceneManagerAPI::LoadScene(const std::wstring& sceneName)
{
	g_Engine->GetEngineCommand()->GetGameCore()->GetSceneManager()->LoadScene(sceneName);
}

void ChoSystem::SceneManagerAPI::UnloadScene(const std::wstring& sceneName)
{
	sceneName;
}

void ChoSystem::SceneManagerAPI::ChangeMainScene(const std::wstring& sceneName)
{
	sceneName;
}

bool ChoSystem::Input::PushKey(const uint8_t& keyNumber)
{
	return g_Engine->GetEngineCommand()->GetInputManager()->PushKey(keyNumber);
}

bool ChoSystem::Input::TriggerKey(const uint8_t& keyNumber)
{
	return g_Engine->GetEngineCommand()->GetInputManager()->TriggerKey(keyNumber);
}

const DIMOUSESTATE2& ChoSystem::Input::GetAllMouse()
{
	return g_Engine->GetEngineCommand()->GetInputManager()->GetAllMouse();
}

MouseMove ChoSystem::Input::GetMouseMove()
{
	return g_Engine->GetEngineCommand()->GetInputManager()->GetMouseMove();
}

bool ChoSystem::Input::IsPressMouse(const int32_t& mouseNumber)
{
	return g_Engine->GetEngineCommand()->GetInputManager()->IsPressMouse(mouseNumber);
}

bool ChoSystem::Input::IsTriggerMouse(const int32_t& mouseNumber)
{
	return g_Engine->GetEngineCommand()->GetInputManager()->IsTriggerMouse(mouseNumber);
}

const Vector2& ChoSystem::Input::GetMouseWindowPosition()
{
	return g_Engine->GetEngineCommand()->GetInputManager()->GetMouseWindowPosition();
}

Vector2 ChoSystem::Input::GetMouseScreenPosition()
{
	return g_Engine->GetEngineCommand()->GetInputManager()->GetMouseScreenPosition();
}

bool ChoSystem::Input::GetJoystickState(const int32_t& stickNo, XINPUT_STATE& out)
{
	return g_Engine->GetEngineCommand()->GetInputManager()->GetJoystickState(stickNo, out);
}

bool GetJoystickStatePrevious(const int32_t& stickNo, XINPUT_STATE& out)
{
	return g_Engine->GetEngineCommand()->GetInputManager()->GetJoystickStatePrevious(stickNo, out);
}

void SetJoystickDeadZone(const int32_t& stickNo, const int32_t& deadZoneL, const int32_t& deadZoneR)
{
	g_Engine->GetEngineCommand()->GetInputManager()->SetJoystickDeadZone(stickNo, deadZoneL, deadZoneR);
}

size_t GetNumberOfJoysticks()
{
	return g_Engine->GetEngineCommand()->GetInputManager()->GetNumberOfJoysticks();
}

bool IsTriggerPadButton(const PadButton& button, int32_t stickNo)
{
	return g_Engine->GetEngineCommand()->GetInputManager()->IsTriggerPadButton(button, stickNo);
}

bool IsPressPadButton(const PadButton& button, int32_t stickNo)
{
	return g_Engine->GetEngineCommand()->GetInputManager()->IsPressPadButton(button, stickNo);
}

Vector2 GetStickValue(const LR& padStick, int32_t stickNo)
{
	return g_Engine->GetEngineCommand()->GetInputManager()->GetStickValue(padStick, stickNo);
}

float GetLRTrigger(const LR& LorR, int32_t stickNo)
{
	return g_Engine->GetEngineCommand()->GetInputManager()->GetLRTrigger(LorR, stickNo);
}
