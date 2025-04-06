#include "pch.h"
#include "GameCoreCommand.h"
#include "Cho/GameCore/GameCore.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Core/ChoLog/ChoLog.h"
#include "Core/Utility/GenerateUnique.h"
using namespace Cho;

GameObject* GameCoreCommand::Add3DObject(const uint32_t& mapID)
{
	if (!m_pSceneManager->GetCurrentScene())
	{
		Log::Write(LogLevel::Assert, "Current Scene is nullptr");
		return nullptr;
	}
	// エンティティ生成
	Entity entity = m_pECSManager->GenerateEntity();
	// デフォルトの名前を設定
	std::wstring name = L"NewMeshObject";
	// 重複回避
	name = GenerateUniqueName(name, m_pObjectContainer->GetNameToObjectID());
	// TransformComponentを追加
	TransformComponent* transform = m_pECSManager->AddComponent<TransformComponent>(entity);
	// mapIDを取得
	transform->mapID = mapID;
	// GameObjectを追加
	ObjectID objectID = m_pObjectContainer->AddGameObject(entity, name, ObjectType::MeshObject);
	m_pSceneManager->GetCurrentScene()->AddUseObject(objectID);
	return m_pObjectContainer->GetGameObject(objectID);
}

GameObject* GameCoreCommand::AddCameraObject(const uint32_t& mapID, ResourceManager* resourceManager)
{
	if (!m_pSceneManager->GetCurrentScene())
	{
		Log::Write(LogLevel::Assert, "Current Scene is nullptr");
		return nullptr;
	}
	// エンティティ生成
	Entity entity = m_pECSManager->GenerateEntity();
	// デフォルトの名前を設定
	std::wstring name = L"NewCamera";
	// 重複回避
	name = GenerateUniqueName(name, m_pObjectContainer->GetNameToObjectID());
	// TransformComponentを追加
	TransformComponent* transform = m_pECSManager->AddComponent<TransformComponent>(entity);
	// mapIDを取得
	transform->mapID = mapID;
	// CameraComponentを追加
	CameraComponent* camera = m_pECSManager->AddComponent<CameraComponent>(entity);
	if (!camera)
	{
		Log::Write(LogLevel::Assert, "CameraComponent is nullptr");
	}
	// Resourceの生成
	camera->bufferIndex = resourceManager->CreateConstantBuffer<BUFFER_DATA_VIEWPROJECTION>();
	// GameObjectを追加
	ObjectID objectID = m_pObjectContainer->AddGameObject(entity, name, ObjectType::Camera);
	m_pSceneManager->GetCurrentScene()->AddUseObject(objectID);
	return m_pObjectContainer->GetGameObject(objectID);
}

void GameCoreCommand::AddMeshFilterComponent(const uint32_t& entity, ModelManager* modelManager)
{
	MeshFilterComponent* mesh = m_pECSManager->AddComponent<MeshFilterComponent>(entity);
	mesh->modelID = 0;// 一旦0にしておく
	// Transformがあるとき、連携する
	TransformComponent* transform = m_pECSManager->GetComponent<TransformComponent>(entity);
	if (transform)
	{
		// ModelのUseListに登録
		mesh->modelID = modelManager->GetModelDataIndex(L"Cube", transform->mapID);
	} else
	{
		Log::Write(LogLevel::Assert, "TransformComponent is nullptr");
	}
}

void GameCoreCommand::AddMeshRendererComponent(const uint32_t& entity)
{
	MeshRendererComponent* render = m_pECSManager->AddComponent<MeshRendererComponent>(entity);
	render->visible = true;
}

void GameCoreCommand::AddCameraComponent(const uint32_t& entity, ResourceManager* resourceManager)
{
	CameraComponent* camera = m_pECSManager->AddComponent<CameraComponent>(entity);
	if (!camera)
	{
		Log::Write(LogLevel::Assert, "CameraComponent is nullptr");
	}
	// Resourceの生成
	camera->bufferIndex = resourceManager->CreateConstantBuffer<BUFFER_DATA_VIEWPROJECTION>();
}

std::optional<uint32_t> GameCoreCommand::SetMainCamera(const uint32_t& setCameraID)
{
	if (!m_pSceneManager->GetCurrentScene())
	{
		Log::Write(LogLevel::Assert, "Current Scene is nullptr");
	}
	std::optional<uint32_t> preCameraID = m_pSceneManager->GetCurrentScene()->GetMainCameraID();
	m_pSceneManager->GetCurrentScene()->SetMainCameraID(setCameraID);
	return preCameraID;
}
