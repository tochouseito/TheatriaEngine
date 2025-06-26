#include "pch.h"
#include "GameCore.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Platform/FileSystem/FileSystem.h"
#include "GameCore/Systems/SingleSystems.h"
#include "GameCore/Systems/MultiSystems.h"
#include "GameCore/Systems/EditorSystems.h"
#include "EngineCommand/EngineCommands.h"

void GameCore::Initialize(InputManager* input, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine)
{
	// シーンマネージャーの生成
	m_pSceneManager = std::make_unique<SceneManager>(this,resourceManager);
	// ECSマネージャの生成
	m_pECSManager = std::make_unique<ECSManager>();
	// ECSイベントの登録
	RegisterECSEvents();
	// オブジェクトコンテナの生成
	m_pObjectContainer = std::make_unique<ObjectContainer>(m_pECSManager.get(), resourceManager, input);
	// Systemマネージャの生成
	m_pSingleSystemManager = std::make_unique<SingleSystemManager>();
	m_pMultiSystemManager = std::make_unique<MultiSystemManager>();
	// エディタのシステムマネージャの生成
	m_pEditorSingleSystem = std::make_unique<SingleSystemManager>();
	m_pEditorMultiSystem = std::make_unique<MultiSystemManager>();
	// box2dの生成
	//b2Vec2 gravity(0.0f, -9.8f);
	b2Vec2 gravity(0.0f, 0.0f);
	m_pPhysicsWorld = std::make_unique<b2World>(gravity);
	m_pContactListener = std::make_unique<ContactListener2D>(m_pECSManager.get(), resourceManager, input, m_pObjectContainer.get());
	m_pPhysicsWorld->SetContactListener(m_pContactListener.get());
	// システムの生成
	input;resourceManager;graphicsEngine;
	m_EnvironmentData.ambientColor = { 0.01f,0.01f,0.01f,1.0f };
}

void GameCore::Start(ResourceManager& resourceManager)
{
	Log::Write(LogLevel::Info, "GameCore Start");
	m_pSingleSystemManager->StartAll(m_pECSManager.get());
	m_pMultiSystemManager->StartAll(m_pECSManager.get());
	Log::Write(LogLevel::Info, "GameCore Start End");
	resourceManager;
}

void GameCore::Update(ResourceManager& resourceManager, GraphicsEngine& graphicsEngine)
{
	//m_pSceneManager->Update();
	// 環境設定の更新
	UpdateEnvironmentSetting();
	// ゲームが実行中でなければreturn
	if (isRunning)
	{
		Log::Write(LogLevel::Info, "GameCore Update");
		InitializeGenerateObject();
		m_pSingleSystemManager->UpdateAll(m_pECSManager.get());
		m_pMultiSystemManager->UpdateAll(m_pECSManager.get());
		Log::Write(LogLevel::Info, "GameCore Update End");
	} else
	{
		m_pEditorSingleSystem->UpdateAll(m_pECSManager.get());
		m_pEditorMultiSystem->UpdateAll(m_pECSManager.get());
	}
	resourceManager;
	graphicsEngine;
}

void GameCore::SceneUpdate()
{
	m_pSceneManager->Update();
}

void GameCore::GameRun()
{
	if (isRunning)
	{
		return;
	}
	// ゲームの初期化処理
	// スクリプトDLLの読み込み
	/*if (!FileSystem::ScriptProject::BuildScriptDLL())
	{
		return;
	}*/
	Cho::FileSystem::ScriptProject::LoadScriptDLL();
	isRunning = true;
	// 
	m_MainSceneID = m_pSceneManager->GetMainScene()->GetSceneID();
	// StartSystemの実行
	m_pObjectContainer->InitializeAllGameObjects();
	//m_pSingleSystemManager->StartAll(m_pECSManager.get());
	//m_pMultiSystemManager->StartAll(m_pECSManager.get());
}

void GameCore::GameStop()
{
	if (!isRunning)
	{
		return;
	}
	// スクリプトのインスタンスを解放
	m_pSingleSystemManager->EndAll(m_pECSManager.get());
	m_pMultiSystemManager->EndAll(m_pECSManager.get());
	// 生成されたオブジェクトを削除
	ClearGenerateObject();
	m_GameGenerateID.clear();
	// SceneManager
	m_pSceneManager->EditorReLoad(m_MainSceneID);
	m_pObjectContainer->InitializeAllGameObjects();
	// DLLのアンロード
	Cho::FileSystem::ScriptProject::UnloadScriptDLL();
	isRunning = false;
}

void GameCore::InitializeGenerateObject()
{
	for (const ObjectID& id : m_GameGenerateID)
	{
		GameObject* object = m_pObjectContainer->GetGameObject(id);
		if (!object->IsActive()) { continue; }
		// オブジェクトの初期化
		Entity entity = object->GetEntity();
		// TransformComponentを取得
		TransformComponent* transform = m_pECSManager->GetComponent<TransformComponent>(entity);
		if (!transform) { continue; }
		// TransformComponentの初期化
		tfOnceSystem->Start(*transform);
		// スクリプトの取得
		ScriptComponent* script = m_pECSManager->GetComponent<ScriptComponent>(entity);
		if (script)
		{
			// スクリプトの初期化
			scriptGenerateOnceSystem->InstanceGenerate(*script);
			scriptInitializeOnceSystem->StartScript(*script);
		}
		// Rigidbody2DComponentの取得
		Rigidbody2DComponent* rb = m_pECSManager->GetComponent<Rigidbody2DComponent>(entity);
		if (rb)
		{
			// Rigidbody2DComponentの初期化
			physicsOnceSystem->CreateBody(entity, *transform, *rb);
		}
		// BoxCollider2DComponentの取得
		BoxCollider2DComponent* box = m_pECSManager->GetComponent<BoxCollider2DComponent>(entity);
		if (box)
		{
			// BoxCollider2DComponentの初期化
			boxInitOnceSystem->CreateFixture(*transform, *rb, *box);
		}
		ParticleComponent* particle = m_pECSManager->GetComponent<ParticleComponent>(entity);
		if (particle)
		{
			// ParticleComponentの初期化
			particleInitializeOnceSystem->InitializeParticle(*particle);
		}
		// 初期化済みのIDを追加
		m_GameInitializedID.push_back(id);
	}
	// 初期化済みのIDをクリア
	m_GameGenerateID.clear();
	
	for(const auto& sceneID : m_GameLoadSceneID)
	{
		for(const ObjectID& id : m_pSceneManager->GetScene(sceneID)->GetUseObjects())
		{
			GameObject* object = m_pObjectContainer->GetGameObject(id);
			if (!object->IsActive()) { continue; }
			// オブジェクトの初期化
			Entity entity = object->GetEntity();
			// TransformComponentを取得
			TransformComponent* transform = m_pECSManager->GetComponent<TransformComponent>(entity);
			if (!transform) { continue; }
			// TransformComponentの初期化
			tfOnceSystem->Start(*transform);
			// スクリプトの取得
			ScriptComponent* script = m_pECSManager->GetComponent<ScriptComponent>(entity);
			if (script)
			{
				// スクリプトの初期化
				scriptGenerateOnceSystem->InstanceGenerate(*script);
				scriptInitializeOnceSystem->StartScript(*script);
			}
			// Rigidbody2DComponentの取得
			Rigidbody2DComponent* rb = m_pECSManager->GetComponent<Rigidbody2DComponent>(entity);
			if (rb)
			{
				// Rigidbody2DComponentの初期化
				physicsOnceSystem->CreateBody(entity, *transform, *rb);
			}
			// BoxCollider2DComponentの取得
			BoxCollider2DComponent* box = m_pECSManager->GetComponent<BoxCollider2DComponent>(entity);
			if (box)
			{
				// BoxCollider2DComponentの初期化
				boxInitOnceSystem->CreateFixture(*transform, *rb, *box);
			}
			ParticleComponent* particle = m_pECSManager->GetComponent<ParticleComponent>(entity);
			if (particle)
			{
				// ParticleComponentの初期化
				particleInitializeOnceSystem->InitializeParticle(*particle);
			}
		}
	}
	m_GameLoadSceneID.clear();
}

void GameCore::ClearGenerateObject()
{
	for (const ObjectID& id : m_GameInitializedID)
	{
		GameObject* object = m_pObjectContainer->GetGameObject(id);
		std::unique_ptr<DeleteObjectCommand> command = std::make_unique<DeleteObjectCommand>(object->GetID().value());
		command->Execute(m_EngineCommand);
	}
	m_GameInitializedID.clear();
}

void GameCore::UpdateEnvironmentSetting()
{
	// 環境情報バッファ
	ConstantBuffer<BUFFER_DATA_ENVIRONMENT>* envBuffer = m_EngineCommand->GetResourceManager()->GetEnvironmentBuffer();
	envBuffer->UpdateData(m_EnvironmentData);
}

void GameCore::SceneInitialize(ScenePrefab* scene)
{
	for (ObjectID& id : scene->GetUseObjects())
	{
		GameObject* object = m_pObjectContainer->GetGameObject(id);
		if (!object->IsActive()) { continue; }
		// オブジェクトの初期化
		Entity entity = object->GetEntity();
		// TransformComponentを取得
		TransformComponent* transform = m_pECSManager->GetComponent<TransformComponent>(entity);
		if (!transform) { continue; }
		// TransformComponentの初期化
		tfFinalizeOnceSystem->Finalize(entity, *transform);
		// スクリプトの取得
		ScriptComponent* script = m_pECSManager->GetComponent<ScriptComponent>(entity);
		if (script)
		{
			// スクリプトの初期化
			scriptFinalizeOnceSystem->FinalizeScript(*script);
		}
		// Rigidbody2DComponentの取得
		Rigidbody2DComponent* rb = m_pECSManager->GetComponent<Rigidbody2DComponent>(entity);
		if (rb)
		{
			// Rigidbody2DComponentの初期化
			physicsResetOnceSystem->Reset(*transform, *rb);
		}
	}
}

void GameCore::SceneFinelize(ScenePrefab* scene)
{
	if (!isRunning) { return; }
	for (ObjectID& id : scene->GetUseObjects())
	{
		GameObject* object = m_pObjectContainer->GetGameObject(id);
		if (!object->IsActive()) { continue; }
		// オブジェクトの初期化
		Entity entity = object->GetEntity();
		// TransformComponentを取得
		TransformComponent* transform = m_pECSManager->GetComponent<TransformComponent>(entity);
		if (!transform) { continue; }
		// TransformComponentの初期化
		tfFinalizeOnceSystem->Finalize(entity,*transform);
		// スクリプトの取得
		ScriptComponent* script = m_pECSManager->GetComponent<ScriptComponent>(entity);
		if (script)
		{
			// スクリプトの初期化
			scriptFinalizeOnceSystem->FinalizeScript(*script);
		}
		// Rigidbody2DComponentの取得
		Rigidbody2DComponent* rb = m_pECSManager->GetComponent<Rigidbody2DComponent>(entity);
		if (rb)
		{
			// Rigidbody2DComponentの初期化
			physicsResetOnceSystem->Reset(*transform, *rb);
		}
	}
}

void GameCore::RegisterECSEvents()
{
	m_pComponentEventDispatcher = std::make_unique<ComponentEventDispatcher>(m_EngineCommand);
	m_pComponentEventDispatcher->SetECSManager(m_pECSManager.get());
	m_pECSManager->AddListener(m_pComponentEventDispatcher.get());
	// イベントの登録
	// TransformComponent
	m_pComponentEventDispatcher->RegisterOnAdd<TransformComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] TransformComponent* c) {
			// Transform統合バッファからmapIDを取得
			uint32_t mapID = m_EngineCommand->GetResourceManager()->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
			c->mapID = mapID;
		});
	m_pComponentEventDispatcher->RegisterOnCopy<TransformComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] TransformComponent* c) {
			// Transform統合バッファからmapIDを取得
			TransformComponent& srcTransform = *m_pECSManager->GetComponent<TransformComponent>(src);
			*c = srcTransform;
			uint32_t mapID = m_EngineCommand->GetResourceManager()->GetIntegrationData(IntegrationDataType::Transform)->GetMapID();
			c->mapID = mapID;
		});
	m_pComponentEventDispatcher->RegisterOnRemove<TransformComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] TransformComponent* c) {
			// Transform統合バッファからmapIDを削除
			m_EngineCommand->GetResourceManager()->GetIntegrationData(IntegrationDataType::Transform)->RemoveMapID(c->mapID.value());
		});
	// CameraComponent
	m_pComponentEventDispatcher->RegisterOnAdd<CameraComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] CameraComponent* c) {
			// Bufferの生成
			c->bufferIndex = m_EngineCommand->GetResourceManager()->CreateConstantBuffer<BUFFER_DATA_VIEWPROJECTION>();
		});
	m_pComponentEventDispatcher->RegisterOnCopy<CameraComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] CameraComponent* c) {
			CameraComponent& srcCamera = *m_pECSManager->GetComponent<CameraComponent>(src);
			*c = srcCamera;
			// Bufferの生成
			c->bufferIndex = m_EngineCommand->GetResourceManager()->CreateConstantBuffer<BUFFER_DATA_VIEWPROJECTION>();
		});
	m_pComponentEventDispatcher->RegisterOnRemove<CameraComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] CameraComponent* c) {
			// シーンのMainCameraを削除
			if (m_EngineCommand->GetGameCore()->GetSceneManager()->GetMainScene()->GetMainCameraID() == e)
			{
				m_EngineCommand->GetGameCore()->GetSceneManager()->GetMainScene()->SetMainCameraID(std::nullopt);
			}
			// Bufferの削除
			//m_EngineCommand->GetResourceManager()->DeleteConstantBuffer(c->bufferIndex.value());
		});
	// MeshFilterComponent
	m_pComponentEventDispatcher->RegisterOnAdd<MeshFilterComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] MeshFilterComponent* c) {
			// デフォルトのモデルとしてCubeを設定
			c->modelName = L"Cube";
			// モデルのIDを取得
			c->modelID = m_EngineCommand->GetResourceManager()->GetModelManager()->GetModelDataIndex(c->modelName);
			// モデルのUseListに登録
			TransformComponent* transform = m_pECSManager->GetComponent<TransformComponent>(e);
			m_EngineCommand->GetResourceManager()->GetModelManager()->RegisterModelUseList(c->modelID.value(), transform->mapID.value());
		});
	m_pComponentEventDispatcher->RegisterOnCopy<MeshFilterComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] MeshFilterComponent* c) {
			MeshFilterComponent& srcMeshFilter = *m_pECSManager->GetComponent<MeshFilterComponent>(src);
			*c = srcMeshFilter;
			// モデルのUseListに登録
			TransformComponent* transform = m_pECSManager->GetComponent<TransformComponent>(dst);
			m_EngineCommand->GetResourceManager()->GetModelManager()->RegisterModelUseList(c->modelID.value(), transform->mapID.value());
		});
	m_pComponentEventDispatcher->RegisterOnRemove<MeshFilterComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] MeshFilterComponent* c) {
			// モデルのUseListから削除
			TransformComponent* transform = m_pECSManager->GetComponent<TransformComponent>(e);
			m_EngineCommand->GetResourceManager()->GetModelManager()->RemoveModelUseList(c->modelID.value(),transform->mapID.value());
		});
	// MeshRendererComponent
	m_pComponentEventDispatcher->RegisterOnAdd<MeshRendererComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] MeshRendererComponent* c) {
			// 可視化
			c->visible = true;
		});
	m_pComponentEventDispatcher->RegisterOnCopy<MeshRendererComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] MeshRendererComponent* c) {
			MeshRendererComponent& srcMeshRenderer = *m_pECSManager->GetComponent<MeshRendererComponent>(src);
			*c = srcMeshRenderer;
			// 可視化
			c->visible = true;
		});
	m_pComponentEventDispatcher->RegisterOnRemove<MeshRendererComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] MeshRendererComponent* c) {
			// 可視化
			c->visible = false;
		});
	// ScriptComponent
	m_pComponentEventDispatcher->RegisterOnAdd<ScriptComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] ScriptComponent* c) {
			c->objectID = e;
		});
	m_pComponentEventDispatcher->RegisterOnCopy<ScriptComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] ScriptComponent* c) {
			ScriptComponent& srcScript = *m_pECSManager->GetComponent<ScriptComponent>(src);
			*c = srcScript;
			c->objectID = dst;
		});
	m_pComponentEventDispatcher->RegisterOnRemove<ScriptComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] ScriptComponent* c) {
			// スクリプトのインスタンスを削除
			//m_pObjectContainer->DeleteScriptInstance(c->objectID);
		});
	// MaterialComponent
	m_pComponentEventDispatcher->RegisterOnAdd<MaterialComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] MaterialComponent* c) {
			Color color;
			c->color = color.From255(200, 200, 200);
			c->enableLighting = true;
			c->matUV = Matrix4::Identity();
			c->shininess = 50.0f;
			uint32_t mapID = m_EngineCommand->GetResourceManager()->GetIntegrationData(IntegrationDataType::Material)->GetMapID();
			c->mapID = mapID;
			TransformComponent* transform = m_pECSManager->GetComponent<TransformComponent>(e);
			transform->materialID = mapID;
		});
	m_pComponentEventDispatcher->RegisterOnCopy<MaterialComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] MaterialComponent* c) {
			MaterialComponent& srcMaterial = *m_pECSManager->GetComponent<MaterialComponent>(src);
			*c = srcMaterial;
			uint32_t mapID = m_EngineCommand->GetResourceManager()->GetIntegrationData(IntegrationDataType::Material)->GetMapID();
			c->mapID = mapID;
			TransformComponent* transform = m_pECSManager->GetComponent<TransformComponent>(dst);
			transform->materialID = mapID;
		});
	m_pComponentEventDispatcher->RegisterOnRemove<MaterialComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] MaterialComponent* c) {
			// Material統合バッファからmapIDを削除
			m_EngineCommand->GetResourceManager()->GetIntegrationData(IntegrationDataType::Material)->RemoveMapID(c->mapID.value());
		});
	// ParticleComponent
	m_pComponentEventDispatcher->RegisterOnAdd<ParticleComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] ParticleComponent* c) {
			c->bufferIndex = m_EngineCommand->GetResourceManager()->CreateRWStructuredBuffer<BUFFER_DATA_PARTICLE>(c->count);
			c->perFrameBufferIndex = m_EngineCommand->GetResourceManager()->CreateConstantBuffer<BUFFER_DATA_PARTICLE_PERFRAME>();
			c->freeListBufferIndex = m_EngineCommand->GetResourceManager()->CreateRWStructuredBuffer<uint32_t>(c->count, true);
		});
	m_pComponentEventDispatcher->RegisterOnCopy<ParticleComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] ParticleComponent* c) {
			ParticleComponent& srcParticle = *m_pECSManager->GetComponent<ParticleComponent>(src);
			*c = srcParticle;
			c->bufferIndex = m_EngineCommand->GetResourceManager()->CreateRWStructuredBuffer<BUFFER_DATA_PARTICLE>(c->count);
			c->perFrameBufferIndex = m_EngineCommand->GetResourceManager()->CreateConstantBuffer<BUFFER_DATA_PARTICLE_PERFRAME>();
			c->freeListBufferIndex = m_EngineCommand->GetResourceManager()->CreateRWStructuredBuffer<uint32_t>(c->count, true);
		});
	m_pComponentEventDispatcher->RegisterOnRemove<ParticleComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] ParticleComponent* c) {
			//// Particle統合バッファからmapIDを削除
			//m_EngineCommand->GetResourceManager()->GetIntegrationData(IntegrationDataType::Particle)->RemoveMapID(c->bufferIndex.value());
			//// RWStructuredBufferの削除
			//m_EngineCommand->GetResourceManager()->DeleteRWStructuredBuffer(c->bufferIndex.value());
			//m_EngineCommand->GetResourceManager()->DeleteConstantBuffer(c->perFrameBufferIndex.value());
			//m_EngineCommand->GetResourceManager()->DeleteRWStructuredBuffer(c->freeListBufferIndex.value());
		});
	// EmitterComponent
	m_pComponentEventDispatcher->RegisterOnAdd<EmitterComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] EmitterComponent* c) {
			c->bufferIndex = m_EngineCommand->GetResourceManager()->CreateStructuredBuffer<BUFFER_DATA_EMITTER>(1);
		});
	m_pComponentEventDispatcher->RegisterOnCopy<EmitterComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] EmitterComponent* c) {
			EmitterComponent& srcEmitter = *m_pECSManager->GetComponent<EmitterComponent>(src);
			*c = srcEmitter;
			c->bufferIndex = m_EngineCommand->GetResourceManager()->CreateStructuredBuffer<BUFFER_DATA_EMITTER>(1);
		});
	m_pComponentEventDispatcher->RegisterOnRemove<EmitterComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] EmitterComponent* c) {
			// Emitter統合バッファからmapIDを削除
			//m_EngineCommand->GetResourceManager()->GetIntegrationData(IntegrationDataType::Emitter)->RemoveMapID(c->bufferIndex.value());
			// StructuredBufferの削除
			//m_EngineCommand->GetResourceManager()->DeleteStructuredBuffer(c->bufferIndex.value());
		});
	// LineRendererComponent
	m_pComponentEventDispatcher->RegisterOnAdd<LineRendererComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] LineRendererComponent* c, [[maybe_unused]] size_t idx) {
			uint32_t mapID = m_EngineCommand->GetResourceManager()->GetIntegrationData(IntegrationDataType::Line)->GetMapID();
			c->mapID = mapID;
		});
	m_pComponentEventDispatcher->RegisterOnCopy<LineRendererComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] LineRendererComponent* c, [[maybe_unused]] size_t idx) {
			LineRendererComponent& srcLine = *m_pECSManager->GetComponent<LineRendererComponent>(src);
			*c = srcLine;
			uint32_t mapID = m_EngineCommand->GetResourceManager()->GetIntegrationData(IntegrationDataType::Line)->GetMapID();
			c->mapID = mapID;
		});
	m_pComponentEventDispatcher->RegisterOnRemove<LineRendererComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] LineRendererComponent* c, [[maybe_unused]] size_t idx) {
			// Line統合バッファからmapIDを削除
			m_EngineCommand->GetResourceManager()->GetIntegrationData(IntegrationDataType::Line)->RemoveMapID(c->mapID.value());
		});
	// Rigidbody2DComponent
	m_pComponentEventDispatcher->RegisterOnAdd<Rigidbody2DComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] Rigidbody2DComponent* c) {
			c->selfObjectID = e;
		});
	m_pComponentEventDispatcher->RegisterOnCopy<Rigidbody2DComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] Rigidbody2DComponent* c) {
			Rigidbody2DComponent& srcRigidbody = *m_pECSManager->GetComponent<Rigidbody2DComponent>(src);
			*c = srcRigidbody;
			c->selfObjectID = dst;
		});
	m_pComponentEventDispatcher->RegisterOnRemove<Rigidbody2DComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] Rigidbody2DComponent* c) {
		});
	// BoxCollider2DComponent
	m_pComponentEventDispatcher->RegisterOnAdd<BoxCollider2DComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] BoxCollider2DComponent* c) {
		});
	m_pComponentEventDispatcher->RegisterOnCopy<BoxCollider2DComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] BoxCollider2DComponent* c) {
			BoxCollider2DComponent& srcBox = *m_pECSManager->GetComponent<BoxCollider2DComponent>(src);
			*c = srcBox;
		});
	m_pComponentEventDispatcher->RegisterOnRemove<BoxCollider2DComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] BoxCollider2DComponent* c) {
		});
	// EffectComponent
	m_pComponentEventDispatcher->RegisterOnAdd<EffectComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] EffectComponent* c) {
			c->isRun = false;
			c->isLoop = true;
		});
	m_pComponentEventDispatcher->RegisterOnCopy<EffectComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] EffectComponent* c) {
			EffectComponent& srcEffect = *m_pECSManager->GetComponent<EffectComponent>(src);
			*c = srcEffect;
			c->isRun = false;
			c->isLoop = true;
		});
	m_pComponentEventDispatcher->RegisterOnRemove<EffectComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] EffectComponent* c) {
			// Effect統合バッファからmapIDを削除
			//m_EngineCommand->GetResourceManager()->GetIntegrationData(IntegrationDataType::Effect)->RemoveMapID(c->mapID.value());
		});
	// UISpriteComponent
	m_pComponentEventDispatcher->RegisterOnAdd<UISpriteComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] UISpriteComponent* c) {
			uint32_t mapID = m_EngineCommand->GetResourceManager()->GetIntegrationData(IntegrationDataType::UISprite)->GetMapID();
			c->mapID = mapID;
			m_EngineCommand->GetResourceManager()->GetUIContainer()->AddUI(mapID);
		});
	m_pComponentEventDispatcher->RegisterOnCopy<UISpriteComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] UISpriteComponent* c) {
			UISpriteComponent& srcUISprite = *m_pECSManager->GetComponent<UISpriteComponent>(src);
			*c = srcUISprite;
			uint32_t mapID = m_EngineCommand->GetResourceManager()->GetIntegrationData(IntegrationDataType::UISprite)->GetMapID();
			c->mapID = mapID;
			m_EngineCommand->GetResourceManager()->GetUIContainer()->AddUI(mapID);
		});
	m_pComponentEventDispatcher->RegisterOnRemove<UISpriteComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] UISpriteComponent* c) {
			// UISprite統合バッファからmapIDを削除
			m_EngineCommand->GetResourceManager()->GetIntegrationData(IntegrationDataType::UISprite)->RemoveMapID(c->mapID.value());
			m_EngineCommand->GetResourceManager()->GetUIContainer()->RemoveUI(c->mapID.value());
		});
	// LightComponent
	m_pComponentEventDispatcher->RegisterOnAdd<LightComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] LightComponent* c) {
			uint32_t mapID = m_EngineCommand->GetResourceManager()->GetLightIndex();
			c->mapID = mapID;
		});
	m_pComponentEventDispatcher->RegisterOnCopy<LightComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] LightComponent* c) {
			LightComponent& srcLight = *m_pECSManager->GetComponent<LightComponent>(src);
			*c = srcLight;
			uint32_t mapID = m_EngineCommand->GetResourceManager()->GetLightIndex();
			c->mapID = mapID;
		});
	m_pComponentEventDispatcher->RegisterOnRemove<LightComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] LightComponent* c) {
			// Light統合バッファからmapIDを削除
			m_EngineCommand->GetResourceManager()->RecycleLightIndex(c->mapID.value());
		});
	// AudioComponent
	m_pComponentEventDispatcher->RegisterOnAdd<AudioComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] AudioComponent* c) {
		});
	m_pComponentEventDispatcher->RegisterOnCopy<AudioComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] AudioComponent* c) {
			AudioComponent& srcAudio = *m_pECSManager->GetComponent<AudioComponent>(src);
			*c = srcAudio;
		});
	m_pComponentEventDispatcher->RegisterOnRemove<AudioComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] AudioComponent* c) {
			for (auto& sound : c->soundData)
			{
				if (sound.isPlaying)
				{
					m_EngineCommand->GetResourceManager()->GetAudioManager()->SoundStop(sound);
				}
			}
		});
	// AnimationComponent
	m_pComponentEventDispatcher->RegisterOnAdd<AnimationComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] AnimationComponent* c) {
			MeshFilterComponent* meshFilter = m_pECSManager->GetComponent<MeshFilterComponent>(e);
			if (!meshFilter) { return; }
			ModelData* model = m_EngineCommand->GetResourceManager()->GetModelManager()->GetModelData(meshFilter->modelID.value());
			if (!model) { return; }
			if (model->animations.empty()) { return; }
			if (model->isBone)
			{
				c->skeleton = model->skeleton;
				c->skinCluster = model->skinCluster;
				c->modelName = model->name;
				c->boneOffsetID = model->AllocateBoneOffsetIdx();
			}
		});
	m_pComponentEventDispatcher->RegisterOnCopy<AnimationComponent>(
		[&]([[maybe_unused]] Entity src, [[maybe_unused]] Entity dst, [[maybe_unused]] AnimationComponent* c) {
			AnimationComponent& srcAnimation = *m_pECSManager->GetComponent<AnimationComponent>(src);
			*c = srcAnimation;
			MeshFilterComponent* meshFilter = m_pECSManager->GetComponent<MeshFilterComponent>(dst);
			if (!meshFilter) { return; }
			ModelData* model = m_EngineCommand->GetResourceManager()->GetModelManager()->GetModelData(meshFilter->modelID.value());
			if (!model) { return; }
			if (model->animations.empty()) { return; }
			if (model->isBone)
			{
				c->skeleton = model->skeleton;
				c->skinCluster = model->skinCluster;
				c->modelName = model->name;
				c->boneOffsetID = model->AllocateBoneOffsetIdx();
			}
		});
	m_pComponentEventDispatcher->RegisterOnRemove<AnimationComponent>(
		[&]([[maybe_unused]] Entity e, [[maybe_unused]] AnimationComponent* c) {
			ModelData* model = m_EngineCommand->GetResourceManager()->GetModelManager()->GetModelData(c->modelName);
			model->RemoveBoneOffsetIdx(c->boneOffsetID.value());
		});
	// Prefabにコンポーネントを登録
	Prefab::RegisterCopyFunc<TransformComponent>();
	Prefab::RegisterCopyFunc<CameraComponent>();
	Prefab::RegisterCopyFunc<MeshFilterComponent>();
	Prefab::RegisterCopyFunc<MeshRendererComponent>();
	Prefab::RegisterCopyFunc<ScriptComponent>();
	Prefab::RegisterCopyFunc<MaterialComponent>();
	Prefab::RegisterCopyFunc<ParticleComponent>();
	Prefab::RegisterCopyFunc<EmitterComponent>();
	Prefab::RegisterCopyFunc<LineRendererComponent>();
	Prefab::RegisterCopyFunc<Rigidbody2DComponent>();
	Prefab::RegisterCopyFunc<BoxCollider2DComponent>();
	Prefab::RegisterCopyFunc<EffectComponent>();
	Prefab::RegisterCopyFunc<UISpriteComponent>();
	Prefab::RegisterCopyFunc<LightComponent>();
	Prefab::RegisterCopyFunc<AudioComponent>();
	Prefab::RegisterCopyFunc<AnimationComponent>();
}

void GameCore::CreateSystems(InputManager* input, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine)
{
	// シングルシステム
	// 初期化システムの登録
	// TransformComponentの初期化
	m_pECSManager->AddSystem<TransformInitializeSystem>();
	auto tfSystem = m_pECSManager->GetSystem<TransformInitializeSystem>();
	std::unique_ptr<ECSManager::ISystem> tfStateSystem = std::make_unique<TransformInitializeSystem>(m_pECSManager.get());

	m_pSingleSystemManager->RegisterSystem(std::move(tfStateSystem), SystemState::Initialize);
	// Particleの初期化
	std::unique_ptr<ECSManager::ISystem> particleInitSystem = std::make_unique<ParticleInitializeSystem>(m_pECSManager.get(),resourceManager,graphicsEngine);
	m_pSingleSystemManager->RegisterSystem(std::move(particleInitSystem), SystemState::Initialize);
	// スクリプトのインスタンス生成
	std::unique_ptr<ECSManager::ISystem> scriptGenerateSystem = std::make_unique<ScriptGenerateInstanceSystem>(m_pObjectContainer.get(), input, m_pECSManager.get(), resourceManager);
	m_pSingleSystemManager->RegisterSystem(std::move(scriptGenerateSystem), SystemState::Initialize);
	// スクリプトの初期化
	std::unique_ptr<ECSManager::ISystem> scriptInitializeSystem = std::make_unique<ScriptInitializeSystem>(m_pObjectContainer.get(), input, m_pECSManager.get(), resourceManager);
	m_pSingleSystemManager->RegisterSystem(std::move(scriptInitializeSystem), SystemState::Initialize);
	// Rigidbody2DComponentの初期化
	std::unique_ptr<ECSManager::ISystem> physicsSystem = std::make_unique<Rigidbody2DInitSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(physicsSystem), SystemState::Initialize);
	// BoxCollider2DComponentの初期化
	std::unique_ptr<ECSManager::ISystem> boxInitSystem = std::make_unique<BoxCollider2DInitSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(boxInitSystem), SystemState::Initialize);
	// 更新システムの登録
	std::unique_ptr<ECSManager::ISystem> scriptUpdateSystem = std::make_unique<ScriptUpdateSystem>(m_pObjectContainer.get(), input, m_pECSManager.get(), resourceManager);
	m_pSingleSystemManager->RegisterSystem(std::move(scriptUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> materialUpdateSystem = std::make_unique<MaterialUpdateSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Material));
	m_pSingleSystemManager->RegisterSystem(std::move(materialUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> tfUpdateSystem = std::make_unique<TransformUpdateSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pSingleSystemManager->RegisterSystem(std::move(tfUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> animationUpdateSystem = std::make_unique<AnimationUpdateSystem>(m_pECSManager.get(), resourceManager,graphicsEngine);
	m_pSingleSystemManager->RegisterSystem(std::move(animationUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> cameraSystem = std::make_unique<CameraUpdateSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pSingleSystemManager->RegisterSystem(std::move(cameraSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> emitterUpdateSystem = std::make_unique<EmitterUpdateSystem>(m_pECSManager.get(), resourceManager, graphicsEngine);
	m_pSingleSystemManager->RegisterSystem(std::move(emitterUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> lightUpdateSystem = std::make_unique<LightUpdateSystem>(m_pECSManager.get(), resourceManager,graphicsEngine);
	m_pSingleSystemManager->RegisterSystem(std::move(lightUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> particleUpdateSystem = std::make_unique<ParticleUpdateSystem>(m_pECSManager.get(), resourceManager,graphicsEngine);
	m_pSingleSystemManager->RegisterSystem(std::move(particleUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> boxUpdateSystem = std::make_unique<BoxCollider2DUpdateSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(boxUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> collisionSystem = std::make_unique<CollisionSystem>(m_pECSManager.get(), resourceManager, input, m_pObjectContainer.get());
	m_pSingleSystemManager->RegisterSystem(std::move(collisionSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> rbUpdateSystem = std::make_unique<Rigidbody2DUpdateSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(rbUpdateSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> uiUpdateSystem = std::make_unique<UIUpdateSystem>(m_pECSManager.get(), resourceManager, graphicsEngine,resourceManager->GetIntegrationBuffer(IntegrationDataType::UISprite));
	m_pSingleSystemManager->RegisterSystem(std::move(uiUpdateSystem), SystemState::Update);
	// クリーンアップシステムの登録
	std::unique_ptr<ECSManager::ISystem> tfFinalizeSystem = std::make_unique<TransformFinalizeSystem>(m_pECSManager.get());
	m_pSingleSystemManager->RegisterSystem(std::move(tfFinalizeSystem), SystemState::Finalize);
	std::unique_ptr<ECSManager::ISystem> scriptFinalizeSystem = std::make_unique<ScriptFinalizeSystem>(m_pECSManager.get());
	m_pSingleSystemManager->RegisterSystem(std::move(scriptFinalizeSystem), SystemState::Finalize);
	std::unique_ptr<ECSManager::ISystem> physicsResetSystem = std::make_unique<Rigidbody2DResetSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	m_pSingleSystemManager->RegisterSystem(std::move(physicsResetSystem), SystemState::Finalize);
	std::unique_ptr<ECSManager::ISystem> particleFinaSystem = std::make_unique<ParticleInitializeSystem>(m_pECSManager.get(), resourceManager, graphicsEngine);
	m_pSingleSystemManager->RegisterSystem(std::move(particleFinaSystem), SystemState::Finalize);
	// マルチシステム
	// 初期化システムの登録
	// 更新システムの登録
	std::unique_ptr<ECSManager::IMultiSystem> lineRendererSystem = std::make_unique<LineRendererSystem>(m_pECSManager.get(), resourceManager);
	m_pMultiSystemManager->RegisterSystem(std::move(lineRendererSystem), SystemState::Update);
	// クリーンアップシステムの登録

	// エディタシステム
	// シングルシステムの生成
	std::unique_ptr<ECSManager::ISystem> materialEditorSystem = std::make_unique<MaterialEditorSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Material));
	m_pEditorSingleSystem->RegisterSystem(std::move(materialEditorSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> transformEditorSystem = std::make_unique<TransformEditorSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pEditorSingleSystem->RegisterSystem(std::move(transformEditorSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> cameraEditorSystem = std::make_unique<CameraEditorSystem>(m_pECSManager.get(), resourceManager, resourceManager->GetIntegrationBuffer(IntegrationDataType::Transform));
	m_pEditorSingleSystem->RegisterSystem(std::move(cameraEditorSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> lightEditorSystem = std::make_unique<LightUpdateSystem>(m_pECSManager.get(), resourceManager, graphicsEngine);
	m_pEditorSingleSystem->RegisterSystem(std::move(lightEditorSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> emitterEditorSystem = std::make_unique<EmitterEditorUpdateSystem>(m_pECSManager.get(), resourceManager, graphicsEngine);
	m_pEditorSingleSystem->RegisterSystem(std::move(emitterEditorSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> effectEditorSystem = std::make_unique<EffectEditorUpdateSystem>(m_pECSManager.get(), m_EngineCommand);
	m_pEditorSingleSystem->RegisterSystem(std::move(effectEditorSystem), SystemState::Update);
	std::unique_ptr<ECSManager::ISystem> uiUpdateEditorSystem = std::make_unique<UIUpdateSystem>(m_pECSManager.get(), resourceManager, graphicsEngine, resourceManager->GetIntegrationBuffer(IntegrationDataType::UISprite));
	m_pEditorSingleSystem->RegisterSystem(std::move(uiUpdateEditorSystem), SystemState::Update);

	// マルチシステムの生成
	std::unique_ptr<ECSManager::IMultiSystem> lineRendererEditorSystem = std::make_unique<LineRendererSystem>(m_pECSManager.get(), resourceManager);
	m_pEditorMultiSystem->RegisterSystem(std::move(lineRendererEditorSystem), SystemState::Update);

	// 更新中に生成されたオブジェクトの初期化システム
	tfOnceSystem = std::make_unique<TransformInitializeSystem>(m_pECSManager.get());
	scriptGenerateOnceSystem = std::make_unique<ScriptGenerateInstanceSystem>(m_pObjectContainer.get(), input, m_pECSManager.get(), resourceManager);
	scriptInitializeOnceSystem = std::make_unique<ScriptInitializeSystem>(m_pObjectContainer.get(), input, m_pECSManager.get(), resourceManager);
	physicsOnceSystem = std::make_unique<Rigidbody2DInitSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	boxInitOnceSystem = std::make_unique<BoxCollider2DInitSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
	particleInitializeOnceSystem = std::make_unique<ParticleInitializeSystem>(m_pECSManager.get(), resourceManager, graphicsEngine);

	tfFinalizeOnceSystem = std::make_unique<TransformFinalizeSystem>(m_pECSManager.get());
	scriptFinalizeOnceSystem = std::make_unique<ScriptFinalizeSystem>(m_pECSManager.get());
	physicsResetOnceSystem = std::make_unique<Rigidbody2DResetSystem>(m_pECSManager.get(), m_pPhysicsWorld.get());
}

