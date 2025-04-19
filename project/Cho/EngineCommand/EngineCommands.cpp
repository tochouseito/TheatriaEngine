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
	// SelectedObjectを設定
	edit->SetSelectedObject(&edit->m_GameCore->GetObjectContainer()->GetGameObject(m_ObjectID));
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
	std::wstring name = L"NewCameraObject";
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
	ObjectID objectID = edit->m_GameCore->GetObjectContainer()->AddGameObject(entity, name, ObjectType::Camera);
	m_ObjectID = objectID;
	// シーンに追加
	edit->m_GameCore->GetSceneManager()->GetCurrentScene()->AddUseObject(objectID);
	// SelectedObjectを設定
	edit->SetSelectedObject(&edit->m_GameCore->GetObjectContainer()->GetGameObject(m_ObjectID));
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
	//script->entity = m_Entity;
	script->objectID = m_ObjectID;
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
	rb->selfObjectID = m_ObjectID;
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

bool DeleteObjectCommand::Execute(EngineCommand* edit)
{
	GameObject& object = edit->m_GameCore->GetObjectContainer()->GetGameObject(m_ObjectID);
	if (!object.IsActive()) { return false; }
	m_Name = object.GetName();
	m_Type = object.GetType();
	// モデルのUseListから削除
	TransformComponent* transform = edit->m_GameCore->GetECSManager()->GetComponent<TransformComponent>(object.GetEntity());
	if (!transform) { Log::Write(LogLevel::Assert, "TransfomrComponent nullptr"); }
	MeshFilterComponent* meshFilter = edit->m_GameCore->GetECSManager()->GetComponent<MeshFilterComponent>(object.GetEntity());
	if (meshFilter)
	{
		edit->m_ResourceManager->GetModelManager()->RemoveModelUseList(meshFilter->modelID.value(), transform->mapID.value());
	}
	// TransformMapIDを返却
	edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Transform)->RemoveMapID(transform->mapID.value());
	// 削除前にComponentを記録する
	m_Transform = *transform;
	if (meshFilter) { m_MeshFilter = *meshFilter; }
	MeshRendererComponent* meshRenderer = edit->m_GameCore->GetECSManager()->GetComponent<MeshRendererComponent>(object.GetEntity());
	if (meshRenderer) { m_MeshRenderer = *meshRenderer; }
	ScriptComponent* script = edit->m_GameCore->GetECSManager()->GetComponent<ScriptComponent>(object.GetEntity());
	if (script) { m_Script = *script; }
	std::vector<LineRendererComponent>* lineRenderer = edit->m_GameCore->GetECSManager()->GetAllComponents<LineRendererComponent>(object.GetEntity());
	if (lineRenderer) { m_LineRenderer = *lineRenderer; }
	Rigidbody2DComponent* rb = edit->m_GameCore->GetECSManager()->GetComponent<Rigidbody2DComponent>(object.GetEntity());
	if (rb) { m_Rigidbody2D = *rb; }
	BoxCollider2DComponent* box = edit->m_GameCore->GetECSManager()->GetComponent<BoxCollider2DComponent>(object.GetEntity());
	if (box) { m_BoxCollider2D = *box; }
	// Componentの削除
	edit->m_GameCore->GetECSManager()->RemoveComponent<TransformComponent>(object.GetEntity());
	if (meshFilter) { edit->m_GameCore->GetECSManager()->RemoveComponent<MeshFilterComponent>(object.GetEntity()); }
	if (meshRenderer) { edit->m_GameCore->GetECSManager()->RemoveComponent<MeshRendererComponent>(object.GetEntity()); }
	if (script) { edit->m_GameCore->GetECSManager()->RemoveComponent<ScriptComponent>(object.GetEntity()); }
	if (lineRenderer) { edit->m_GameCore->GetECSManager()->RemoveAllComponents<LineRendererComponent>(object.GetEntity()); }
	if (rb) { edit->m_GameCore->GetECSManager()->RemoveComponent<Rigidbody2DComponent>(object.GetEntity()); }
	if (box) { edit->m_GameCore->GetECSManager()->RemoveComponent<BoxCollider2DComponent>(object.GetEntity()); }
	// Entityの削除
	edit->m_GameCore->GetECSManager()->RemoveEntity(object.GetEntity());
	// CurrentSceneから削除
	edit->m_GameCore->GetSceneManager()->GetCurrentScene()->RemoveUseObject(m_ObjectID);
	// GameObjectの削除
	edit->m_GameCore->GetObjectContainer()->DeleteGameObject(object.GetID().value());
	return true;
}

bool DeleteObjectCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool RenameObjectCommand::Execute(EngineCommand* edit)
{
	GameObject& object = edit->m_GameCore->GetObjectContainer()->GetGameObject(m_ObjectID);
	if (!object.IsActive()) { return false; }
	// 変更前の名前を保存
	m_PreName = object.GetName();
	// 名前があるかどうかを確認
	std::unordered_map<std::wstring, ObjectID>& nameToObjectID = edit->m_GameCore->GetObjectContainer()->GetNameToObjectID();
	auto it = nameToObjectID.find(m_PreName);
	if (!nameToObjectID.contains(m_PreName))
	{
		return false;
	}
	// 名前の重複を確認
	m_NewName = GenerateUniqueName(m_NewName, edit->m_GameCore->GetObjectContainer()->GetNameToObjectID());
	// 名前コンテナから削除
	nameToObjectID.erase(it);
	// 名前を変更
	nameToObjectID[m_NewName] = object.GetID().value();
	object.SetName(m_NewName);
	return true;
}

bool RenameObjectCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}
