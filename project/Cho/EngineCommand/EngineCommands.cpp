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
	MaterialComponent* material = edit->m_GameCore->GetECSManager()->GetComponent<MaterialComponent>(object.GetEntity());
	if (material)
	{
		// Material統合バッファからmapIDを返却
		edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Material)->RemoveMapID(material->mapID.value());
	}
	std::vector<LineRendererComponent>* lineRenderer = edit->m_GameCore->GetECSManager()->GetAllComponents<LineRendererComponent>(object.GetEntity());
	if (lineRenderer)
	{
		for (auto& line : *lineRenderer)
		{
			// Line統合バッファからmapIDを返却
			edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Line)->RemoveMapID(line.mapID.value());
		}
	}
	// CameraComponentを取得
	CameraComponent* camera = edit->m_GameCore->GetECSManager()->GetComponent<CameraComponent>(object.GetEntity());
	if (camera)
	{
		// シーンのMainCamaraなら削除
		if (edit->m_GameCore->GetSceneManager()->GetCurrentScene()->GetMainCameraID() == m_ObjectID)
		{
			edit->m_GameCore->GetSceneManager()->GetCurrentScene()->SetMainCameraID(std::nullopt);
		}
	}
	UISpriteComponent* uiSprite = edit->m_GameCore->GetECSManager()->GetComponent<UISpriteComponent>(object.GetEntity());
	if (uiSprite)
	{
		// UISprite統合バッファからmapIDを返却
		edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::UISprite)->RemoveMapID(uiSprite->mapID.value());
		// UISpriteのUseListから削除
		edit->m_ResourceManager->GetUIContainer()->RemoveUI(uiSprite->mapID.value());
	}
	// LightComponentを取得
	LightComponent* light = edit->m_GameCore->GetECSManager()->GetComponent<LightComponent>(object.GetEntity());
	if (light)
	{
		// Light統合バッファからmapIDを返却
		edit->m_ResourceManager->RecycleLightIndex(light->mapID.value());
	}
	// AudioComponentを取得
	AudioComponent* audio = edit->m_GameCore->GetECSManager()->GetComponent<AudioComponent>(object.GetEntity());
	if (audio)
	{
		// 再生中なら止める
		if (audio->isPlay)
		{
			edit->m_ResourceManager->GetAudioManager()->SoundStop(audio->audioID.value());
		}
	}
	// AnimationComponentを取得
	AnimationComponent* animation = edit->m_GameCore->GetECSManager()->GetComponent<AnimationComponent>(object.GetEntity());
	if (animation)
	{
		
	}
	// 削除前にComponentを記録する
	m_Transform = *transform;
	if (camera) { m_Camera = *camera; }
	if (meshFilter) { m_MeshFilter = *meshFilter; }
	MeshRendererComponent* meshRenderer = edit->m_GameCore->GetECSManager()->GetComponent<MeshRendererComponent>(object.GetEntity());
	if (meshRenderer) { m_MeshRenderer = *meshRenderer; }
	if (material) { m_Material = *material; }
	ScriptComponent* script = edit->m_GameCore->GetECSManager()->GetComponent<ScriptComponent>(object.GetEntity());
	if (script) { m_Script = *script; }
	if (lineRenderer) { m_LineRenderer = *lineRenderer; }
	Rigidbody2DComponent* rb = edit->m_GameCore->GetECSManager()->GetComponent<Rigidbody2DComponent>(object.GetEntity());
	if (rb) { m_Rigidbody2D = *rb; }
	BoxCollider2DComponent* box = edit->m_GameCore->GetECSManager()->GetComponent<BoxCollider2DComponent>(object.GetEntity());
	if (box) { m_BoxCollider2D = *box; }
	EmitterComponent* emitter = edit->m_GameCore->GetECSManager()->GetComponent<EmitterComponent>(object.GetEntity());
	if (emitter) { m_Emitter = *emitter; }
	ParticleComponent* particle = edit->m_GameCore->GetECSManager()->GetComponent<ParticleComponent>(object.GetEntity());
	if (particle) { m_Particle = *particle; }
	if (uiSprite) { m_UISprite = *uiSprite; }
	if (light) { m_Light = *light; }
	if (audio) { m_Audio = *audio; }
	if (animation) { m_Animation = *animation; }
	// Componentの初期化
	if (transform) { transform->Initialize(); }
	if (camera) { camera->Initialize(); }
	if (meshFilter) { meshFilter->Initialize(); }
	if (meshRenderer) { meshRenderer->Initialize(); }
	if (material) { material->Initialize(); }
	if (script) { script->Initialize(); }
	if (lineRenderer)
	{
		for (auto& line : *lineRenderer)
		{
			line.Initialize();
		}
	}
	if (rb) { rb->Initialize(); }
	if (box) { box->Initialize(); }
	if (emitter) { emitter->Initialize(); }
	if (particle) { particle->Initialize(); }
	if (uiSprite) { uiSprite->Initialize(); }
	if (light) { light->Initialize(); }
	if (audio) { audio->Initialize(); }
	//if (animation) { animation->Initialize(); }
	// Componentの削除
	edit->m_GameCore->GetECSManager()->RemoveComponent<TransformComponent>(object.GetEntity());
	if (camera) { edit->m_GameCore->GetECSManager()->RemoveComponent<CameraComponent>(object.GetEntity()); }
	if (meshFilter) { edit->m_GameCore->GetECSManager()->RemoveComponent<MeshFilterComponent>(object.GetEntity()); }
	if (meshRenderer) { edit->m_GameCore->GetECSManager()->RemoveComponent<MeshRendererComponent>(object.GetEntity()); }
	if (material) { edit->m_GameCore->GetECSManager()->RemoveComponent<MaterialComponent>(object.GetEntity()); }
	if (script) { edit->m_GameCore->GetECSManager()->RemoveComponent<ScriptComponent>(object.GetEntity()); }
	if (lineRenderer) { edit->m_GameCore->GetECSManager()->RemoveAllComponents<LineRendererComponent>(object.GetEntity()); }
	if (rb) { edit->m_GameCore->GetECSManager()->RemoveComponent<Rigidbody2DComponent>(object.GetEntity()); }
	if (box) { edit->m_GameCore->GetECSManager()->RemoveComponent<BoxCollider2DComponent>(object.GetEntity()); }
	if (emitter) { edit->m_GameCore->GetECSManager()->RemoveComponent<EmitterComponent>(object.GetEntity()); }
	if (particle) { edit->m_GameCore->GetECSManager()->RemoveComponent<ParticleComponent>(object.GetEntity()); }
	if (uiSprite) { edit->m_GameCore->GetECSManager()->RemoveComponent<UISpriteComponent>(object.GetEntity()); }
	if (light) { edit->m_GameCore->GetECSManager()->RemoveComponent<LightComponent>(object.GetEntity()); }
	if (audio) { edit->m_GameCore->GetECSManager()->RemoveComponent<AudioComponent>(object.GetEntity()); }
	if (animation) { edit->m_GameCore->GetECSManager()->RemoveComponent<AnimationComponent>(object.GetEntity()); }
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

bool AddMaterialComponent::Execute(EngineCommand* edit)
{
	// MaterialComponentを追加
	MaterialComponent* material = edit->m_GameCore->GetECSManager()->AddComponent<MaterialComponent>(m_Entity);
	if (!material) { return false; }
	// 初期値
	Color color;
	material->color = color.From255(200, 200, 200);
	material->enableLighting = true;
	material->matUV = Matrix4::Identity();
	material->shininess = 50.0f;
	// Material統合バッファからmapIDを取得
	uint32_t mapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Material)->GetMapID();
	TransformComponent* transform = edit->m_GameCore->GetECSManager()->GetComponent<TransformComponent>(m_Entity);
	transform->materialID = mapID;
	material->mapID = mapID;
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
	if (!edit->m_GameCore->GetSceneManager()->GetCurrentScene())
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
	uint32_t mapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
	// TransformComponentを追加
	TransformComponent* transform = edit->m_GameCore->GetECSManager()->AddComponent<TransformComponent>(entity);
	transform->mapID = mapID;
	// GameObjectを追加
	ObjectID objectID = edit->m_GameCore->GetObjectContainer()->AddGameObject(entity, name, ObjectType::ParticleSystem);
	m_ObjectID = objectID;
	// シーンに追加
	edit->m_GameCore->GetSceneManager()->GetCurrentScene()->AddUseObject(objectID);
	// SelectedObjectを設定
	edit->SetSelectedObject(&edit->m_GameCore->GetObjectContainer()->GetGameObject(m_ObjectID));
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
	EmitterComponent* emitter = edit->m_GameCore->GetECSManager()->AddComponent<EmitterComponent>(m_Entity);
	if (!emitter) { return false; }
	emitter->bufferIndex = edit->m_ResourceManager->CreateStructuredBuffer<BUFFER_DATA_EMITTER>(1);
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
	ParticleComponent* particle = edit->m_GameCore->GetECSManager()->AddComponent<ParticleComponent>(m_Entity);
	if (!particle) { return false; }
	// Resourceの生成
	// パーティクル
	particle->bufferIndex = edit->m_ResourceManager->CreateRWStructuredBuffer<BUFFER_DATA_PARTICLE>(particle->count);
	// PerFrame
	particle->perFrameBufferIndex = edit->m_ResourceManager->CreateConstantBuffer<BUFFER_DATA_PARTICLE_PERFRAME>();
	// FreeListIndex
	//particle->freeListIndexBufferIndex = edit->m_ResourceManager->CreateRWStructuredBuffer<int32_t>(1);
	// FreeList
	particle->freeListBufferIndex = edit->m_ResourceManager->CreateRWStructuredBuffer<uint32_t>(particle->count,true);
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
	if (!edit->m_GameCore->GetSceneManager()->GetCurrentScene())
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
	EffectComponent* effect = edit->m_GameCore->GetECSManager()->AddComponent<EffectComponent>(entity);
	if (!effect) { return false; }
	effect->isRun = false;
	effect->isLoop = true;
	edit->m_EffectEntity = entity;
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
	if (!edit->m_GameCore->GetSceneManager()->GetCurrentScene())
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
	uint32_t tfMapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
	// TransformComponentを追加
	TransformComponent* transform = edit->m_GameCore->GetECSManager()->AddComponent<TransformComponent>(entity);
	transform->mapID = tfMapID;
	// UISprite統合バッファからmapIDを取得
	uint32_t mapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::UISprite)->GetMapID();
	// SpriteComponentを追加
	UISpriteComponent* uiSprite = edit->m_GameCore->GetECSManager()->AddComponent<UISpriteComponent>(entity);
	uiSprite->mapID = mapID;
	// UseListに登録
	edit->m_ResourceManager->GetUIContainer()->AddUI(mapID);
	// GameObjectを追加
	ObjectID objectID = edit->m_GameCore->GetObjectContainer()->AddGameObject(entity, name, ObjectType::UI);
	m_ObjectID = objectID;
	// シーンに追加
	edit->m_GameCore->GetSceneManager()->GetCurrentScene()->AddUseObject(objectID);
	// SelectedObjectを設定
	edit->SetSelectedObject(&edit->m_GameCore->GetObjectContainer()->GetGameObject(m_ObjectID));
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
	if (!edit->m_GameCore->GetSceneManager()->GetCurrentScene())
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
	uint32_t tfMapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
	// TransformComponentを追加
	TransformComponent* transform = edit->m_GameCore->GetECSManager()->AddComponent<TransformComponent>(entity);
	transform->mapID = tfMapID;
	// LightバッファからmapIDを取得
	uint32_t mapID = edit->m_ResourceManager->GetLightIndex();
	// LightComponentを追加
	LightComponent* light = edit->m_GameCore->GetECSManager()->AddComponent<LightComponent>(entity);
	light->mapID = mapID;
	// GameObjectを追加
	ObjectID objectID = edit->m_GameCore->GetObjectContainer()->AddGameObject(entity, name, ObjectType::Light);
	m_ObjectID = objectID;
	// シーンに追加
	edit->m_GameCore->GetSceneManager()->GetCurrentScene()->AddUseObject(objectID);
	// SelectedObjectを設定
	edit->SetSelectedObject(&edit->m_GameCore->GetObjectContainer()->GetGameObject(m_ObjectID));
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
	if (model->isBone)
	{
		animation->skeleton = model->skeleton;
		animation->skinCluster = model->skinCluster;
		// Resourceの生成
		//animation->paletteBufferIndex = edit->m_ResourceManager->CreateStructuredBuffer<ConstBufferDataWell>(static_cast<UINT>(animation->skeleton->joints.size()));
		//StructuredBuffer<ConstBufferDataWell>* paletteBuffer = dynamic_cast<StructuredBuffer<ConstBufferDataWell>*>(edit->m_ResourceManager->GetBuffer<IStructuredBuffer>(animation->paletteBufferIndex));
		/*animation->influenceBufferIndex = edit->m_ResourceManager->CreateStructuredBuffer<ConstBufferDataVertexInfluence>(static_cast<UINT>(model->meshes[0].vertices.size()));
		StructuredBuffer<ConstBufferDataVertexInfluence>* influenceBuffer = dynamic_cast<StructuredBuffer<ConstBufferDataVertexInfluence>*>(edit->m_ResourceManager->GetBuffer<IStructuredBuffer>(animation->influenceBufferIndex));
		std::span<ConstBufferDataVertexInfluence> influenceSpan = influenceBuffer->GetMappedData();
		std::memset(influenceSpan.data(), 0, sizeof(ConstBufferDataVertexInfluence) * influenceSpan.size());*/
		// OutputVertexBufferを生成
		//animation->skinningBufferIndex = edit->m_ResourceManager->CreateVertexBuffer<VertexData>(static_cast<UINT>(model->meshes[0].vertices.size()),true);
		animation->modelName = model->name;
		animation->boneOffsetID = model->nextBoneOffsetIndex;
		model->nextBoneOffsetIndex++;
	}
	
	return true;
}

bool AddAnimationComponent::Undo(EngineCommand* edit)
{
	edit;
	return false;
}

bool CopyGameObjectCommand::Execute(EngineCommand* edit)
{
	//ObjectContainer* objectContainer = edit->m_GameCore->GetObjectContainer();
	ECSManager* ecs = edit->m_GameCore->GetECSManager();
	//SceneManager* sceneManager = edit->m_GameCore->GetSceneManager();
	// GameObjectを取得
	GameObject& object = edit->m_GameCore->GetObjectContainer()->GetGameObject(m_ObjectID);
	if (!object.IsActive()) { return false; }
	GameObjectData objData = object;
	// オブジェクト名とタイプを取得
	std::wstring name = objData.m_Name;
	name = GenerateUniqueName(name, edit->m_GameCore->GetObjectContainer()->GetNameToObjectID());
	ObjectType type = objData.m_Type;
	// Entityを生成
	Entity entity = edit->m_GameCore->GetECSManager()->GenerateEntity();
	// GameObjectを追加
	ObjectID objectID = edit->m_GameCore->GetObjectContainer()->AddGameObject(entity, name, type);
	GameObject& newObj = edit->m_GameCore->GetObjectContainer()->GetGameObjectByName(name);
	// SceneUseListに登録
	edit->m_GameCore->GetSceneManager()->GetCurrentScene()->AddUseObject(newObj.GetID().value());
	// コンポーネントの追加
	// TransformComponentの追加
	if (objData.m_Transform.has_value())
	{
		TransformComponent* transform = ecs->AddComponent<TransformComponent>(entity);
		*transform = objData.m_Transform.value();
		transform->mapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
	}
	// CameraComponentの追加
	if (objData.m_Camera.has_value())
	{
		CameraComponent* camera = ecs->AddComponent<CameraComponent>(entity);
		*camera = objData.m_Camera.value();
		camera->bufferIndex = edit->m_ResourceManager->CreateConstantBuffer<BUFFER_DATA_VIEWPROJECTION>();
	}
	// MeshFilterComponentの追加
	if (objData.m_MeshFilter.has_value())
	{
		MeshFilterComponent* mesh = ecs->AddComponent<MeshFilterComponent>(entity);
		*mesh = objData.m_MeshFilter.value();
		mesh->modelID = edit->m_ResourceManager->GetModelManager()->GetModelDataIndex(mesh->modelName);
		// モデルのUseListに登録
		TransformComponent* transform = ecs->GetComponent<TransformComponent>(entity);
		edit->m_ResourceManager->GetModelManager()->RegisterModelUseList(mesh->modelID.value(), transform->mapID.value());

	}
	// MeshRendererComponentの追加
	if (objData.m_MeshRenderer.has_value())
	{
		MeshRendererComponent* renderer = ecs->AddComponent<MeshRendererComponent>(entity);
		*renderer = objData.m_MeshRenderer.value();
	}
	// MaterialComponentの追加
	if (objData.m_Material.has_value())
	{
		MaterialComponent* material = ecs->AddComponent<MaterialComponent>(entity);
		*material = objData.m_Material.value();
		material->mapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Material)->GetMapID();
		TransformComponent* transform = ecs->GetComponent<TransformComponent>(entity);
		transform->materialID = material->mapID;
	}
	// ScriptComponentの追加
	if (objData.m_Script.has_value())
	{
		ScriptComponent* script = ecs->AddComponent<ScriptComponent>(entity);
		*script = objData.m_Script.value();
		script->objectID = objectID;
	}
	// LineRendererComponentの追加
	for (const auto& line : objData.m_LineRenderer)
	{
		LineRendererComponent* lineRenderer = ecs->AddComponent<LineRendererComponent>(entity);
		*lineRenderer = line;
		lineRenderer->mapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::Line)->GetMapID();
	}
	// Rigidbody2DComponentの追加
	if (objData.m_Rigidbody2D.has_value())
	{
		Rigidbody2DComponent* rb = ecs->AddComponent<Rigidbody2DComponent>(entity);
		*rb = objData.m_Rigidbody2D.value();
		rb->selfObjectID = objectID;
	}
	// BoxCollider2DComponentの追加
	if (objData.m_BoxCollider2D.has_value())
	{
		BoxCollider2DComponent* box = ecs->AddComponent<BoxCollider2DComponent>(entity);
		*box = objData.m_BoxCollider2D.value();
	}
	// EmitterComponentの追加
	if (objData.m_Emitter.has_value())
	{
		EmitterComponent* emitter = ecs->AddComponent<EmitterComponent>(entity);
		*emitter = objData.m_Emitter.value();
		emitter->bufferIndex = edit->m_ResourceManager->CreateConstantBuffer<BUFFER_DATA_EMITTER>();
	}
	// ParticleComponentの追加
	if (objData.m_Particle.has_value())
	{
		ParticleComponent* particle = ecs->AddComponent<ParticleComponent>(entity);
		*particle = objData.m_Particle.value();
		// パーティクル
		particle->bufferIndex = edit->m_ResourceManager->CreateRWStructuredBuffer<BUFFER_DATA_PARTICLE>(particle->count);
		// PerFrame
		particle->perFrameBufferIndex = edit->m_ResourceManager->CreateConstantBuffer<BUFFER_DATA_PARTICLE_PERFRAME>();
		// FreeListIndex
		particle->freeListIndexBufferIndex = edit->m_ResourceManager->CreateRWStructuredBuffer<int32_t>(1);
		// FreeList
		particle->freeListBufferIndex = edit->m_ResourceManager->CreateRWStructuredBuffer<uint32_t>(particle->count, true);
	}
	// UISpriteComponentの追加
	if (objData.m_UISprite.has_value())
	{
		UISpriteComponent* uiSprite = ecs->AddComponent<UISpriteComponent>(entity);
		*uiSprite = objData.m_UISprite.value();
		uiSprite->mapID = edit->m_ResourceManager->GetIntegrationData(IntegrationDataType::UISprite)->GetMapID();
		edit->m_ResourceManager->GetUIContainer()->AddUI(uiSprite->mapID.value());
	}
	// LightComponentの追加
	if (objData.m_Light.has_value())
	{
		LightComponent* light = ecs->AddComponent<LightComponent>(entity);
		*light = objData.m_Light.value();
		light->mapID = edit->m_ResourceManager->GetLightIndex();
	}
	// AudioComponentの追加
	if (objData.m_Audio.has_value())
	{
		AudioComponent* audio = ecs->AddComponent<AudioComponent>(entity);
		*audio = objData.m_Audio.value();
		if (!audio->audioName.empty())
		{
			audio->audioID = edit->m_ResourceManager->GetAudioManager()->GetSoundDataIndex(audio->audioName);
		}
	}
	// AnimationComponentの追加
	if (objData.m_Animation.has_value())
	{
		AnimationComponent* animation = ecs->AddComponent<AnimationComponent>(entity);
		*animation = objData.m_Animation.value();
		if (!animation->modelName.empty())
		{
			ModelData* model = edit->m_ResourceManager->GetModelManager()->GetModelData(animation->modelName);
			if (model)
			{
				animation->boneOffsetID = model->nextBoneOffsetIndex;
				model->nextBoneOffsetIndex++;
				if (model->isBone)
				{
					animation->skeleton = model->skeleton;
					animation->skinCluster = model->skinCluster;
				}
			}
		}
	}
	return true;
}

bool CopyGameObjectCommand::Undo(EngineCommand* edit)
{
	edit;
	return false;
}