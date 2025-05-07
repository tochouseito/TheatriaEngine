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

CHO_API void ChoSystem::CloneGameObject(std::optional<uint32_t> id, Vector3 generatePosition)
{
	EngineCommand* engineCommand = g_Engine->GetEngineCommand();
	if (!engineCommand) { return; }
	std::unique_ptr<Add3DObjectCommand> command = std::make_unique<Add3DObjectCommand>();
	command->Execute(engineCommand);
	GameObject& object = engineCommand->GetGameCore()->GetObjectContainer()->GetGameObject(id.value());
	GameObject& newObject = engineCommand->GetGameCore()->GetObjectContainer()->GetGameObject(command->GetObjectID());
	// nameを変更
	std::unique_ptr<RenameObjectCommand> renameCommand = std::make_unique<RenameObjectCommand>(newObject.GetID().value(), object.GetName());
	renameCommand->Execute(engineCommand);
	// Transform以外のComponentを取得
	MeshFilterComponent* meshFilter = engineCommand->GetGameCore()->GetECSManager()->GetComponent<MeshFilterComponent>(object.GetEntity());
	MeshRendererComponent* meshRenderer = engineCommand->GetGameCore()->GetECSManager()->GetComponent<MeshRendererComponent>(object.GetEntity());
	ScriptComponent* script = engineCommand->GetGameCore()->GetECSManager()->GetComponent<ScriptComponent>(object.GetEntity());
	std::vector<LineRendererComponent>* lineRenderer = engineCommand->GetGameCore()->GetECSManager()->GetAllComponents<LineRendererComponent>(object.GetEntity());
	Rigidbody2DComponent* rb = engineCommand->GetGameCore()->GetECSManager()->GetComponent<Rigidbody2DComponent>(object.GetEntity());
	BoxCollider2DComponent* box = engineCommand->GetGameCore()->GetECSManager()->GetComponent<BoxCollider2DComponent>(object.GetEntity());
	// あるやつをnewObjectに追加
	if (meshFilter)
	{
		std::unique_ptr<AddMeshFilterComponent> addMeshFilterCommand = std::make_unique<AddMeshFilterComponent>(newObject.GetEntity());
		addMeshFilterCommand->Execute(engineCommand);
		// MeshFilterComponentのMeshを設定
		MeshFilterComponent* newMeshFilter = engineCommand->GetGameCore()->GetECSManager()->GetComponent<MeshFilterComponent>(newObject.GetEntity());
		if (newMeshFilter)
		{
			newMeshFilter->modelID = meshFilter->modelID;
			newMeshFilter->modelName = meshFilter->modelName;
		}
	}
	if (meshRenderer)
	{
		std::unique_ptr<AddMeshRendererComponent> addMeshRendererCommand = std::make_unique<AddMeshRendererComponent>(newObject.GetEntity());
		addMeshRendererCommand->Execute(engineCommand);
	}
	if (script)
	{
		std::unique_ptr<AddScriptComponent> addScriptCommand = std::make_unique<AddScriptComponent>(newObject.GetEntity(),newObject.GetID().value());
		addScriptCommand->Execute(engineCommand);
		// ScriptComponentのスクリプト名を設定
		ScriptComponent* newScript = engineCommand->GetGameCore()->GetECSManager()->GetComponent<ScriptComponent>(newObject.GetEntity());
		if (newScript)
		{
			newScript->scriptName = script->scriptName;
			newScript->objectID = newObject.GetID().value();
		}
	}
	if (lineRenderer)
	{
		for (auto& line : *lineRenderer)
		{
			line;
			std::unique_ptr<AddLineRendererComponent> addLineCommand = std::make_unique<AddLineRendererComponent>(newObject.GetEntity());
			addLineCommand->Execute(engineCommand);
		}
	}
	if (rb)
	{
		std::unique_ptr<AddRigidbody2DComponent> addRigidbodyCommand = std::make_unique<AddRigidbody2DComponent>(newObject.GetEntity(), newObject.GetID().value());
		addRigidbodyCommand->Execute(engineCommand);
		Rigidbody2DComponent* newRigidbody = engineCommand->GetGameCore()->GetECSManager()->GetComponent<Rigidbody2DComponent>(newObject.GetEntity());
		if (newRigidbody)
		{
			newRigidbody->mass = rb->mass;
			newRigidbody->gravityScale = rb->gravityScale;
			newRigidbody->isKinematic = rb->isKinematic;
			newRigidbody->fixedRotation = rb->fixedRotation;
			newRigidbody->bodyType = rb->bodyType;
		}
	}
	if (box)
	{
		std::unique_ptr<AddBoxCollider2DComponent> addBoxCommand = std::make_unique<AddBoxCollider2DComponent>(newObject.GetEntity());
		addBoxCommand->Execute(engineCommand);
		BoxCollider2DComponent* newBox = engineCommand->GetGameCore()->GetECSManager()->GetComponent<BoxCollider2DComponent>(newObject.GetEntity());
		if (newBox)
		{
			newBox->offsetX = box->offsetX;
			newBox->offsetY = box->offsetY;
			newBox->width = box->width;
			newBox->height = box->height;
			newBox->density = box->density;
			newBox->friction = box->friction;
			newBox->restitution = box->restitution;
			newBox->isSensor = box->isSensor;
		}
	}
	// TransformComponentを取得
	TransformComponent* transform = engineCommand->GetGameCore()->GetECSManager()->GetComponent<TransformComponent>(newObject.GetEntity());
	// 初期位置を設定
	if (transform)
	{
		transform->translation = generatePosition;
	}
	engineCommand->GetGameCore()->AddGameGenerateObject(newObject.GetID().value());
}
