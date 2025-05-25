#include "pch.h"
#include "SingleSystems.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "GameCore/ObjectContainer/ObjectContainer.h"
#include "GameCore/IScript/IScript.h"
#include "Platform/FileSystem/FileSystem.h"
#include "OS/Windows/WinApp/WinApp.h"
#include "Core/ChoLog/ChoLog.h"
using namespace Cho;
#include "Platform/Timer/Timer.h"
// 優先度順に更新する
void TransformUpdateSystem::priorityUpdate(ECSManager* ecs)
{
	std::vector<std::pair<Entity, TransformComponent*>> transforms;

	// すべてのTransformComponentを取得
	for (auto& [arch, container] : ecs->GetArchToEntities())
	{
		if ((arch & m_Required) == m_Required)
		{
			for (Entity e : container.GetEntities())
			{
				if (auto* t = ecs->GetComponent<TransformComponent>(e))
				{
					transforms.emplace_back(e, t);
				}
			}
		}
	}

	// 親子関係優先 + tickPriority順で安定ソート
	std::sort(transforms.begin(), transforms.end(),
		[](const auto& a, const auto& b)
		{
			const auto& aParent = a.second->parent;
			const auto& bParent = b.second->parent;

			if (aParent && bParent)
			{
				if (*aParent == b.first) return false; // b is parent of a
				if (*bParent == a.first) return true;  // a is parent of b
			} else if (aParent && *aParent == b.first) return false;
			else if (bParent && *bParent == a.first) return true;

			// 優先度比較
			return a.second->tickPriority < b.second->tickPriority;
		});

	// 更新実行
	for (auto& [e, t] : transforms)
	{
		UpdateComponent(e, *t);
	}
}

void TransformUpdateSystem::UpdateComponent(Entity e, TransformComponent& transform)
{
	// 度数からラジアンに変換
	Vector3 radians = ChoMath::DegreesToRadians(transform.degrees);

	// 差分計算
	Vector3 diff = radians - transform.preRot;

	// 各軸のクオータニオンを作成
	Quaternion qx = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), diff.x);
	Quaternion qy = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), diff.y);
	Quaternion qz = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), diff.z);

	// 同時回転を累積
	transform.rotation = transform.rotation * qx * qy * qz;//*transform.rotation;

	// 精度を維持するための正規化
	transform.rotation.Normalize();

	// アフィン変換
	transform.matWorld = ChoMath::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);

	// 次のフレーム用に保存する
	transform.prePos = transform.translation;
	transform.preRot = radians;
	transform.preScale = transform.scale;

	// 親があれば親のワールド行列を掛ける
	if (transform.parent.has_value())
	{
		transform.matWorld = ChoMath::Multiply(transform.matWorld, m_pECS->GetComponent<TransformComponent>(transform.parent.value())->matWorld);
	}

	// 物理コンポーネントがあれば、物理ボディの位置を更新
	Rigidbody2DComponent* rb = m_pECS->GetComponent<Rigidbody2DComponent>(e);
	if (rb && rb->runtimeBody)
	{
		rb->runtimeBody->SetLinearVelocity(b2Vec2(rb->velocity.x, rb->velocity.y));
	}

	// アニメーションコンポーネントがあればスキニングの確認
	AnimationComponent* anim = m_pECS->GetComponent<AnimationComponent>(e);
	if (anim && anim->boneOffsetID.has_value()) {
		transform.boneOffsetID = anim->boneOffsetID.value();
	}

	// 行列の転送
	TransferMatrix(transform);
}

void TransformUpdateSystem::TransferMatrix(TransformComponent& transform)
{
	// マップデータ更新
	BUFFER_DATA_TF data = {};
	data.matWorld = transform.matWorld;
	data.worldInverse = ChoMath::Transpose(Matrix4::Inverse(transform.matWorld));
	data.rootMatrix = transform.rootMatrix;
	if (transform.materialID.has_value())
	{
		data.materialID = transform.materialID.value();
	} else
	{
		data.materialID = 0;
	}
	if(transform.boneOffsetID.has_value()){
		data.isAnimated = 1;
		data.boneOffsetStartIndex = transform.boneOffsetID.value();
	} else
	{
		data.isAnimated = 0;
		data.boneOffsetStartIndex = 0;
	}
	m_pIntegrationBuffer->UpdateData(data, transform.mapID.value());
}

void TransformInitializeSystem::Start(TransformComponent& transform)
{
	// 初期値保存
	transform.startValue.translation = transform.translation;
	transform.startValue.rotation = transform.rotation;
	transform.startValue.scale = transform.scale;
	transform.startValue.degrees = transform.degrees;

	// 度数からラジアンに変換
	Vector3 radians = ChoMath::DegreesToRadians(transform.degrees);

	// 各軸のクオータニオンを作成
	Quaternion qx = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), radians.x);
	Quaternion qy = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), radians.y);
	Quaternion qz = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), radians.z);

	// 同時回転を累積
	transform.rotation = qx * qy * qz;

	// 精度を維持するための正規化
	transform.rotation.Normalize();

	// アフィン変換
	transform.matWorld = ChoMath::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);

	// 次のフレーム用に保存する
	transform.prePos = transform.translation;
	transform.preRot = radians;
	transform.preScale = transform.scale;
}

void TransformFinalizeSystem::Finalize(Entity entity,TransformComponent& transform)
{
	entity;
	// 初期値に戻す
	transform.translation = transform.startValue.translation;
	transform.rotation = transform.startValue.rotation;
	transform.scale = transform.startValue.scale;
	transform.degrees = transform.startValue.degrees;
}

void CameraUpdateSystem::UpdateMatrix(TransformComponent& transform, CameraComponent& camera)
{ 
	TransferMatrix(transform, camera);
}

void CameraUpdateSystem::TransferMatrix(TransformComponent& transform, CameraComponent& camera)
{
	BUFFER_DATA_VIEWPROJECTION data = {};
	data.matWorld = transform.matWorld;
	data.view = Matrix4::Inverse(transform.matWorld);
	data.projection = ChoMath::MakePerspectiveFovMatrix(camera.fovAngleY, camera.aspectRatio, camera.nearZ, camera.farZ);
	data.projectionInverse = Matrix4::Inverse(data.projection);
	data.cameraPosition = transform.translation;
	ConstantBuffer<BUFFER_DATA_VIEWPROJECTION>* buffer = dynamic_cast<ConstantBuffer<BUFFER_DATA_VIEWPROJECTION>*>(m_pResourceManager->GetBuffer<IConstantBuffer>(camera.bufferIndex));
	buffer->UpdateData(data);
}

void ScriptGenerateInstanceSystem::InstanceGenerate(ScriptComponent& script)
{
	Log::Write(LogLevel::Info, "Loading script: " + script.scriptName);
	GameObject& object = m_pObjectContainer->GetGameObject(script.objectID.value());
	if (script.scriptName.empty() || !object.IsActive())
	{
		script.isActive = false;
		return;
	}
	std::string funcName = "Create" + script.scriptName + "Script";
	funcName.erase(std::remove_if(funcName.begin(), funcName.end(), ::isspace), funcName.end());
	// CreateScript関数を取得
	typedef IScript* (*CreateScriptFunc)(GameObject&);
	CreateScriptFunc createScript = (CreateScriptFunc)GetProcAddress(Cho::FileSystem::ScriptProject::m_DllHandle, funcName.c_str());
	if (!createScript)
	{
		script.isActive = false;
		return;
	}
	// スクリプトを生成
	object.Initialize();// 初期化
	IScript* scriptInstance = createScript(object);
	if (!scriptInstance)
	{
		script.isActive = false;
		return;
	}
	//// スクリプトのメンバ情報をリフレクション
	//auto typeFn = (GetTypeInfoFn)GetProcAddress(Cho::FileSystem::ScriptProject::m_DllHandle, ("GetScriptTypeInfo" + script.scriptName).c_str());
	//if (typeFn)
	//{
	//	const ScriptTypeInfo* info = typeFn();
	//	info;
	//}
	//auto funcFn = (GetFuncListFn)GetProcAddress(Cho::FileSystem::ScriptProject::m_DllHandle, ("GetScriptFunctions" + script.scriptName).c_str());
	//if (funcFn)
	//{
	//	const auto& functions = *funcFn();
	//	functions;
	//}
	//for (const auto& member : info->members)
	//{
	//	std::cout << "Name: " << member.name << ", Type: " << member.type.name() << "\n";

	//	if (member.type == typeid(int))
	//	{
	//		int* value = static_cast<int*>(member.getter(scriptInstance));
	//		std::cout << "Value = " << *value << std::endl;
	//		int newValue = 200;
	//		member.setter(static_cast<void*>(scriptInstance), static_cast<void*>(&newValue));
	//	}
	//}
	//GetScriptFunctionsFn getFuncs = (GetScriptFunctionsFn)GetProcAddress(Cho::FileSystem::ScriptProject::m_DllHandle, "GetScriptFunctions");
	//if (getFuncs)
	//{
	//	const std::vector<ScriptFunction>& funcs = *getFuncs(); // ← ここで関数呼び出し

	//	for (const auto& func : funcs)
	//	{
	//		std::cout << "Call Function: " << func.name << std::endl;
	//		func.invoker(static_cast<void*>(scriptInstance)); // 実行
	//	}
	//} else
	//{
	//	std::cerr << "GetScriptFunctions not found in DLL!" << std::endl;
	//}
	// スクリプトのStart関数とUpdate関数をラップ
	script.startFunc = [scriptInstance]() {
		scriptInstance->Start();
		};
	script.updateFunc = [scriptInstance]() {
		scriptInstance->Update();
		};
	// インスタンスの解放用のクロージャを設定
	script.cleanupFunc = [scriptInstance, this]() {
		delete scriptInstance;
		};
	// 衝突関数をラップ
	script.onCollisionEnterFunc = [scriptInstance](GameObject& other) {
		scriptInstance->OnCollisionEnter(other);
		};
	script.onCollisionStayFunc = [scriptInstance](GameObject& other) {
		scriptInstance->OnCollisionStay(other);
		};
	script.onCollisionExitFunc = [scriptInstance](GameObject& other) {
		scriptInstance->OnCollisionExit(other);
		};
	script.scriptInstance = scriptInstance;
	script.isActive = true;
	Log::Write(LogLevel::Info, "Script loaded: " + script.scriptName);
}

void ScriptInitializeSystem::StartScript(ScriptComponent& script)
{
	Log::Write(LogLevel::Info, "Starting script: " + script.scriptName);
	if (!script.isActive) return;
	Log::Write(LogLevel::Info, "scriptIsActive");
	try
	{
		// スクリプトのStart関数を呼び出す
		script.startFunc();
	}
	catch (const std::exception& e)
	{
		// スクリプトのエラー処理
		Log::Write(LogLevel::Debug, "Script error: " + std::string(e.what()));
		script.isActive = false;
	}
	catch (...)
	{
		// その他のエラー処理
		Log::Write(LogLevel::Debug, "Unknown script error");
		script.isActive = false;
	}
	Log::Write(LogLevel::Info, "Script started: " + script.scriptName);
}
void ScriptUpdateSystem::UpdateScript(ScriptComponent& script)
{
	Log::Write(LogLevel::Info, "Updating script: " + script.scriptName);
	if (!script.isActive) return;
	try
	{
		// スクリプトのUpdate関数を呼び出す
		script.updateFunc();
	}
	catch (const std::exception& e)
	{
		// スクリプトのエラー処理
		Log::Write(LogLevel::Debug, "Script error: " + std::string(e.what()));
		script.isActive = false;
	}
	catch (...)
	{
		// その他のエラー処理
		Log::Write(LogLevel::Debug, "Unknown script error");
		script.isActive = false;
	}
	Log::Write(LogLevel::Info, "Script updated: " + script.scriptName);
}

void ScriptFinalizeSystem::FinalizeScript(ScriptComponent& script)
{
	if (!script.isActive) return;
	try
	{
		// スクリプトのCleanup関数を呼び出す
		script.cleanupFunc();
		script.scriptInstance = nullptr;
	}
	catch (const std::exception& e)
	{
		// スクリプトのエラー処理
		Log::Write(LogLevel::Debug, "Script error: " + std::string(e.what()));
	}
	catch (...)
	{
		// その他のエラー処理
		Log::Write(LogLevel::Debug, "Unknown script error");
	}
	script.isActive = false;
}

void CollisionSystem::CollisionStay(ScriptComponent& script, Rigidbody2DComponent& rb)
{
	if (script.isActive&&rb.isCollisionStay&&rb.otherObjectID)
	{
		// 相手のゲームオブジェクトを取得
		GameObject& otherObject = m_pObjectContainer->GetGameObject(rb.otherObjectID.value());
		if (!otherObject.IsActive()) { return; }
		if (!rb.isActive) { return; }
		otherObject.Initialize(false);// これいる？？
		script.onCollisionStayFunc(otherObject);
	}
}

void MaterialUpdateSystem::TransferComponent(const MaterialComponent& material)
{
	BUFFER_DATA_MATERIAL data = {};
	data.color = material.color;
	data.enableLighting = material.enableLighting;
	if (material.enableTexture)
	{
		data.enableTexture = true;
		if (!material.textureName.empty() && m_pResourceManager->GetTextureManager()->IsTextureExist(material.textureName))
		{
			if (material.textureID.has_value())
			{
				data.textureId = material.textureID.value();
			} else
			{
				data.textureId = m_pResourceManager->GetTextureManager()->GetTextureID(material.textureName);
			}
		} else
		{
			data.textureId = 0;
		}
	} else
	{
		data.enableTexture = 0;
		data.textureId = 0;
	}
	data.matUV = material.matUV.Identity();
	data.shininess = material.shininess;
	data.uvFlipY = material.uvFlipY;
	m_pIntegrationBuffer->UpdateData(data, material.mapID.value());
}

void EmitterUpdateSystem::UpdateEmitter(EmitterComponent& emitter)
{
	emitter.frequencyTime += DeltaTime();

	// 射出間隔を上回ったら射出許可を出して時間を調整
	if (emitter.frequency <= emitter.frequencyTime)
	{
		emitter.frequencyTime = 0.0f;
		emitter.emit = 1;
	} else
	{
		// 射出間隔を上回ってないので、許可は出せない
		emitter.emit = 1;
	}

	BUFFER_DATA_EMITTER data = {};
	data.position = emitter.position;
	data.radius = emitter.radius;
	data.count = emitter.count;
	data.frequency = emitter.frequency;
	data.frequencyTime = emitter.frequencyTime;
	data.emit = emitter.emit;
	ConstantBuffer<BUFFER_DATA_EMITTER>* buffer = dynamic_cast<ConstantBuffer<BUFFER_DATA_EMITTER>*>(m_pResourceManager->GetBuffer<IConstantBuffer>(emitter.bufferIndex));
	buffer->UpdateData(data);
}

void ParticleInitializeSystem::InitializeParticle(ParticleComponent& particle)
{
	particle.time = 0.0f;

	// CSで初期化
	// コンテキスト取得
	CommandContext* context = m_pGraphicsEngine->GetCommandContext();
	// コマンドリスト開始
	m_pGraphicsEngine->BeginCommandContext(context);
	// パイプラインセット
	context->SetComputePipelineState(m_pGraphicsEngine->GetPipelineManager()->GetParticleInitPSO().pso.Get());
	// ルートシグネチャセット
	context->SetComputeRootSignature(m_pGraphicsEngine->GetPipelineManager()->GetParticleInitPSO().rootSignature.Get());
	// パーティクルバッファをセット
	IRWStructuredBuffer* particleBuffer = m_pResourceManager->GetBuffer<IRWStructuredBuffer>(particle.bufferIndex);
	context->SetComputeRootDescriptorTable(0, particleBuffer->GetUAVGpuHandle());
	// フリーリストバッファをセット
	IRWStructuredBuffer* freeListBuffer = m_pResourceManager->GetBuffer<IRWStructuredBuffer>(particle.freeListBufferIndex);
	context->SetComputeRootDescriptorTable(1, freeListBuffer->GetUAVGpuHandle());
	// カウンターバッファをセット
	context->SetComputeRootUnorderedAccessView(2, freeListBuffer->GetCounterResource()->GetGPUVirtualAddress());
	// Dispatch
	context->Dispatch(1, 1, 1);
	// コマンドリストをクローズ
	m_pGraphicsEngine->EndCommandContext(context,QueueType::Compute);
	// GPUの処理が終わるまで待機
	m_pGraphicsEngine->WaitForGPU(QueueType::Compute);
}

void ParticleUpdateSystem::UpdateParticle(EmitterComponent& emitter, ParticleComponent& particle)
{
	particle.time += DeltaTime();
	particle.deltaTime = DeltaTime();

	{
		BUFFER_DATA_PARTICLE_PERFRAME data = {};
		data.time = particle.time;
		data.deltaTime = particle.deltaTime;
		ConstantBuffer<BUFFER_DATA_PARTICLE_PERFRAME>* buffer = dynamic_cast<ConstantBuffer<BUFFER_DATA_PARTICLE_PERFRAME>*>(m_pResourceManager->GetBuffer<IConstantBuffer>(particle.perFrameBufferIndex));
		buffer->UpdateData(data);
	}
	// Emit
	CommandContext* context = m_pGraphicsEngine->GetCommandContext();
	m_pGraphicsEngine->BeginCommandContext(context);
	// パイプラインセット
	context->SetComputePipelineState(m_pGraphicsEngine->GetPipelineManager()->GetParticleEmitPSO().pso.Get());
	// ルートシグネチャセット
	context->SetComputeRootSignature(m_pGraphicsEngine->GetPipelineManager()->GetParticleEmitPSO().rootSignature.Get());
	// パーティクルバッファをセット
	IRWStructuredBuffer* particleBuffer = m_pResourceManager->GetBuffer<IRWStructuredBuffer>(particle.bufferIndex);
	context->SetComputeRootDescriptorTable(0, particleBuffer->GetUAVGpuHandle());
	// エミッターバッファをセット
	IConstantBuffer* emitterBuffer = m_pResourceManager->GetBuffer<IConstantBuffer>(emitter.bufferIndex);
	context->SetComputeRootConstantBufferView(1, emitterBuffer->GetResource()->GetGPUVirtualAddress());
	// PerFrameバッファをセット
	IConstantBuffer* perFrameBuffer = m_pResourceManager->GetBuffer<IConstantBuffer>(particle.perFrameBufferIndex);
	context->SetComputeRootConstantBufferView(2, perFrameBuffer->GetResource()->GetGPUVirtualAddress());
	// フリーリストインデックスバッファをセット
	//IRWStructuredBuffer* freeListIndexBuffer = m_pResourceManager->GetBuffer<IRWStructuredBuffer>(particle.freeListIndexBufferIndex);
	//context->SetComputeRootDescriptorTable(3, freeListIndexBuffer->GetUAVGpuHandle());
	// フリーリストバッファをセット
	IRWStructuredBuffer* freeListBuffer = m_pResourceManager->GetBuffer<IRWStructuredBuffer>(particle.freeListBufferIndex);
	context->SetComputeRootDescriptorTable(3, freeListBuffer->GetUAVGpuHandle());
	// Dispatch
	context->Dispatch(1, 1, 1);
	
	// EmitとUpdateの並列を阻止
	context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, particleBuffer->GetResource());
	//context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, freeListIndexBuffer->GetResource());
	context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, freeListBuffer->GetResource());

	// Update
	// パイプラインセット
	context->SetComputePipelineState(m_pGraphicsEngine->GetPipelineManager()->GetParticleUpdatePSO().pso.Get());
	// ルートシグネチャセット
	context->SetComputeRootSignature(m_pGraphicsEngine->GetPipelineManager()->GetParticleUpdatePSO().rootSignature.Get());
	// パーティクルバッファをセット
	context->SetComputeRootDescriptorTable(0, particleBuffer->GetUAVGpuHandle());
	// PerFrameバッファをセット
	context->SetComputeRootConstantBufferView(1, perFrameBuffer->GetResource()->GetGPUVirtualAddress());
	// フリーリストインデックスバッファをセット
	//context->SetComputeRootDescriptorTable(2, freeListIndexBuffer->GetUAVGpuHandle());
	// フリーリストバッファをセット
	context->SetComputeRootDescriptorTable(2, freeListBuffer->GetUAVGpuHandle());
	// Dispatch
	context->Dispatch(1, 1, 1);

	// クローズ
	m_pGraphicsEngine->EndCommandContext(context, QueueType::Compute);
	// GPUの処理が終わるまで待機
	m_pGraphicsEngine->WaitForGPU(QueueType::Compute);

}

void UIUpdateSystem::UpdateUI(Entity e, UISpriteComponent& uiSprite)
{
	// UIの更新
	float left = 0.0f - uiSprite.anchorPoint.x;
	float right = uiSprite.size.x - uiSprite.anchorPoint.x;
	float top = 0.0f - uiSprite.anchorPoint.y;
	float bottom = uiSprite.size.y - uiSprite.anchorPoint.y;

	float tex_left = uiSprite.textureLeftTop.x / uiSprite.size.x;
	float tex_right = (uiSprite.textureLeftTop.x + uiSprite.size.x) / uiSprite.size.x;
	float tex_top = uiSprite.textureLeftTop.y / uiSprite.size.y;
	float tex_bottom = (uiSprite.textureLeftTop.y + uiSprite.size.y) / uiSprite.size.y;

	//uiSprite.scale = uiSprite.size;
	Vector3 scale = Vector3(uiSprite.scale.x, uiSprite.scale.y, 1.0f);
	Vector3 rotation = Vector3(0.0f, 0.0f, uiSprite.rotation);
	Vector3 translation = Vector3(uiSprite.position.x, uiSprite.position.y, 0.0f);
	Matrix4 worldMatrixSprite = ChoMath::MakeAffineMatrix(scale, rotation, translation);

	Matrix4 viewMatrixSprite = ChoMath::MakeIdentity4x4();

	Matrix4 projectionMatrixSprite = ChoMath::MakeOrthographicMatrix(0.0f, 0.0f,
		static_cast<float>(WinApp::GetWindowWidth()), static_cast<float>(WinApp::GetWindowHeight()),
		0.0f, 100.0f
	);

	Matrix4 worldViewProjectionMatrixSprite = ChoMath::Multiply(worldMatrixSprite, ChoMath::Multiply(viewMatrixSprite, projectionMatrixSprite));

	uiSprite.matWorld = worldViewProjectionMatrixSprite;

	//uiSprite.material.matUV = MakeAffineMatrix(Vector3(uiSprite.uvScale.x, uiSprite.uvScale.y, 1.0f), Vector3(0.0f, 0.0f, uiSprite.uvRot), Vector3(uiSprite.uvPos.x, uiSprite.uvPos.y, 0.0f));

	//uiSprite.constData->matWorld = uiSprite.matWorld;

	BUFFER_DATA_UISPRITE data = {};
	data.matWorld = uiSprite.matWorld;
	data.left = left;
	data.right = right;
	data.top = top;
	data.bottom = bottom;
	data.tex_left = tex_left;
	data.tex_right = tex_right;
	data.tex_top = tex_top;
	data.tex_bottom = tex_bottom;
	MaterialComponent* material = m_pECS->GetComponent<MaterialComponent>(e);
	if (material && material->mapID)
	{
		data.materialID = material->mapID.value();
	} else
	{
		data.materialID = 0;
	}
	m_pIntegrationBuffer->UpdateData(data, uiSprite.mapID.value());
}

void BoxCollider2DUpdateSystem::UpdateFixture(const TransformComponent& transform, Rigidbody2DComponent& rb, BoxCollider2DComponent& box)
{
	transform;
	// サイズの変更があった場合、フィクスチャを再作成
	if (box.runtimeFixture == nullptr) { return; }
	if (box.width != box.runtimeFixture->GetShape()->m_radius || box.height != box.runtimeFixture->GetShape()->m_radius)
	{
		rb.runtimeBody->DestroyFixture(box.runtimeFixture);
		b2PolygonShape shape;
		shape.SetAsBox(box.width / 2.0f, box.height / 2.0f, b2Vec2(box.offsetX, box.offsetY), 0.0f);
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = box.density;
		fixtureDef.friction = box.friction;
		fixtureDef.restitution = box.restitution;
		fixtureDef.isSensor = box.isSensor;
		box.runtimeFixture = rb.runtimeBody->CreateFixture(&fixtureDef);
	}
}

void LightUpdateSystem::UpdateLight(Entity e, LightComponent& light, TransformComponent& transform)
{
	e;
	Vector3 direction = ChoMath::TransformDirection(Vector3(0.0f, 0.0f, 1.0f), ChoMath::MakeRotateXYZMatrix(ChoMath::DegreesToRadians(transform.degrees)));
	//direction.Normalize();
	// ライトのワールド行列を転送
	BUFFER_DATA_LIGHT& data = m_pResourceManager->GetLightBuffer()->GetData();
	uint32_t i = light.mapID.value();
	data.lightData[i].color = light.color;
	data.lightData[i].direction = direction;
	data.lightData[i].intensity = light.intensity;
	data.lightData[i].range = light.range;
	data.lightData[i].decay = light.decay;
	data.lightData[i].spotAngle = light.spotAngle;
	data.lightData[i].spotFalloffStart = light.spotFalloffStart;
	data.lightData[i].type = static_cast<uint32_t>(light.type);
	data.lightData[i].active = light.active;
	data.lightData[i].transformMapID = transform.mapID.value();
	// 参照に書き込んでいるので転送は不要
}

void AnimationInitializeSystem::InitializeAnimation(AnimationComponent& animation)
{
	animation;
}

void AnimationUpdateSystem::UpdateAnimation(AnimationComponent& animation)
{
	ModelData* model = m_pResourceManager->GetModelManager()->GetModelData(animation.modelName);
	timeUpdate(animation, model);
}

Vector3 AnimationUpdateSystem::CalculateValue(const std::vector<KeyframeVector3>& keyframes, const float& time)
{
	assert(!keyframes.empty());// キーがないものは返す値がわからないのでだめ
	if (keyframes.size() == 1 || time <= keyframes[0].time) {// キーが一つか、時刻がキーフレーム前なら最初の値とする
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Vector3::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}

Quaternion AnimationUpdateSystem::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, const float& time)
{
	assert(!keyframes.empty());// キーがないものは返す値がわからないのでだめ
	if (keyframes.size() == 1 || time <= keyframes[0].time) {// キーが一つか、時刻がキーフレーム前なら最初の値とする
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Quaternion::Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}

Scale AnimationUpdateSystem::CalculateValue(const std::vector<KeyframeScale>& keyframes, const float& time)
{
	assert(!keyframes.empty());// キーがないものは返す値がわからないのでだめ
	if (keyframes.size() == 1 || time <= keyframes[0].time) {// キーが一つか、時刻がキーフレーム前なら最初の値とする
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Scale::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}

void AnimationUpdateSystem::timeUpdate(AnimationComponent& animation, ModelData* model)
{
	if (animation.isRun) {
		// animationIndexが変更された場合、遷移を開始する
		if (animation.prevAnimationIndex != animation.animationIndex) {
			animation.transitionIndex = animation.prevAnimationIndex;
			animation.transition = true;
			animation.transitionTime = 0.0f;
		}

		// 遷移が終了した場合、遷移フラグをfalseにする
		if (animation.lerpTime >= 1.0f) {
			animation.transition = false;
			animation.lerpTime = 0.0f;
			animation.time = animation.transitionTime;
			animation.transitionTime = 0.0f;
		}

		/*エンジンのデルタタイムにする*/
		static float deltaTime = 1.0f / 60.0f;

		// 遷移中の場合、遷移時間を更新
		if (animation.transition) {
			animation.transitionTime += deltaTime;//DeltaTime();
			animation.lerpTime = animation.transitionTime / animation.transitionDuration;
			animation.lerpTime = std::clamp(animation.lerpTime, 0.0f, 1.0f);
		}

		// 時間更新
		animation.isEnd = false;
		animation.time += deltaTime;//DeltaTime();
		if (animation.time >= model->animations[animation.animationIndex].duration) {
			animation.isEnd = true;
			if (!animation.isRestart) {
				animation.isRun = false;
			}
		}
		else {
			animation.isEnd = false;
		}
		if (!animation.isRestart) {
			animation.time;
		}
		else {
			animation.time = std::fmod(animation.time, model->animations[animation.animationIndex].duration);
		}
	}
	// アニメーション更新
	ApplyAnimation(animation, model);
	if (model->isBone) {
		SkeletonUpdate(animation, model);
		SkinClusterUpdate(animation, model);
		//ApplySkinning(animation, model);
	}
	animation.prevAnimationIndex = animation.animationIndex;
}

void AnimationUpdateSystem::ApplyAnimation(AnimationComponent& animation, ModelData* model)
{
	for (Joint& joint : animation.skeleton->joints) {
		// 対象のJointのAnimationがあれば、値の適用を行う。下記のif文はC++17から可能になった初期化付きif文
		if (auto it = model->animations[animation.animationIndex].nodeAnimations.find(joint.name); it != model->animations[animation.animationIndex].nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			if (animation.transition) {
				if (auto it2 = model->animations[animation.transitionIndex].nodeAnimations.find(joint.name); it2 != model->animations[animation.transitionIndex].nodeAnimations.end()) {
					const NodeAnimation& rootNodeAnimation2 = (*it2).second;
					Vector3 startTranslate = CalculateValue(rootNodeAnimation2.translate.keyframes, animation.time);
					Quaternion startRotate = CalculateValue(rootNodeAnimation2.rotate.keyframes, animation.time);
					Scale startScale = CalculateValue(rootNodeAnimation2.scale.keyframes, animation.time);
					Vector3 endTranslate = CalculateValue(rootNodeAnimation.translate.keyframes, animation.transitionTime);
					Quaternion endRotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animation.transitionTime);
					Scale endScale = CalculateValue(rootNodeAnimation.scale.keyframes, animation.transitionTime);
					joint.transform.translation = Vector3::Lerp(startTranslate, endTranslate, animation.lerpTime);
					joint.transform.rotation = Quaternion::Slerp(startRotate, endRotate, animation.lerpTime);
					joint.transform.scale = Scale::Lerp(startScale, endScale, animation.lerpTime);
				}
			}
			else {
				joint.transform.translation = CalculateValue(rootNodeAnimation.translate.keyframes, animation.time);
				joint.transform.rotation = CalculateValue(rootNodeAnimation.rotate.keyframes, animation.time);
				joint.transform.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animation.time);
			}
		}
	}
}

void AnimationUpdateSystem::SkeletonUpdate(AnimationComponent& animation, ModelData* model)
{
	model;
	// すべてのJointを更新。親が若いので通常ループで処理可能になっている
	for (Joint& joint : animation.skeleton->joints) {
		joint.localMatrix = ChoMath::MakeAffineMatrix(joint.transform.scale, joint.transform.rotation, joint.transform.translation);
		if (joint.parent) {
			joint.skeletonSpaceMatrix = joint.localMatrix * animation.skeleton->joints[*joint.parent].skeletonSpaceMatrix;
		}
		else {// 親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

void AnimationUpdateSystem::SkinClusterUpdate(AnimationComponent& animation, ModelData* model)
{
	StructuredBuffer<ConstBufferDataWell>* paletteBuffer = dynamic_cast<StructuredBuffer<ConstBufferDataWell>*>(m_pResourceManager->GetBuffer<IStructuredBuffer>(model->boneMatrixBufferIndex));
	for (uint32_t jointIndex = 0; jointIndex < model->skeleton.joints.size(); ++jointIndex) {
		assert(jointIndex < model->skinCluster.inverseBindPoseMatrices.size());
		uint32_t offset = static_cast<uint32_t>(model->skeleton.joints.size() * animation.boneOffsetID.value());
		ConstBufferDataWell data = {};
		data.skeletonSpaceMatrix = 
			animation.skinCluster->inverseBindPoseMatrices[jointIndex] * animation.skeleton->joints[jointIndex].skeletonSpaceMatrix;
		data.skeletonSpaceInverseTransposeMatrix =
			ChoMath::Transpose(Matrix4::Inverse(data.skeletonSpaceMatrix));
		paletteBuffer->UpdateData(data, jointIndex + offset);
	}
}

void AnimationUpdateSystem::ApplySkinning(AnimationComponent& animation, ModelData* model)
{
	animation;
	model;
	//// コマンドリストの取得
	//CommandContext* context = m_pGraphicsEngine->GetCommandContext();
	//m_pGraphicsEngine->BeginCommandContext(context);
	//// スキニングOutputリソースを遷移
	//IVertexBuffer* skinningBuffer = m_pResourceManager->GetBuffer<IVertexBuffer>(animation.skinningBufferIndex);
	//context->BarrierTransition(skinningBuffer->GetResource(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	//// パイプラインセット
	//context->SetComputePipelineState(m_pGraphicsEngine->GetPipelineManager()->GetSkinningPSO().pso.Get());
	//// ルートシグネチャセット
	//context->SetComputeRootSignature(m_pGraphicsEngine->GetPipelineManager()->GetSkinningPSO().rootSignature.Get());
	//// パレットデータをセット
	//IStructuredBuffer* paletteBuffer = m_pResourceManager->GetBuffer<IStructuredBuffer>(animation.paletteBufferIndex);
	//context->SetComputeRootDescriptorTable(0, paletteBuffer->GetSRVGpuHandle());
	//// メッシュデータをセット
	//IVertexBuffer* vertexBuffer = m_pResourceManager->GetBuffer<IVertexBuffer>(model->meshes[0].vertexBufferIndex);
	//context->SetComputeRootDescriptorTable(1, vertexBuffer->GetSRVGpuHandle());
	//// 影響データをセット
	//IStructuredBuffer* influenceBuffer = m_pResourceManager->GetBuffer<IStructuredBuffer>(animation.influenceBufferIndex);
	//context->SetComputeRootDescriptorTable(2, influenceBuffer->GetSRVGpuHandle());
	//// スキニングデータをセット
	//context->SetComputeRootDescriptorTable(3, skinningBuffer->GetUAVGpuHandle());
	//// Skinning用情報
	//IConstantBuffer* infoBuffer = m_pResourceManager->GetBuffer<IConstantBuffer>(model->meshes[0].skinInfoBufferIndex);
	//context->SetComputeRootConstantBufferView(4, infoBuffer->GetResource()->GetGPUVirtualAddress());
	//// Dispatch
	//context->Dispatch(static_cast<UINT>(model->meshes[0].vertices.size() + 1023) / 1024, 1, 1);
	//// スキニングOutputリソースを元に戻す
	//context->BarrierTransition(skinningBuffer->GetResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	//// クローズ
	//m_pGraphicsEngine->EndCommandContext(context, QueueType::Compute);
	//// GPUの処理が終わるまで待機
	//m_pGraphicsEngine->WaitForGPU(QueueType::Compute);
}

void AnimationFinalizeSystem::FinalizeAnimation(AnimationComponent& animation)
{
	animation;
}
