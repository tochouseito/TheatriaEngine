#include "pch.h"
#include "SingleSystems.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "GameCore/ObjectContainer/ObjectContainer.h"
#include "GameCore/IScript/IScript.h"
#include "Platform/FileSystem/FileSystem.h"
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

	//// 変更がなければreturn
	//if (transform.translation == transform.prePos &&
	//	radians == transform.preRot &&
	//	transform.scale == transform.preScale)
	//{
	//	if (transform.parent && *transform.parent == transform.preParent)
	//	{
	//		return;
	//	}
	//}

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
	/*if (transform.parent)
	{
		transform.preParent = *transform.parent;
	}*/

	// 親があれば親のワールド行列を掛ける
	if (transform.parent)
	{
		transform.matWorld = ChoMath::Multiply(transform.matWorld, m_pECS->GetComponent<TransformComponent>(transform.parent.value())->matWorld);
	}

	// 物理コンポーネントがあれば、物理ボディの位置を更新
	Rigidbody2DComponent* rb = m_pECS->GetComponent<Rigidbody2DComponent>(e);
	if (rb && rb->runtimeBody)
	{
		rb->runtimeBody->SetLinearVelocity(b2Vec2(rb->velocity.x, rb->velocity.y));
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
	if (transform.materialID)
	{
		data.materialID = transform.materialID.value();
	} else
	{
		data.materialID = 0;
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
	// アフィン変換
	transform.matWorld = ChoMath::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);

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
	CreateScriptFunc createScript = (CreateScriptFunc)GetProcAddress(FileSystem::ScriptProject::m_DllHandle, funcName.c_str());
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
		otherObject.Initialize();
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
	// フリーリストインデックスバッファをセット
	IRWStructuredBuffer* freeListIndexBuffer = m_pResourceManager->GetBuffer<IRWStructuredBuffer>(particle.freeListIndexBufferIndex);
	context->SetComputeRootDescriptorTable(1, freeListIndexBuffer->GetUAVGpuHandle());
	// フリーリストバッファをセット
	IRWStructuredBuffer* freeListBuffer = m_pResourceManager->GetBuffer<IRWStructuredBuffer>(particle.freeListBufferIndex);
	context->SetComputeRootDescriptorTable(2, freeListBuffer->GetUAVGpuHandle());
	// Dispatch
	context->Dispatch(particle.count, 1, 1);
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
	IRWStructuredBuffer* freeListIndexBuffer = m_pResourceManager->GetBuffer<IRWStructuredBuffer>(particle.freeListIndexBufferIndex);
	context->SetComputeRootDescriptorTable(3, freeListIndexBuffer->GetUAVGpuHandle());
	// フリーリストバッファをセット
	IRWStructuredBuffer* freeListBuffer = m_pResourceManager->GetBuffer<IRWStructuredBuffer>(particle.freeListBufferIndex);
	context->SetComputeRootDescriptorTable(4, freeListBuffer->GetUAVGpuHandle());
	// Dispatch
	context->Dispatch(1, 1, 1);
	
	// EmitとUpdateの並列を阻止
	context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, particleBuffer->GetResource());
	context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, freeListIndexBuffer->GetResource());
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
	context->SetComputeRootDescriptorTable(2, freeListIndexBuffer->GetUAVGpuHandle());
	// フリーリストバッファをセット
	context->SetComputeRootDescriptorTable(3, freeListBuffer->GetUAVGpuHandle());
	// Dispatch
	context->Dispatch(1, 1, 1);

	// クローズ
	m_pGraphicsEngine->EndCommandContext(context, QueueType::Compute);
	// GPUの処理が終わるまで待機
	m_pGraphicsEngine->WaitForGPU(QueueType::Compute);

}
