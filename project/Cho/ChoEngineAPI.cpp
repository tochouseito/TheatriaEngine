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
//CHO_API GameObject& ChoSystem::FindGameObjectByName(std::wstring_view name)
//{
//	GameCore* gameCore = g_Engine->GetEngineCommand()->GetGameCore();
//	GameObject& result = gameCore->GetObjectContainer()->GetGameObjectByName(name.data());
//	if (!result.IsActive())
//	{
//		return gameCore->GetObjectContainer()->GetDummyGameObject();
//	}
//	result.Initialize(false);
//	return result;
//}

CHO_API GameObject& ChoSystem::CloneGameObject(std::optional<uint32_t> id, Vector3 generatePosition)
{
	EngineCommand* engineCommand = g_Engine->GetEngineCommand();
	GameObject& object = engineCommand->GetGameCore()->GetObjectContainer()->GetGameObject(id.value());
	std::unique_ptr<CloneObjectCommand> command = std::make_unique<CloneObjectCommand>(object.GetCurrentSceneName());
	command->Execute(engineCommand);
	GameObject& newObject = engineCommand->GetGameCore()->GetObjectContainer()->GetGameObject(command->GetObjectID());
	// nameを変更
	std::unique_ptr<RenameObjectCommand> renameCommand = std::make_unique<RenameObjectCommand>(newObject.GetID().value(), object.GetName());
	renameCommand->Execute(engineCommand);
	// Tagのコピー
	newObject.SetTag(object.GetTag());
	// CurrendSceneのコピー
	newObject.SetCurrentSceneName(object.GetCurrentSceneName());
	// Componentを取得
	TransformComponent* transform = engineCommand->GetGameCore()->GetECSManager()->GetComponent<TransformComponent>(newObject.GetEntity());


	MeshFilterComponent* meshFilter = engineCommand->GetGameCore()->GetECSManager()->GetComponent<MeshFilterComponent>(object.GetEntity());
	MeshRendererComponent* meshRenderer = engineCommand->GetGameCore()->GetECSManager()->GetComponent<MeshRendererComponent>(object.GetEntity());
	ScriptComponent* script = engineCommand->GetGameCore()->GetECSManager()->GetComponent<ScriptComponent>(object.GetEntity());
	std::vector<LineRendererComponent>* lineRenderer = engineCommand->GetGameCore()->GetECSManager()->GetAllComponents<LineRendererComponent>(object.GetEntity());
	Rigidbody2DComponent* rb = engineCommand->GetGameCore()->GetECSManager()->GetComponent<Rigidbody2DComponent>(object.GetEntity());
	BoxCollider2DComponent* box = engineCommand->GetGameCore()->GetECSManager()->GetComponent<BoxCollider2DComponent>(object.GetEntity());
	MaterialComponent* material = engineCommand->GetGameCore()->GetECSManager()->GetComponent<MaterialComponent>(object.GetEntity());
	AudioComponent* audio = engineCommand->GetGameCore()->GetECSManager()->GetComponent<AudioComponent>(object.GetEntity());
	AnimationComponent* animation = engineCommand->GetGameCore()->GetECSManager()->GetComponent<AnimationComponent>(object.GetEntity());
	// あるやつをnewObjectに追加
	if (meshFilter)
	{
		std::unique_ptr<AddMeshFilterComponent> addMeshFilterCommand = std::make_unique<AddMeshFilterComponent>(newObject.GetEntity());
		addMeshFilterCommand->Execute(engineCommand);
		// MeshFilterComponentのMeshを設定
		MeshFilterComponent* newMeshFilter = engineCommand->GetGameCore()->GetECSManager()->GetComponent<MeshFilterComponent>(newObject.GetEntity());
		if (newMeshFilter)
		{
			newMeshFilter->modelID = g_Engine->GetEngineCommand()->GetResourceManager()->GetModelManager()->GetModelDataIndex(meshFilter->modelName);
			newMeshFilter->modelName = meshFilter->modelName;
			g_Engine->GetEngineCommand()->GetResourceManager()->GetModelManager()->RegisterModelUseList(meshFilter->modelID.value(), transform->mapID.value());
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
	if (material)
	{
		std::unique_ptr<AddMaterialComponent> addMaterialCommand = std::make_unique<AddMaterialComponent>(newObject.GetEntity());
		addMaterialCommand->Execute(engineCommand);
		MaterialComponent* newMaterial = engineCommand->GetGameCore()->GetECSManager()->GetComponent<MaterialComponent>(newObject.GetEntity());
		if (newMaterial)
		{
			newMaterial->color = material->color;
			newMaterial->enableLighting = material->enableLighting;
			newMaterial->enableTexture = material->enableTexture;
			newMaterial->uvFlipY = material->uvFlipY;
			newMaterial->shininess = material->shininess;
			newMaterial->textureName = material->textureName;
		}
	}
	if(audio)
	{
		std::unique_ptr<AddAudioComponent> addAudioCommand = std::make_unique<AddAudioComponent>(newObject.GetEntity());
		addAudioCommand->Execute(engineCommand);
		AudioComponent* newAudio = engineCommand->GetGameCore()->GetECSManager()->GetComponent<AudioComponent>(newObject.GetEntity());
		if (newAudio)
		{
			for (const auto& sound : audio->soundData)
			{
				SoundData newSound = g_Engine->GetEngineCommand()->GetResourceManager()->GetAudioManager()->CreateSoundData(sound.name);
				if(!newSound.name.empty())
				{
					newAudio->soundData.push_back(std::move(newSound));
				}
			}
			newAudio->isLoop = audio->isLoop;
			newAudio->isPlay = audio->isPlay;
			newAudio->isPause = audio->isPause;
			newAudio->isStop = audio->isStop;
		}
	}
	if(animation)
	{
		std::unique_ptr<AddAnimationComponent> addAnimationCommand = std::make_unique<AddAnimationComponent>(newObject.GetEntity());
		addAnimationCommand->Execute(engineCommand);
		AnimationComponent* newAnimation = engineCommand->GetGameCore()->GetECSManager()->GetComponent<AnimationComponent>(newObject.GetEntity());
		if (newAnimation)
		{
			newAnimation->modelName = animation->modelName;
			newAnimation->numAnimation = animation->numAnimation;
			newAnimation->transitionDuration = animation->transitionDuration;
			ModelData* model = g_Engine->GetEngineCommand()->GetResourceManager()->GetModelManager()->GetModelData(meshFilter->modelID.value());
			if (model->isBone)
			{
				newAnimation->skeleton = model->skeleton;
				newAnimation->skinCluster = model->skinCluster;
				newAnimation->boneOffsetID = model->AllocateBoneOffsetIdx();
			}
		}
	}
	// 初期位置を設定
	if (transform)
	{
		transform->position = generatePosition;
	}
	engineCommand->GetGameCore()->AddGameGenerateObject(newObject.GetID().value());
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
