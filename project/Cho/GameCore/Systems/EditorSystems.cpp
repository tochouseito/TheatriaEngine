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
	transform.matWorld = ChoMath::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);

	// 次のフレーム用に保存する
	transform.prePos = transform.translation;
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
	transform.matWorld = ChoMath::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);
	TransferMatrix(transform, camera);
}

void CameraEditorSystem::TransferMatrix(TransformComponent& transform, CameraComponent& camera)
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
	m_pIntegrationBuffer->UpdateData(data, material.mapID.value());
}

void EmitterEditorUpdateSystem::UpdateEmitter(EmitterComponent& emitter)
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
		emitter.emit = 0;
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

void EffectEditorUpdateSystem::UpdateEffect(EffectComponent& effect)
{
	if (effect.isReset)
	{
		// エフェクトの時間をリセット
		effect.globalTime = 0.0f;
		effect.deltaTime = 0.0f;
		effect.isRun = false;
		// 初期化
		CommandContext* context = m_pEngineCommand->GetGraphicsEngine()->GetCommandContext();
		// コマンドリスト開始
		m_pEngineCommand->GetGraphicsEngine()->BeginCommandContext(context);
		// パイプラインセット
		context->SetComputePipelineState(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectEditorInitPSO().pso.Get());
		// ルートシグネチャセット
		context->SetComputeRootSignature(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectEditorInitPSO().rootSignature.Get());
		// Particleバッファをセット
		IRWStructuredBuffer* particleBuffer = m_pEngineCommand->GetResourceManager()->GetBuffer<IRWStructuredBuffer>(m_pEngineCommand->m_EffectParticleIndex);
		context->SetComputeRootDescriptorTable(0, particleBuffer->GetUAVGpuHandle());
		// ParticleListバッファをセット
		IRWStructuredBuffer* particleListBuffer = m_pEngineCommand->GetResourceManager()->GetBuffer<IRWStructuredBuffer>(m_pEngineCommand->m_EffectParticleFreeListIndex);
		context->SetComputeRootDescriptorTable(1, particleListBuffer->GetUAVGpuHandle());
		// ListCounterバッファをセット
		context->SetComputeRootUnorderedAccessView(2, particleListBuffer->GetCounterResource()->GetGPUVirtualAddress());
		// Dispatch
		context->Dispatch(128, 1, 1);
		// クローズ
		m_pEngineCommand->GetGraphicsEngine()->EndCommandContext(context, QueueType::Compute);
		// 待機
		m_pEngineCommand->GetGraphicsEngine()->WaitForGPU(QueueType::Compute);
		effect.isReset = false;
	}
	// editorのエフェクトの更新
	if (effect.isRun)
	{
		// エフェクトの時間を更新
		effect.deltaTime = Timer::GetDeltaTime();
		effect.globalTime += effect.deltaTime;
		if (!effect.isPreRun||effect.globalTime > effect.maxTime)
		{
			// 初期化
			CommandContext* context = m_pEngineCommand->GetGraphicsEngine()->GetCommandContext();
			// コマンドリスト開始
			m_pEngineCommand->GetGraphicsEngine()->BeginCommandContext(context);
			// パイプラインセット
			context->SetComputePipelineState(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectEditorInitPSO().pso.Get());
			// ルートシグネチャセット
			context->SetComputeRootSignature(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectEditorInitPSO().rootSignature.Get());
			// Particleバッファをセット
			IRWStructuredBuffer* particleBuffer = m_pEngineCommand->GetResourceManager()->GetBuffer<IRWStructuredBuffer>(m_pEngineCommand->m_EffectParticleIndex);
			context->SetComputeRootDescriptorTable(0, particleBuffer->GetUAVGpuHandle());
			// ParticleListバッファをセット
			IRWStructuredBuffer* particleListBuffer = m_pEngineCommand->GetResourceManager()->GetBuffer<IRWStructuredBuffer>(m_pEngineCommand->m_EffectParticleFreeListIndex);
			context->SetComputeRootDescriptorTable(1, particleListBuffer->GetUAVGpuHandle());
			// ListCounterバッファをセット
			context->SetComputeRootUnorderedAccessView(2, particleListBuffer->GetCounterResource()->GetGPUVirtualAddress());
			// Dispatch
			context->Dispatch(128, 1, 1);
			// クローズ
			m_pEngineCommand->GetGraphicsEngine()->EndCommandContext(context, QueueType::Compute);
			// 待機
			m_pEngineCommand->GetGraphicsEngine()->WaitForGPU(QueueType::Compute);

			if (effect.isPreRun)
			{
				if (effect.isLoop)
				{
					effect.globalTime = 0.0f;
				} else
				{
					effect.globalTime = 0.0f;
					effect.isRun = false;
				}
			}
		}
		effect.isPreRun = effect.isRun;
	} else
	{
		return;
	}
	// ノードがないならスキップ
	if (effect.nodeID.empty()) { return; }
	// Root
	ConstantBuffer<EffectRoot>* buffer = dynamic_cast<ConstantBuffer<EffectRoot>*>(m_pEngineCommand->GetResourceManager()->GetBuffer<IConstantBuffer>(m_pEngineCommand->m_EffectRootIndex));
	EffectRoot root = {};
	root.timeManager.globalTime = effect.globalTime;
	root.timeManager.maxTime = effect.maxTime;
	root.timeManager.deltaTime = effect.deltaTime;
	root.isRun = effect.isRun;
	root.isLoop = effect.isLoop;
	StructuredBuffer<EffectNode>* nodeBuffer = dynamic_cast<StructuredBuffer<EffectNode>*>(m_pEngineCommand->GetResourceManager()->GetBuffer<IStructuredBuffer>(m_pEngineCommand->m_EffectNodeIndex));
	StructuredBuffer<EffectSprite>* spriteBuffer = dynamic_cast<StructuredBuffer<EffectSprite>*>(m_pEngineCommand->GetResourceManager()->GetBuffer<IStructuredBuffer>(m_pEngineCommand->m_EffectSpriteIndex));
	for (size_t i = 0; i < effect.nodeID.size(); i++)
	{
		root.nodeID[i] = effect.nodeID[i];
		EffectNode node = {};
		node.common = effect.nodeData[i].common;
		node.position = effect.nodeData[i].position;
		node.rotation = effect.nodeData[i].rotation;
		node.scale = effect.nodeData[i].scale;
		node.drawCommon = effect.nodeData[i].drawCommon;
		node.draw = effect.nodeData[i].draw;
		node.parentIndex = effect.nodeData[i].parentIndex;
		nodeBuffer->UpdateData(node, effect.nodeData[i].id);
		EffectSprite sprite = {};
		sprite.colorType = effect.nodeData[i].sprite.colorType;
		sprite.color = effect.nodeData[i].sprite.color;
		sprite.randColor = effect.nodeData[i].sprite.randColor;
		sprite.easingColor = effect.nodeData[i].sprite.easingColor;
		sprite.placement = effect.nodeData[i].sprite.placement;
		sprite.VertexColorType = effect.nodeData[i].sprite.VertexColorType;
		sprite.vertexColor = effect.nodeData[i].sprite.vertexColor;
		sprite.VertexPositionType = effect.nodeData[i].sprite.VertexPositionType;
		sprite.vertexPosition = effect.nodeData[i].sprite.vertexPosition;
		spriteBuffer->UpdateData(sprite, effect.nodeData[i].draw.meshDataIndex);
	}
	buffer->UpdateData(root);

	// Emit
	CommandContext* context = m_pEngineCommand->GetGraphicsEngine()->GetCommandContext();
	// コマンドリスト開始
	m_pEngineCommand->GetGraphicsEngine()->BeginCommandContext(context);
	// パイプラインセット
	context->SetComputePipelineState(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectEditorEmitPSO().pso.Get());
	// ルートシグネチャセット
	context->SetComputeRootSignature(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectEditorEmitPSO().rootSignature.Get());
	// Rootをセット
	context->SetComputeRootConstantBufferView(0, buffer->GetResource()->GetGPUVirtualAddress());
	// ノードバッファをセット
	context->SetComputeRootDescriptorTable(1, nodeBuffer->GetSRVGpuHandle());
	// スプライトバッファをセット
	context->SetComputeRootDescriptorTable(2, spriteBuffer->GetSRVGpuHandle());
	// Particleバッファをセット
	IRWStructuredBuffer* particleBuffer = m_pEngineCommand->GetResourceManager()->GetBuffer<IRWStructuredBuffer>(m_pEngineCommand->m_EffectParticleIndex);
	context->SetComputeRootDescriptorTable(3, particleBuffer->GetUAVGpuHandle());
	// ParticleListバッファをセット
	IRWStructuredBuffer* particleListBuffer = m_pEngineCommand->GetResourceManager()->GetBuffer<IRWStructuredBuffer>(m_pEngineCommand->m_EffectParticleFreeListIndex);
	context->SetComputeRootDescriptorTable(4, particleListBuffer->GetUAVGpuHandle());
	// ListCounterバッファをセット
	context->SetComputeRootUnorderedAccessView(5, particleListBuffer->GetCounterResource()->GetGPUVirtualAddress());
	// Dispatch
	context->Dispatch(static_cast<UINT>(effect.nodeID.size()), 1, 1);

	// 並列阻止
	context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, particleBuffer->GetResource());
	context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, particleListBuffer->GetResource());

	// Update
	// パイプラインセット
	context->SetComputePipelineState(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectEditorUpdatePSO().pso.Get());
	// ルートシグネチャセット
	context->SetComputeRootSignature(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectEditorUpdatePSO().rootSignature.Get());
	// Rootをセット
	context->SetComputeRootConstantBufferView(0, buffer->GetResource()->GetGPUVirtualAddress());
	// ノードバッファをセット
	context->SetComputeRootDescriptorTable(1, nodeBuffer->GetSRVGpuHandle());
	// スプライトバッファをセット
	context->SetComputeRootDescriptorTable(2, spriteBuffer->GetSRVGpuHandle());
	// Particleバッファをセット
	context->SetComputeRootDescriptorTable(3, particleBuffer->GetUAVGpuHandle());
	// ParticleListバッファをセット
	context->SetComputeRootDescriptorTable(4, particleListBuffer->GetUAVGpuHandle());
	// Dispatch
	context->Dispatch(static_cast<UINT>(effect.nodeID.size()), 1, 1);
	// クローズ
	m_pEngineCommand->GetGraphicsEngine()->EndCommandContext(context, QueueType::Compute);
	// 待機
	m_pEngineCommand->GetGraphicsEngine()->WaitForGPU(QueueType::Compute);
}
