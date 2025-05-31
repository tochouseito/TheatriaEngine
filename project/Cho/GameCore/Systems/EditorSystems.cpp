#include "pch.h"
#include "EditorSystems.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "EngineCommand/EngineCommand.h"
#include "GameCore/IScript/IScript.h"
#include "Core/Utility/EffectStruct.h"
#include "Platform/FileSystem/FileSystem.h"
#include "Platform/Timer/Timer.h"
#include "Core/ChoLog/ChoLog.h"
using namespace Cho;

void TransformEditorSystem::priorityUpdate(ECSManager* ecs)
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

void TransformEditorSystem::UpdateComponent(Entity e, TransformComponent& transform)
{
	e;
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
	transform.matWorld = ChoMath::MakeAffineMatrix(transform.scale, transform.rotation, transform.position);

	// 次のフレーム用に保存する
	transform.prePos = transform.position;
	transform.preRot = radians;
	transform.preScale = transform.scale;

	// 親があれば親のワールド行列を掛ける
	if (transform.parent)
	{
		transform.matWorld = ChoMath::Multiply(transform.matWorld, m_pECS->GetComponent<TransformComponent>(transform.parent.value())->matWorld);
	}

	// 行列の転送
	TransferMatrix(transform);
}

void TransformEditorSystem::TransferMatrix(TransformComponent& transform)
{
	// マップに登録
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

void CameraEditorSystem::UpdateMatrix(TransformComponent& transform, CameraComponent& camera)
{
	// アフィン変換
	transform.matWorld = ChoMath::MakeAffineMatrix(transform.scale, transform.rotation, transform.position);
	TransferMatrix(transform, camera);
}

void CameraEditorSystem::TransferMatrix(TransformComponent& transform, CameraComponent& camera)
{
	BUFFER_DATA_VIEWPROJECTION data = {};
	data.matWorld = transform.matWorld;
	data.view = Matrix4::Inverse(transform.matWorld);
	data.projection = ChoMath::MakePerspectiveFovMatrix(camera.fovAngleY, camera.aspectRatio, camera.nearZ, camera.farZ);
	data.projectionInverse = Matrix4::Inverse(data.projection);
	data.cameraPosition = transform.position;
	ConstantBuffer<BUFFER_DATA_VIEWPROJECTION>* buffer = dynamic_cast<ConstantBuffer<BUFFER_DATA_VIEWPROJECTION>*>(m_pResourceManager->GetBuffer<IConstantBuffer>(camera.bufferIndex));
	buffer->UpdateData(data);
}

void MaterialEditorSystem::TransferComponent(const MaterialComponent& material)
{
	BUFFER_DATA_MATERIAL data = {};
	data.color = material.color;
	data.enableLighting = material.enableLighting;
	if (material.enableTexture)
	{
		data.enableTexture = true;
		if (!material.textureName.empty()&&m_pResourceManager->GetTextureManager()->IsTextureExist(material.textureName))
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

void EmitterEditorUpdateSystem::UpdateEmitter(EmitterComponent& emitter)
{
	emitter.frequencyTime += DeltaTime();

	//// 射出間隔を上回ったら射出許可を出して時間を調整
	//if (emitter.frequency <= emitter.frequencyTime)
	//{
	//	emitter.frequencyTime = 0.0f;
	//	emitter.emit = 1;
	//} else
	//{
	//	// 射出間隔を上回ってないので、許可は出せない
	//	emitter.emit = 0;
	//}

	BUFFER_DATA_EMITTER data = {};
	data.position = emitter.position;
	/*data.radius = emitter.radius;
	data.count = emitter.count;*/
	data.frequency = emitter.frequency;
	data.frequencyTime = emitter.frequencyTime;
	data.emit = emitter.emit;
	//ConstantBuffer<BUFFER_DATA_EMITTER>* buffer = dynamic_cast<ConstantBuffer<BUFFER_DATA_EMITTER>*>(m_pResourceManager->GetBuffer<IConstantBuffer>(emitter.bufferIndex));
	//buffer->UpdateData(data);
}

void EffectEditorUpdateSystem::UpdateEffect(EffectComponent& effect)
{
	// EditorはTimeBaseでの更新
	if (!effect.isRun) { return; }

	// エフェクトの時間を更新
	effect.root.second.time.deltaTime = DeltaTime();
	effect.root.second.time.elapsedTime++;
	if (effect.root.second.time.elapsedTime > effect.root.second.time.duration)
	{
		if (effect.isLoop)
		{
			// ループする場合は時間をリセット
			effect.root.second.time.elapsedTime = 0.0f;
		}
		else
		{
			// ループしない場合は停止
			effect.isRun = false;
			return;
		}
	}

	// バッファに転送
	StructuredBuffer<EffectRoot>* rootBuffer = dynamic_cast<StructuredBuffer<EffectRoot>*>(m_pEngineCommand->m_ResourceManager->GetIntegrationBuffer(IntegrationDataType::EffectRootInt));
	StructuredBuffer<EffectNode>* nodeBuffer = dynamic_cast<StructuredBuffer<EffectNode>*>(m_pEngineCommand->m_ResourceManager->GetIntegrationBuffer(IntegrationDataType::EffectNodeInt));
	StructuredBuffer<EffectSprite>* spriteBuffer = dynamic_cast<StructuredBuffer<EffectSprite>*>(m_pEngineCommand->m_ResourceManager->GetIntegrationBuffer(IntegrationDataType::EffectSpriteInt));
	
	// EffectRootを更新
	EffectRoot root;
	root.globalSeed = effect.root.second.globalSeed;
	root.nodeCount = effect.root.second.nodeCount;
	root.time = effect.root.second.time;
	uint32_t i = 0;
	for (const auto& node : effect.root.second.nodes)
	{
		root.nodeID[i] = node.nodeID;
		i++;
		// Nodeを更新
		EffectNode nodeData;
		nodeData.common = node.common;
		nodeData.position = node.position;
		nodeData.rotation = node.rotation;
		nodeData.scale = node.scale;
		nodeData.drawCommon = node.drawCommon;
		nodeData.draw = node.draw;
		nodeData.isRootParent = node.isRootParent;
		nodeData.parentIndex = node.parentIndex;
		nodeBuffer->UpdateData(nodeData, node.nodeID);
		EFFECT_MESH_TYPE meshType = static_cast<EFFECT_MESH_TYPE>(node.draw.meshType);
		switch (meshType)
		{
		case EFFECT_MESH_TYPE::NONE:
			break;
		case EFFECT_MESH_TYPE::SPRITE: {
			// スプライトの場合はスプライトバッファに転送
			EffectSprite spriteData;
			const EffectSprite* sprite = std::get_if<EffectSprite>(&node.drawMesh);
			spriteData = *sprite;
			spriteBuffer->UpdateData(spriteData, node.draw.meshDataIndex);
			break;
		}
		case EFFECT_MESH_TYPE::RIBBON:
			break;
		case EFFECT_MESH_TYPE::TRAIL:
			break;
		case EFFECT_MESH_TYPE::RING:
			break;
		case EFFECT_MESH_TYPE::MODEL:
			break;
		case EFFECT_MESH_TYPE::CUBE:
			break;
		case EFFECT_MESH_TYPE::SPHERE:
			break;
		default:
			break;
		}
	}
	rootBuffer->UpdateData(root, effect.root.first);
}

void EffectEditorUpdateSystem::InitEffectParticle()
{
	// ParticleBuffer
	IRWStructuredBuffer* particleBuffer = m_pEngineCommand->GetResourceManager()->GetEffectParticleBuffer();
	// ParticleListBuffer
	IRWStructuredBuffer* particleListBuffer = m_pEngineCommand->GetResourceManager()->GetEffectParticleFreeListBuffer();
	// コマンドリスト開始
	CommandContext* context = m_pEngineCommand->GetGraphicsEngine()->GetCommandContext();
	m_pEngineCommand->GetGraphicsEngine()->BeginCommandContext(context);
	// パイプラインセット
	context->SetComputePipelineState(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectInitPSO().pso.Get());
	// ルートシグネチャセット
	context->SetComputeRootSignature(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectInitPSO().rootSignature.Get());
	// Particleバッファをセット
	context->SetComputeRootDescriptorTable(0, particleBuffer->GetUAVGpuHandle());
	// ParticleListバッファをセット
	context->SetComputeRootDescriptorTable(1, particleListBuffer->GetUAVGpuHandle());
	// ListCounterバッファをセット
	context->SetComputeRootUnorderedAccessView(2, particleListBuffer->GetCounterResource()->GetGPUVirtualAddress());
	// Dispatch
	context->Dispatch(128, 1, 1);
	// 並列阻止
	context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, particleBuffer->GetResource());
	context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, particleListBuffer->GetResource());
	// クローズ
	m_pEngineCommand->GetGraphicsEngine()->EndCommandContext(context, QueueType::Compute);
}

void EffectEditorUpdateSystem::UpdateShader()
{
	if (!m_pEngineCommand->m_ResourceManager->GetEffectRootUseListCount()) { return; }

	// バッファを取得
	IStructuredBuffer* useRootListBuffer = m_pEngineCommand->GetResourceManager()->GetEffectRootUseListBuffer();
	IStructuredBuffer* rootBuffer = m_pEngineCommand->GetResourceManager()->GetIntegrationBuffer(IntegrationDataType::EffectRootInt);
	IStructuredBuffer* nodeBuffer = m_pEngineCommand->GetResourceManager()->GetIntegrationBuffer(IntegrationDataType::EffectNodeInt);
	IStructuredBuffer* spriteBuffer = m_pEngineCommand->GetResourceManager()->GetIntegrationBuffer(IntegrationDataType::EffectSpriteInt);
	IRWStructuredBuffer* particleBuffer = m_pEngineCommand->GetResourceManager()->GetEffectParticleBuffer();
	IRWStructuredBuffer* particleListBuffer = m_pEngineCommand->GetResourceManager()->GetEffectParticleFreeListBuffer();

	// コマンドリスト取得
	CommandContext* context = m_pEngineCommand->GetGraphicsEngine()->GetCommandContext();
	// コマンドリスト開始
	m_pEngineCommand->GetGraphicsEngine()->BeginCommandContext(context);

	// 発生
	// パイプラインセット
	context->SetComputePipelineState(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectTimeBaseEmitPSO().pso.Get());
	// ルートシグネチャセット
	context->SetComputeRootSignature(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectTimeBaseEmitPSO().rootSignature.Get());
	// UseListをセット
	context->SetComputeRootDescriptorTable(0, useRootListBuffer->GetSRVGpuHandle());
	// Rootをセット
	context->SetComputeRootDescriptorTable(1, rootBuffer->GetSRVGpuHandle());
	// ノードバッファをセット
	context->SetComputeRootDescriptorTable(2, nodeBuffer->GetSRVGpuHandle());
	// スプライトバッファをセット
	context->SetComputeRootDescriptorTable(3, spriteBuffer->GetSRVGpuHandle());
	// Particleバッファをセット
	context->SetComputeRootDescriptorTable(4, particleBuffer->GetUAVGpuHandle());
	// ParticleListバッファをセット
	context->SetComputeRootDescriptorTable(5, particleListBuffer->GetUAVGpuHandle());
	// ListCounterバッファをセット
	context->SetComputeRootUnorderedAccessView(6, particleListBuffer->GetCounterResource()->GetGPUVirtualAddress());
	// Dispatch
	context->Dispatch(static_cast<UINT>(m_pEngineCommand->m_ResourceManager->GetEffectRootUseListCount()), 1, 1);

	// 並列阻止
	context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, particleBuffer->GetResource());
	context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, particleListBuffer->GetResource());

	// 更新
	// パイプラインセット
	context->SetComputePipelineState(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectTimeBaseUpdatePSO().pso.Get());
	// ルートシグネチャセット
	context->SetComputeRootSignature(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectTimeBaseUpdatePSO().rootSignature.Get());
	// Rootをセット
	context->SetComputeRootDescriptorTable(0, rootBuffer->GetSRVGpuHandle());
	// ノードバッファをセット
	context->SetComputeRootDescriptorTable(1, nodeBuffer->GetSRVGpuHandle());
	// スプライトバッファをセット
	context->SetComputeRootDescriptorTable(2, spriteBuffer->GetSRVGpuHandle());
	// Particleバッファをセット
	context->SetComputeRootDescriptorTable(3, particleBuffer->GetUAVGpuHandle());
	// ParticleListバッファをセット
	context->SetComputeRootDescriptorTable(4, particleListBuffer->GetUAVGpuHandle());
	// Dispatch
	context->Dispatch(128, 1, 1);
	// クローズ
	m_pEngineCommand->GetGraphicsEngine()->EndCommandContext(context, QueueType::Compute);
}

