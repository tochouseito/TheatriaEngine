#include "pch.h"
#define ENGINECREATE_FUNCTION
#define USE_CHOENGINE_SCRIPT
#include "ChoEngineAPI.h"
#include "Main/ChoEngine.h"
#include "EngineCommand/EngineCommands.h"

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

CHO_API void ChoSystem::CloneGameObject(GameObject& object, Vector3 generatePosition)
{
	EngineCommand* engineCommand = g_Engine->GetEngineCommand();
	if (!engineCommand) { return; }
	std::unique_ptr<Add3DObjectCommand> command = std::make_unique<Add3DObjectCommand>();
	command->Execute(engineCommand);
	GameObject& newObject = engineCommand->GetGameCore()->GetObjectContainer()->GetGameObject(command->GetObjectID());
	// Transform以外のComponentを取得
	MeshFilterComponent* meshFilter = engineCommand->GetGameCore()->GetECSManager()->GetComponent<MeshFilterComponent>(object.GetEntity());
	MeshRendererComponent* meshRenderer = engineCommand->GetGameCore()->GetECSManager()->GetComponent<MeshRendererComponent>(object.GetEntity());
	ScriptComponent* script = engineCommand->GetGameCore()->GetECSManager()->GetComponent<ScriptComponent>(object.GetEntity());
	std::vector<LineRendererComponent>* lineRenderer = engineCommand->GetGameCore()->GetECSManager()->GetAllComponents<LineRendererComponent>(object.GetEntity());
	Rigidbody2DComponent* rb = engineCommand->GetGameCore()->GetECSManager()->GetComponent<Rigidbody2DComponent>(object.GetEntity());
	BoxCollider2DComponent* box = engineCommand->GetGameCore()->GetECSManager()->GetComponent<BoxCollider2DComponent>(object.GetEntity());
	// あるやつをnewObjectに追加
	if (meshFilter) { engineCommand->GetGameCore()->GetECSManager()->AddComponent<MeshFilterComponent>(newObject.GetEntity()); }
	if (meshRenderer) { engineCommand->GetGameCore()->GetECSManager()->AddComponent<MeshRendererComponent>(newObject.GetEntity()); }
	if (script) { engineCommand->GetGameCore()->GetECSManager()->AddComponent<ScriptComponent>(newObject.GetEntity()); }
	if (lineRenderer)
	{
		for (auto& line : *lineRenderer)
		{
			line;
			engineCommand->GetGameCore()->GetECSManager()->AddComponent<LineRendererComponent>(newObject.GetEntity());
		}
	}
	if (rb) { engineCommand->GetGameCore()->GetECSManager()->AddComponent<Rigidbody2DComponent>(newObject.GetEntity()); }
	if (box) { engineCommand->GetGameCore()->GetECSManager()->AddComponent<BoxCollider2DComponent>(newObject.GetEntity()); }
	// TransformComponentを取得
	TransformComponent* transform = engineCommand->GetGameCore()->GetECSManager()->GetComponent<TransformComponent>(newObject.GetEntity());
	// 初期位置を設定
	if (transform)
	{
		transform->translation = generatePosition;
	}
	engineCommand->GetGameCore()->AddGameGenerateObject(newObject.GetID().value());
}
