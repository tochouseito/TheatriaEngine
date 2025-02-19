#include "PrecompiledHeader.h"
#include "EffectSystem.h"
#include"D3D12/D3DCommand/D3DCommand.h"
#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"Graphics/GraphicsSystem/GraphicsSystem.h"

void EffectSystem::Initialize(EntityManager& entityManager, ComponentManager& componentManager)
{
	for (Entity entity : entityManager.GetActiveEntities()) {
		if (componentManager.GetComponent<EffectComponent>(entity)) {
			EffectComponent& effect = componentManager.GetComponent<EffectComponent>(entity).value();

			InitMatrix(effect);
		}
	}
}

void EffectSystem::Update(EntityManager& entityManager, ComponentManager& componentManager)
{
	for (Entity entity : entityManager.GetActiveEntities()) {
		if (componentManager.GetComponent<EffectComponent>(entity)) {
			EffectComponent& effect = componentManager.GetComponent<EffectComponent>(entity).value();
			if (effect.effectNodes.size() == 0) {
				continue;
			}

			if (effect.isInit) {
				InitMatrix(effect);
				effect.isInit = false;
				if (!effect.isLoop) {
					effect.isEnd = false;
				}
			}
			else {
				UpdateMatrix(effect);
			}
		}
	}
}

void EffectSystem::SetPtrs(D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem)
{
	d3dCommand_ = d3dCommand;
	rvManager_ = rvManager;
	graphicsSystem_ = graphicsSystem;
}

void EffectSystem::InitMatrix(EffectComponent& effect)
{
	for (EffectNode& node : effect.effectNodes) {

		InitDispatch(node);

		LoopEffectNodeInit(node, effect);
	}
	effect.timeManager->globalTime = 0.0f;
	effect.timeManager->preTime = 0.0f;
	TransferMatrix(effect);
}

void EffectSystem::UpdateMatrix(EffectComponent& effect)
{
	if (effect.isRun) {
		effect.timeManager->globalTime++;
	}
	if (effect.timeManager->globalTime != effect.timeManager->preTime) {
		for (EffectNode& node : effect.effectNodes) {

			UpdateDispatch(node, effect);

			LoopEffectNodeUpdate(node, effect);
		}

		if (effect.timeManager->globalTime > effect.timeManager->maxTime) {
			effect.isInit = true;
			
			if (!effect.isLoop) {
				effect.isRun = false;
				effect.isEnd = true;
			}
		}

	}

	effect.timeManager->preTime = effect.timeManager->globalTime;

	// 転送
	TransferMatrix(effect);
}

void EffectSystem::TransferMatrix(EffectComponent& effect)
{
	for (EffectNode& node : effect.effectNodes) {

		TransferEffectNode(node);

		LoopEffectNodeTransfer(node, effect);
	}
}

void EffectSystem::LoopEffectNodeInit(EffectNode& effectNode, EffectComponent& effect)
{
	for (EffectNode& node : effectNode.children) {

		InitDispatch(node);

		LoopEffectNodeInit(node, effect);
	}
}

void EffectSystem::LoopEffectNodeUpdate(EffectNode& effectNode, EffectComponent& effect)
{
	for (EffectNode& node : effectNode.children) {

		UpdateDispatch(node,effect);

		LoopEffectNodeUpdate(node, effect);
	}
}

void EffectSystem::LoopEffectNodeTransfer(EffectNode& effectNode, EffectComponent& effect)
{
	for (EffectNode& node : effectNode.children) {

		TransferEffectNode(node);

		LoopEffectNodeTransfer(node, effect);
	}
}

void EffectSystem::TransferEffectNode(EffectNode& node)
{
	// Common
	node.constData->common.maxCount = node.common.maxCount;
	node.constData->common.emitCount = node.common.emitCount;
	node.constData->common.PosInfluenceType = node.common.PosInfluenceType;
	node.constData->common.RotInfluenceType = node.common.RotInfluenceType;
	node.constData->common.SclInfluenceType = node.common.SclInfluenceType;
	node.constData->common.deleteLifetime = node.common.deleteLifetime;
	node.constData->common.deleteParentDeleted = node.common.deleteParentDeleted;
	node.constData->common.deleteAllChildrenDeleted = node.common.deleteAllChildrenDeleted;
	node.constData->common.lifeTime.isMedian = node.common.lifeTime.isMedian;
	node.constData->common.lifeTime.randValue.first = node.common.lifeTime.randValue.first;
	node.constData->common.lifeTime.randValue.second = node.common.lifeTime.randValue.second;
	node.constData->common.lifeTime.lifeTime = node.common.lifeTime.lifeTime;
	node.constData->common.emitTime.isMedian = node.common.emitTime.isMedian;
	node.constData->common.emitTime.randValue.first = node.common.emitTime.randValue.first;
	node.constData->common.emitTime.randValue.second = node.common.emitTime.randValue.second;
	node.constData->common.emitTime.emitTime = node.common.emitTime.emitTime;
	node.constData->common.emitStartTime.isMedian = node.common.emitStartTime.isMedian;
	node.constData->common.emitStartTime.randValue.first = node.common.emitStartTime.randValue.first;
	node.constData->common.emitStartTime.randValue.second = node.common.emitStartTime.randValue.second;
	node.constData->common.emitStartTime.emitStartTime = node.common.emitStartTime.emitStartTime;

	// Position
	node.constData->position.type = node.position.type;
	node.constData->position.value = node.position.value;
	node.constData->position.pva.value.isMedian = node.position.pva.value.isMedian;
	node.constData->position.pva.value.first = node.position.pva.value.first;
	node.constData->position.pva.value.second = node.position.pva.value.second;
	node.constData->position.pva.velocity.isMedian = node.position.pva.velocity.isMedian;
	node.constData->position.pva.velocity.first = node.position.pva.velocity.first;
	node.constData->position.pva.velocity.second = node.position.pva.velocity.second;
	node.constData->position.pva.acceleration.isMedian = node.position.pva.acceleration.isMedian;
	node.constData->position.pva.acceleration.first = node.position.pva.acceleration.first;
	node.constData->position.pva.acceleration.second = node.position.pva.acceleration.second;

	// Rotation
	node.constData->rotation.type = node.rotation.type;
	node.constData->rotation.value = node.rotation.value;
	node.constData->rotation.pva.value.isMedian = node.rotation.pva.value.isMedian;
	node.constData->rotation.pva.value.first = node.rotation.pva.value.first;
	node.constData->rotation.pva.value.second = node.rotation.pva.value.second;
	node.constData->rotation.pva.velocity.isMedian = node.rotation.pva.velocity.isMedian;
	node.constData->rotation.pva.velocity.first = node.rotation.pva.velocity.first;
	node.constData->rotation.pva.velocity.second = node.rotation.pva.velocity.second;
	node.constData->rotation.pva.acceleration.isMedian = node.rotation.pva.acceleration.isMedian;
	node.constData->rotation.pva.acceleration.first = node.rotation.pva.acceleration.first;
	node.constData->rotation.pva.acceleration.second = node.rotation.pva.acceleration.second;

	// Scale
	node.constData->scale.type = node.scale.type;
	node.constData->scale.value = node.scale.value;
	node.constData->scale.pva.value.isMedian = node.scale.pva.value.isMedian;
	node.constData->scale.pva.value.first = node.scale.pva.value.first;
	node.constData->scale.pva.value.second = node.scale.pva.value.second;
	node.constData->scale.pva.velocity.isMedian = node.scale.pva.velocity.isMedian;
	node.constData->scale.pva.velocity.first = node.scale.pva.velocity.first;
	node.constData->scale.pva.velocity.second = node.scale.pva.velocity.second;
	node.constData->scale.pva.acceleration.isMedian = node.scale.pva.acceleration.isMedian;
	node.constData->scale.pva.acceleration.first = node.scale.pva.acceleration.first;
	node.constData->scale.pva.acceleration.second = node.scale.pva.acceleration.second;

	// DrawCommon
	node.constData->drawCommon.emissive = node.drawCommon.emissiveMagnification;
	node.constData->drawCommon.fadeOutType = node.drawCommon.fadeOutType;

	// Draw
	node.constData->draw.meshType = node.draw.meshType;
	node.constData->draw.meshSprite.color = node.draw.meshSprite.color;
	node.constData->draw.meshSprite.placement = node.draw.meshSprite.placement;
}

void EffectSystem::InitDispatch(EffectNode& effectNode)
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = d3dCommand_->GetCommand(CommandType::Compute).list.Get();

	commandList->SetComputeRootSignature(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::EffectInit).rootSignature.Get());
	commandList->SetPipelineState(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::EffectInit).Blend[kBlendModeNone].Get());
	commandList->SetComputeRootDescriptorTable(0, rvManager_->GetHandle(effectNode.particle.particleUAVIndex).GPUHandle);
	commandList->SetComputeRootDescriptorTable(1, rvManager_->GetHandle(effectNode.particle.freeListIndex.uavIndex).GPUHandle);
	commandList->SetComputeRootDescriptorTable(2, rvManager_->GetHandle(effectNode.particle.freeList.uavIndex).GPUHandle);
	commandList->Dispatch(1, 1, 1);
}

void EffectSystem::UpdateDispatch(EffectNode& effectNode, EffectComponent& effect)
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = d3dCommand_->GetCommand(CommandType::Compute).list.Get();

	commandList->SetComputeRootSignature(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::EffectEmit).rootSignature.Get());
	commandList->SetPipelineState(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::EffectEmit).Blend[kBlendModeNone].Get());
	commandList->SetComputeRootConstantBufferView(0, rvManager_->GetCBVResource(effect.timeManagerCBVIndex)->GetGPUVirtualAddress());
	commandList->SetComputeRootDescriptorTable(1, rvManager_->GetHandle(effectNode.srvIndex).GPUHandle);
	commandList->SetComputeRootDescriptorTable(2, rvManager_->GetHandle(effectNode.particle.particleUAVIndex).GPUHandle);
	commandList->SetComputeRootDescriptorTable(3, rvManager_->GetHandle(effectNode.particle.freeListIndex.uavIndex).GPUHandle);
	commandList->SetComputeRootDescriptorTable(4, rvManager_->GetHandle(effectNode.particle.freeList.uavIndex).GPUHandle);
	commandList->Dispatch(1, 1, 1);

	/*EmitとUpdateの並列を阻止*/
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.UAV.pResource = rvManager_->GetHandle(effectNode.particle.particleUAVIndex).resource.Get();
	commandList->ResourceBarrier(1, &barrier);

	D3D12_RESOURCE_BARRIER barrier2{};
	barrier2.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier2.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier2.UAV.pResource = rvManager_->GetHandle(effectNode.particle.freeListIndex.uavIndex).resource.Get();
	commandList->ResourceBarrier(1, &barrier2);

	D3D12_RESOURCE_BARRIER barrier3{};
	barrier3.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier3.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier3.UAV.pResource = rvManager_->GetHandle(effectNode.particle.freeList.uavIndex).resource.Get();
	commandList->ResourceBarrier(1, &barrier3);

	commandList->SetComputeRootSignature(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::EffectUpdate).rootSignature.Get());
	commandList->SetPipelineState(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::EffectUpdate).Blend[kBlendModeNone].Get());
	commandList->SetComputeRootConstantBufferView(0, rvManager_->GetCBVResource(effect.timeManagerCBVIndex)->GetGPUVirtualAddress());
	commandList->SetComputeRootDescriptorTable(1, rvManager_->GetHandle(effectNode.srvIndex).GPUHandle);
	commandList->SetComputeRootDescriptorTable(2, rvManager_->GetHandle(effectNode.particle.particleUAVIndex).GPUHandle);
	commandList->SetComputeRootDescriptorTable(3, rvManager_->GetHandle(effectNode.particle.freeListIndex.uavIndex).GPUHandle);
	commandList->SetComputeRootDescriptorTable(4, rvManager_->GetHandle(effectNode.particle.freeList.uavIndex).GPUHandle);
	commandList->Dispatch(1, 1, 1);

}
