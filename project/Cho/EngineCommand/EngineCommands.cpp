#include "pch.h"
#include "EngineCommands.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "GameCore/GameCore.h"
#include "Core/Utility/GenerateUnique.h"
#include "Core/ChoLog/ChoLog.h"
using namespace Cho;

bool Add3DObjectCommand::Execute(EngineCommand* edit)
{
	// CurrentSceneがないなら失敗
	if (!edit->m_GameCore->GetSceneManager()->GetCurrentScene())
	{
		Log::Write(LogLevel::Assert, "Current Scene is nullptr");
		return false;
	}
	// 各IDの取得
	// Entity
	Entity entity = edit->m_GameCore->GetECSManager()->GenerateEntity();
	// デフォルトの名前
	std::wstring name = L"NewMeshObject";
	// 重複回避
	name = GenerateUniqueName(name, edit->m_GameCore->GetObjectContainer()->GetNameToObjectID());
	// Transform統合バッファからmapIDを取得
	uint32_t mapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
	// TransformComponentを追加
	TransformComponent* transform = edit->m_GameCore->GetECSManager()->AddComponent<TransformComponent>(entity);
	transform->mapID = mapID;
	// GameObjectを追加
	ObjectID objectID = edit->m_GameCore->GetObjectContainer()->AddGameObject(entity, name, ObjectType::MeshObject);
	m_ObjectID = objectID;
	// シーンに追加
	edit->m_GameCore->GetSceneManager()->GetCurrentScene()->AddUseObject(objectID);
	return true;
}

bool Add3DObjectCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddCameraObjectCommand::Execute(EngineCommand* edit)
{
	// CurrentSceneがないなら失敗
	if (!edit->m_GameCore->GetSceneManager()->GetCurrentScene())
	{
		Log::Write(LogLevel::Assert, "Current Scene is nullptr");
		return false;
	}
	// 各IDの取得
	// Entity
	Entity entity = edit->m_GameCore->GetECSManager()->GenerateEntity();
	// デフォルトの名前
	std::wstring name = L"NewMeshObject";
	// 重複回避
	name = GenerateUniqueName(name, edit->m_GameCore->GetObjectContainer()->GetNameToObjectID());
	// Transform統合バッファからmapIDを取得
	uint32_t mapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
	// TransformComponentを追加
	TransformComponent* transform = edit->m_GameCore->GetECSManager()->AddComponent<TransformComponent>(entity);
	transform->mapID = mapID;
	// CameraComponentを追加
	CameraComponent* camera = edit->m_GameCore->GetECSManager()->AddComponent<CameraComponent>(entity);
	// Resourceの生成
	camera->bufferIndex = edit->m_ResourceManager->CreateConstantBuffer<BUFFER_DATA_VIEWPROJECTION>();
	// GameObjectを追加
	ObjectID objectID = edit->m_GameCore->GetObjectContainer()->AddGameObject(entity, name, ObjectType::MeshObject);
	m_ObjectID = objectID;
	// シーンに追加
	edit->m_GameCore->GetSceneManager()->GetCurrentScene()->AddUseObject(objectID);
	return true;
}

bool AddCameraObjectCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddMeshFilterComponent::Execute(EngineCommand* edit)
{
	// MeshFilterComponentを追加
	MeshFilterComponent* mesh = edit->m_GameCore->GetECSManager()->AddComponent<MeshFilterComponent>(m_Entity);
	// Transformとの連携
	TransformComponent* transform = edit->m_GameCore->GetECSManager()->GetComponent<TransformComponent>(m_Entity);
	// デフォルトモデルとしてCube
	mesh->modelName = L"Cube";
	// モデルのIDを取得
	mesh->modelID = edit->m_ResourceManager->GetModelManager()->GetModelDataIndex(mesh->modelName);
	// モデルのUseListに登録
	edit->m_ResourceManager->GetModelManager()->RegisterModelUseList(mesh->modelID.value(), transform->mapID.value());

	return true;
}

bool AddMeshFilterComponent::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddMeshRendererComponent::Execute(EngineCommand* edit)
{
	// MeshRendererComponentを追加
	MeshRendererComponent* render = edit->m_GameCore->GetECSManager()->AddComponent<MeshRendererComponent>(m_Entity);
	render->visible = true;
	return true;
}

bool AddMeshRendererComponent::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool SetMainCamera::Execute(EngineCommand* edit)
{
	// MainCameraを設定
	// シーンがないなら失敗
	if (!edit->m_GameCore->GetSceneManager()->GetCurrentScene())
	{
		Log::Write(LogLevel::Assert, "Current Scene is nullptr");
		return false;
	}
	// 現在のMainCameraIDを取得
	m_PreCameraID = edit->m_GameCore->GetSceneManager()->GetCurrentScene()->GetMainCameraID();
	// MainCameraを設定
	edit->m_GameCore->GetSceneManager()->GetCurrentScene()->SetMainCameraID(m_SetCameraID.value());

	return true;
}

bool SetMainCamera::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddScriptComponent::Execute(EngineCommand* edit)
{
	// ScriptComponentを追加
	ScriptComponent* script = edit->m_GameCore->GetECSManager()->AddComponent<ScriptComponent>(m_Entity);
	// Entityを設定
	script->entity = m_Entity;
	return true;
}

bool AddScriptComponent::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddLineRendererComponent::Execute(EngineCommand* edit)
{
	// LineRendererComponentを追加
	LineRendererComponent* line = edit->m_GameCore->GetECSManager()->AddComponent<LineRendererComponent>(m_Entity);
	// mapIDを取得
	uint32_t mapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Line)->GetMapID();
	// mapIDを設定
	line->mapID = mapID;
	return false;
}

bool AddLineRendererComponent::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddRigidbody2DComponent::Execute(EngineCommand* edit)
{
	// Rigidbody2DComponentを追加
	Rigidbody2DComponent* rb = edit->m_GameCore->GetECSManager()->AddComponent<Rigidbody2DComponent>(m_Entity);
	rb;
	return true;
}

bool AddRigidbody2DComponent::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddBoxCollider2DComponent::Execute(EngineCommand* edit)
{
	// BoxCollider2DComponentを追加
	BoxCollider2DComponent* box = edit->m_GameCore->GetECSManager()->AddComponent<BoxCollider2DComponent>(m_Entity);
	box;
	return true;
}

bool AddBoxCollider2DComponent::Undo(EngineCommand* edit)
{
	edit;
	return false;
}
