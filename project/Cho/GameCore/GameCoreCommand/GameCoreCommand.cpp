#include "pch.h"
#include "GameCoreCommand.h"
#include "Cho/GameCore/GameCore.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Core/ChoLog/ChoLog.h"
using namespace Cho;

void GameCoreCommand::AddGameObject(const std::wstring& name, const uint32_t& type)
{
	if (!m_pSceneManager->GetCurrentScene())
	{
		Log::Write(LogLevel::Assert, "Current Scene is nullptr");
		return;
	}
	Entity entity = m_pECSManager->GenerateEntity();
	m_pSceneManager->GetCurrentScene()->AddUseObject(m_pObjectContainer->AddGameObject(entity, name,static_cast<ObjectType>(type)));
}

void GameCoreCommand::AddTransformComponent(const uint32_t& entity, const uint32_t& mapID)
{
	TransformComponent* transform = m_pECSManager->AddComponent<TransformComponent>(entity);
	// mapIDを取得
	transform->mapID = mapID;
}

void GameCoreCommand::AddMeshComponent(const uint32_t& entity, ModelManager* modelManager)
{
	MeshComponent* mesh = m_pECSManager->AddComponent<MeshComponent>(entity);
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

void GameCoreCommand::AddRenderComponent(const uint32_t& entity)
{
	RenderComponent* render = m_pECSManager->AddComponent<RenderComponent>(entity);
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
