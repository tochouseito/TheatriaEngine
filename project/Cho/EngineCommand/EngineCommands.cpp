#include "pch.h"
#include "EngineCommands.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "GameCore/GameCore.h"
#include "Editor/EditorManager/EditorManager.h"
#include "Core/Utility/GenerateUnique.h"
#include "Core/ChoLog/ChoLog.h"
using namespace Cho;
#include <random>

bool Add3DObjectCommand::Execute(EngineCommand* edit)
{
	// デフォルトの名前
	std::wstring name = L"NewMeshObject";
	// Worldに追加
	m_Handle = edit->GetGameCore()->GetGameWorld()->CreateGameObject(name, ObjectType::MeshObject);
	// SelectedObjectを設定
	edit->GetEditorManager()->SetSelectedGameObject(edit->GetGameCore()->GetGameWorld()->GetGameObject(m_Handle));
	return true;
}

bool Add3DObjectCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddCameraObjectCommand::Execute(EngineCommand* edit)
{
	// デフォルトの名前
	std::wstring name = L"NewCameraObject";
	// Worldに追加
	m_Handle = edit->GetGameCore()->GetGameWorld()->CreateGameObject(name, ObjectType::Camera);
	// SelectedObjectを設定
	edit->GetEditorManager()->SetSelectedGameObject(edit->GetGameCore()->GetGameWorld()->GetGameObject(m_Handle));
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
	edit->m_GameCore->GetECSManager()->AddComponent<MeshFilterComponent>(m_Entity);
	//// Transformとの連携
	//TransformComponent* transform = edit->m_GameCore->GetECSManager()->GetComponent<TransformComponent>(m_Entity);
	//// デフォルトモデルとしてCube
	//mesh->modelName = L"Cube";
	//// モデルのIDを取得
	//mesh->modelID = edit->m_ResourceManager->GetModelManager()->GetModelDataIndex(mesh->modelName);
	//// モデルのUseListに登録
	//edit->m_ResourceManager->GetModelManager()->RegisterModelUseList(mesh->modelID.value(), transform->mapID.value());

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
	if (!edit->m_GameCore->GetSceneManager()->GetMainScene())
	{
		Log::Write(LogLevel::Assert, "Current Scene is nullptr");
		return false;
	}
	// 現在のMainCameraIDを取得
	m_PreCameraID = edit->m_GameCore->GetSceneManager()->GetMainScene()->GetMainCameraID();
	// MainCameraを設定
	edit->m_GameCore->GetSceneManager()->GetMainScene()->SetMainCameraID(m_SetCameraID.value());

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
	edit->m_GameCore->GetECSManager()->AddComponent<ScriptComponent>(m_Entity);
	// Entityを設定
	//script->entity = m_Entity;
	//script->objectID = m_ObjectID;
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
	edit->m_GameCore->GetECSManager()->AddComponent<LineRendererComponent>(m_Entity);
	// mapIDを取得
	//uint32_t mapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Line)->GetMapID();
	// mapIDを設定
	//line->mapID = mapID;
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
	edit->m_GameCore->GetECSManager()->AddComponent<Rigidbody2DComponent>(m_Entity);
	//rb->selfObjectID = m_ObjectID;
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
	edit->m_GameCore->GetECSManager()->AddComponent<BoxCollider2DComponent>(m_Entity);
	return true;
}

bool AddBoxCollider2DComponent::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool DeleteObjectCommand::Execute(EngineCommand* edit)
{
	ECSManager* ecs = edit->m_GameCore->GetECSManager();
	GameObject* object = edit->m_GameCore->GetObjectContainer()->GetGameObject(m_ObjectID);
	if (!object) { return false; }
	Entity entity = object->GetEntity();
	// Prefab作成
	m_Prefab = std::make_unique<IPrefab>(IPrefab::FromEntity(*ecs, entity));
	m_Prefab->SetName(object->GetName());
	m_Prefab->SetType(object->GetType());
	m_Prefab->SetTag(object->GetTag());
	// Entityの削除
	ecs->RemoveEntity(entity);
	// CurrentSceneから削除
	edit->m_GameCore->GetSceneManager()->GetMainScene()->RemoveUseObject(m_ObjectID);
	// GameObjectの削除
	edit->m_GameCore->GetObjectContainer()->DeleteGameObject(object->GetID().value());
	return true;
}

bool DeleteObjectCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool RenameObjectCommand::Execute(EngineCommand* edit)
{
	GameObject* object = edit->m_GameCore->GetObjectContainer()->GetGameObject(m_ObjectID);
	if (!object->IsActive()) { return false; }
	// 変更前の名前を保存
	m_PreName = object->GetName();
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
	nameToObjectID[m_NewName] = object->GetID().value();
	object->SetName(m_NewName);
	return true;
}

bool RenameObjectCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddMaterialComponent::Execute(EngineCommand* edit)
{
	// MaterialComponentを追加
	edit->m_GameCore->GetECSManager()->AddComponent<MaterialComponent>(m_Entity);
	//// 初期値
	//Color color;
	//material->color = color.From255(200, 200, 200);
	//material->enableLighting = true;
	//material->matUV = Matrix4::Identity();
	//material->shininess = 50.0f;
	//// Material統合バッファからmapIDを取得
	//uint32_t mapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Material)->GetMapID();
	//TransformComponent* transform = edit->m_GameCore->GetECSManager()->GetComponent<TransformComponent>(m_Entity);
	//transform->materialID = mapID;
	//material->mapID = mapID;
	return true;
}

bool AddMaterialComponent::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddParticleSystemObjectCommand::Execute(EngineCommand* edit)
{
	// CurrentSceneがないなら失敗
	if (!edit->m_GameCore->GetSceneManager()->GetMainScene())
	{
		Log::Write(LogLevel::Assert, "Current Scene is nullptr");
		return false;
	}
	// 各IDの取得
	// Entity
	Entity entity = edit->m_GameCore->GetECSManager()->GenerateEntity();
	// デフォルトの名前
	std::wstring name = L"NewParticleSystem";
	// 重複回避
	name = GenerateUniqueName(name, edit->m_GameCore->GetObjectContainer()->GetNameToObjectID());
	// Transform統合バッファからmapIDを取得
	//uint32_t mapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
	// TransformComponentを追加
	TransformComponent* transform = edit->m_GameCore->GetECSManager()->AddComponent<TransformComponent>(entity);
	//transform->mapID = mapID;
	// GameObjectを追加
	ObjectID objectID = edit->m_GameCore->GetObjectContainer()->AddGameObject(entity, name, ObjectType::ParticleSystem);
	m_ObjectID = objectID;
	// シーンに追加
	edit->m_GameCore->GetSceneManager()->GetMainScene()->AddUseObject(objectID);
	// SelectedObjectを設定
	edit->SetSelectedObject(edit->m_GameCore->GetObjectContainer()->GetGameObject(m_ObjectID)->GetName());
	transform->scale.Zero();
	// MeshFilterComponentを追加
	std::unique_ptr<AddMeshFilterComponent> addMeshFilter = std::make_unique<AddMeshFilterComponent>(entity);
	addMeshFilter->Execute(edit);
	// MeshRendererComponentを追加
	std::unique_ptr<AddMeshRendererComponent> addMeshRenderer = std::make_unique<AddMeshRendererComponent>(entity);
	addMeshRenderer->Execute(edit);
	// MaterialComponentを追加
	std::unique_ptr<AddMaterialComponent> addMaterial = std::make_unique<AddMaterialComponent>(entity);
	addMaterial->Execute(edit);
	// EmitterComponentを追加
	std::unique_ptr<AddEmitterComponent> addEmitter = std::make_unique<AddEmitterComponent>(entity);
	addEmitter->Execute(edit);
	EmitterComponent* emitter = edit->m_GameCore->GetECSManager()->GetComponent<EmitterComponent>(entity);
	emitter->scale.value.x.median = 1.0f;
	emitter->scale.value.y.median = 1.0f;
	emitter->scale.value.z.median = 1.0f;
	emitter->lifeTime.median = 20.0f;
	// ParticleComponentを追加
	std::unique_ptr<AddParticleComponent> addParticle = std::make_unique<AddParticleComponent>(entity);
	addParticle->Execute(edit);
	return true;
}

bool AddParticleSystemObjectCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddEmitterComponent::Execute(EngineCommand* edit)
{
	// EmitterComponentを追加
	edit->m_GameCore->GetECSManager()->AddComponent<EmitterComponent>(m_Entity);
	//if (!emitter) { return false; }
	//emitter->bufferIndex = edit->m_ResourceManager->CreateStructuredBuffer<BUFFER_DATA_EMITTER>(1);
	return true;
}

bool AddEmitterComponent::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddParticleComponent::Execute(EngineCommand* edit)
{
	// ParticleComponentを追加
	edit->m_GameCore->GetECSManager()->AddComponent<ParticleComponent>(m_Entity);
	//if (!particle) { return false; }
	// Resourceの生成
	// パーティクル
	//particle->bufferIndex = edit->m_ResourceManager->CreateRWStructuredBuffer<BUFFER_DATA_PARTICLE>(particle->count);
	// PerFrame
	//particle->perFrameBufferIndex = edit->m_ResourceManager->CreateConstantBuffer<BUFFER_DATA_PARTICLE_PERFRAME>();
	// FreeListIndex
	//particle->freeListIndexBufferIndex = edit->m_ResourceManager->CreateRWStructuredBuffer<int32_t>(1);
	// FreeList
	//particle->freeListBufferIndex = edit->m_ResourceManager->CreateRWStructuredBuffer<uint32_t>(particle->count,true);
	return true;
}

bool AddParticleComponent::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddEffectObjectCommand::Execute(EngineCommand* edit)
{
	// CurrentSceneがないなら失敗
	if (!edit->m_GameCore->GetSceneManager()->GetMainScene())
	{
		Log::Write(LogLevel::Assert, "Current Scene is nullptr");
		return false;
	}
	// 各IDの取得
	// Entity
	Entity entity = edit->m_GameCore->GetECSManager()->GenerateEntity();
	m_Entity = entity;
	// デフォルトの名前
	std::wstring name = L"EditorEffect";
	// 重複回避
	name = GenerateUniqueName(name, edit->m_GameCore->GetObjectContainer()->GetNameToObjectID());
	// EffectComponentを追加
	edit->m_GameCore->GetECSManager()->AddComponent<EffectComponent>(entity);
	/*if (!effect) { return false; }
	effect->isRun = false;
	effect->isLoop = true;*/
	return true;
}

bool AddEffectObjectCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddUIObjectCommand::Execute(EngineCommand* edit)
{
	// CurrentSceneがないなら失敗
	if (!edit->m_GameCore->GetSceneManager()->GetMainScene())
	{
		Log::Write(LogLevel::Assert, "Current Scene is nullptr");
		return false;
	}
	// 各IDの取得
	// Entity
	Entity entity = edit->m_GameCore->GetECSManager()->GenerateEntity();
	// デフォルトの名前
	std::wstring name = L"NewUI";
	// 重複回避
	name = GenerateUniqueName(name, edit->m_GameCore->GetObjectContainer()->GetNameToObjectID());
	// Transform統合バッファからmapIDを取得
	//uint32_t tfMapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
	// TransformComponentを追加
	edit->m_GameCore->GetECSManager()->AddComponent<TransformComponent>(entity);
	//transform->mapID = tfMapID;
	// UISprite統合バッファからmapIDを取得
	//uint32_t mapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::UISprite)->GetMapID();
	// SpriteComponentを追加
	edit->m_GameCore->GetECSManager()->AddComponent<UISpriteComponent>(entity);
	//uiSprite->mapID = mapID;
	// UseListに登録
	//edit->m_ResourceManager->GetUIContainer()->AddUI(mapID);
	// GameObjectを追加
	ObjectID objectID = edit->m_GameCore->GetObjectContainer()->AddGameObject(entity, name, ObjectType::UI);
	if (objectID == entity)
	{
		Log::Write(LogLevel::Assert, "Failed to add GameObject to ObjectContainer. ObjectID is same as Entity.");
		return false;
	}
	m_ObjectID = objectID;
	// シーンに追加
	edit->m_GameCore->GetSceneManager()->GetMainScene()->AddUseObject(objectID);
	// SelectedObjectを設定
	edit->SetSelectedObject(edit->m_GameCore->GetObjectContainer()->GetGameObject(m_ObjectID)->GetName());
	return true;
}

bool AddUIObjectCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool SetGravityCommand::Execute(EngineCommand* edit)
{
	b2World* world = edit->m_GameCore->GetPhysicsWorld();
	if (!world) { return false; }
	// 重力を取得
	b2Vec2 gravity = world->GetGravity();
	m_PreGravity = Vector3(gravity.x, gravity.y, 0.0f);
	// 重力を設定
	world->SetGravity(b2Vec2(m_Gravity.x, m_Gravity.y));
	return true;
}

bool SetGravityCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddLightObjectCommand::Execute(EngineCommand* edit)
{
	// CurrentSceneがないなら失敗
	if (!edit->m_GameCore->GetSceneManager()->GetMainScene())
	{
		Log::Write(LogLevel::Assert, "Current Scene is nullptr");
		return false;
	}
	// 各IDの取得
	// Entity
	Entity entity = edit->m_GameCore->GetECSManager()->GenerateEntity();
	// デフォルトの名前
	std::wstring name = L"NewLight";
	// 重複回避
	name = GenerateUniqueName(name, edit->m_GameCore->GetObjectContainer()->GetNameToObjectID());
	// Transform統合バッファからmapIDを取得
	//uint32_t tfMapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
	// TransformComponentを追加
	edit->m_GameCore->GetECSManager()->AddComponent<TransformComponent>(entity);
	//transform->mapID = tfMapID;
	// LightバッファからmapIDを取得
	//uint32_t mapID = edit->m_ResourceManager->GetLightIndex();
	// LightComponentを追加
	edit->m_GameCore->GetECSManager()->AddComponent<LightComponent>(entity);
	//light->mapID = mapID;
	// GameObjectを追加
	ObjectID objectID = edit->m_GameCore->GetObjectContainer()->AddGameObject(entity, name, ObjectType::Light);
	m_ObjectID = objectID;
	// シーンに追加
	edit->m_GameCore->GetSceneManager()->GetMainScene()->AddUseObject(objectID);
	// SelectedObjectを設定
	edit->SetSelectedObject(edit->m_GameCore->GetObjectContainer()->GetGameObject(m_ObjectID)->GetName());
	return true;
}

bool AddLightObjectCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddAudioComponent::Execute(EngineCommand* edit)
{
	// AudioComponentを追加
	AudioComponent* audio = edit->m_GameCore->GetECSManager()->AddComponent<AudioComponent>(m_Entity);
	if (!audio) { return false; }
	return true;
}

bool AddAudioComponent::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddAnimationComponent::Execute(EngineCommand* edit)
{
	// AnimationComponentを追加
	MeshFilterComponent* meshFilter = edit->m_GameCore->GetECSManager()->GetComponent<MeshFilterComponent>(m_Entity);
	if (!meshFilter) { return false; }
	ModelData* model = edit->m_ResourceManager->GetModelManager()->GetModelData(meshFilter->modelID.value());
	if (!model) { return false; }
	if (model->animations.empty()) { return false; }
	AnimationComponent* animation = edit->m_GameCore->GetECSManager()->AddComponent<AnimationComponent>(m_Entity);
	if (!animation) { return false; }
	/*if (model->isBone)
	{
		animation->skeleton = model->skeleton;
		animation->skinCluster = model->skinCluster;
		animation->modelName = model->name;
		animation->boneOffsetID = model->AllocateBoneOffsetIdx();
	}*/
	
	return true;
}

bool AddAnimationComponent::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool CopyGameObjectCommand::Execute(EngineCommand* edit)
{
	ECSManager* ecs = edit->m_GameCore->GetECSManager();
	// GameObjectを取得
	GameObject* object = edit->m_GameCore->GetObjectContainer()->GetGameObject(m_SrcID);
	if (!object->IsActive()) { return false; }
	// オブジェクト名とタイプを取得
	std::wstring name = object->GetName();
	name = GenerateUniqueName(name, edit->m_GameCore->GetObjectContainer()->GetNameToObjectID());
	// Entityを生成
	Entity entity = edit->m_GameCore->GetECSManager()->CopyEntity(object->GetEntity());
	// GameObjectを追加
	ObjectID objectID = edit->m_GameCore->GetObjectContainer()->AddGameObject(entity, name, object->GetType());
	GameObject* newObj = edit->m_GameCore->GetObjectContainer()->GetGameObjectByName(name);
	// タイプ、タグのコピー
	newObj->SetType(object->GetType());
	newObj->SetTag(object->GetTag());
	// SceneUseListに登録
	edit->m_GameCore->GetSceneManager()->GetMainScene()->AddUseObject(newObj->GetID().value());
	return true;
}

bool CopyGameObjectCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool CreateEffectCommand::Execute(EngineCommand* edit)
{
	// CurrentSceneがないなら失敗
	if (!edit->m_GameCore->GetSceneManager()->GetMainScene())
	{
		Log::Write(LogLevel::Assert, "Current Scene is nullptr");
		return false;
	}
	// 作成中のEffectObjectがあるなら失敗
	if (edit->GetEffectEntity().has_value()) { return false; }
	// 各IDの取得
	// Entity
	Entity entity = edit->m_GameCore->GetECSManager()->GenerateEntity();
	m_Entity = entity;
	// デフォルトの名前
	std::wstring name = L"EditorEffect";
	// 重複回避
	name = GenerateUniqueName(name, edit->m_GameCore->GetObjectContainer()->GetNameToObjectID());
	// EffectComponentを追加
	EffectComponent* effect = edit->m_GameCore->GetECSManager()->AddComponent<EffectComponent>(entity);
	if (!effect) { return false; }
	//effect->isRun = false;
	//effect->isLoop = true;
	edit->SetEffectEntity(entity);
	// Rootを取得
	uint32_t rootID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::EffectRootInt)->GetMapID();
	effect->root = std::make_pair(rootID, EffectRootData());
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(1, 1000);
	effect->root.second.globalSeed = static_cast<uint32_t>(dist(gen));
	// UseListに登録
	edit->m_ResourceManager->AddEffectRootUseList(rootID);
	// Nodeを一つ追加
	uint32_t nodeID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::EffectNodeInt)->GetMapID();
	effect->root.second.nodeCount++;
	EffectNodeData nodedata;
	nodedata.nodeID = nodeID;
	// Rootを親に設定
	nodedata.isRootParent = 1;
	// 初期はSprite
	uint32_t spriteID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::EffectSpriteInt)->GetMapID();
	nodedata.draw.meshDataIndex = spriteID;
	nodedata.draw.meshType = static_cast<uint32_t>(EFFECT_MESH_TYPE::SPRITE);
	EffectSprite sprite;
	nodedata.drawMesh = sprite;
	nodedata.name = "NewNode";
	nodedata.scale.value = Vector3(1.0f, 1.0f, 1.0f);
	nodedata.scale.pva.value.x.median = 1.0f;
	nodedata.scale.pva.value.y.median = 1.0f;
	nodedata.scale.pva.value.z.median = 1.0f;
	effect->root.second.nodes.push_back(std::move(nodedata));
	return true;
}

bool CreateEffectCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool AddEffectNodeCommand::Execute(EngineCommand* edit)
{
	// CurrentSceneがないなら失敗
	if (!edit->m_GameCore->GetSceneManager()->GetMainScene())
	{
		Log::Write(LogLevel::Assert, "Current Scene is nullptr");
		return false;
	}
	// 作成中のEffectObjectがないなら失敗
	if (!edit->GetEffectEntity().has_value()) { return false; }
	// EffectComponentを取得
	EffectComponent* effect = edit->m_GameCore->GetECSManager()->GetComponent<EffectComponent>(edit->GetEffectEntity().value());
	if (!effect) { return false; }
	// 新しいNodeのIDを取得
	uint32_t nodeID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::EffectNodeInt)->GetMapID();
	//std::span<EffectRoot> rootSpan = dynamic_cast<StructuredBuffer<EffectRoot>*>(edit->m_ResourceManager->GetIntegrationBuffer(IntegrationDataType::EffectRootInt))->GetMappedData();
	// EffectRootのNodeIDに追加
	effect->root.second.nodeCount++;
	EffectNodeData nodedata;
	nodedata.nodeID = nodeID;
	// 選択中のNodeを親に設定
	if (edit->m_EffectNodeID.has_value())
	{
		nodedata.isRootParent = 0;
		nodedata.parentIndex = effect->root.second.nodes[edit->m_EffectNodeID.value()].nodeID;
	}
	else
	{
		// 親がないならRootを親に設定
		nodedata.isRootParent = 1;
	}
	// 初期はSprite
	uint32_t spriteID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::EffectSpriteInt)->GetMapID();
	nodedata.draw.meshDataIndex = spriteID;
	nodedata.draw.meshType = static_cast<uint32_t>(EFFECT_MESH_TYPE::SPRITE);
	EffectSprite sprite;
	nodedata.drawMesh = sprite;
	nodedata.name = "NewNode";
	nodedata.scale.value = Vector3(1.0f, 1.0f, 1.0f);
	nodedata.scale.pva.value.x.median = 1.0f;
	nodedata.scale.pva.value.y.median = 1.0f;
	nodedata.scale.pva.value.z.median = 1.0f;
	// Nodeを追加
	effect->root.second.nodes.push_back(std::move(nodedata));
	// 追加したNodeを選択中に設定
	edit->SetEffectNodeIndex(nodeID);
	return true;
}

bool AddEffectNodeCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool CloneObjectCommand::Execute(EngineCommand* edit)
{
	ECSManager* ecs = edit->m_GameCore->GetECSManager();
	// GameObjectを取得
	GameObject* object = edit->m_GameCore->GetObjectContainer()->GetGameObject(m_SrcID);
	if (!object->IsActive()) { return false; }
	//GameObjectData objData = *object;
	// オブジェクト名とタイプを取得
	std::wstring name = object->GetName();
	name = GenerateUniqueName(name, edit->m_GameCore->GetObjectContainer()->GetNameToObjectID());
	ObjectType type = object->GetType();
	// Entityを生成
	Entity entity = edit->m_GameCore->GetECSManager()->CopyEntity(object->GetEntity());
	// GameObjectを追加
	ObjectID objectID = edit->m_GameCore->GetObjectContainer()->AddGameObject(entity, name, type);
	m_DstID = objectID;
	GameObject* newObj = edit->m_GameCore->GetObjectContainer()->GetGameObjectByName(name);
	// タイプ、タグのコピー
	newObj->SetType(type);
	newObj->SetTag(object->GetTag());
	// SceneUseListに登録
	// シーンに追加
	edit->m_GameCore->GetSceneManager()->GetScene(edit->m_GameCore->GetSceneManager()->GetSceneID(m_CurrendSceneName))->AddClonedObject(objectID);
	return true;
}

bool CloneObjectCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}