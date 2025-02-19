#include "PrecompiledHeader.h"
#include "ParticleSystem.h"
#include"SystemState/SystemState.h"
#include"D3D12/D3DCommand/D3DCommand.h"
#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"Graphics/GraphicsSystem/GraphicsSystem.h"

void ParticleSystem::Initialize(EntityManager& entityManager, ComponentManager& componentManager)
{
	for (Entity entity : entityManager.GetActiveEntities()) {
		std::optional<std::reference_wrapper<ParticleComponent>> particleComp = componentManager.GetComponent<ParticleComponent>(entity);
		if (particleComp) {
			ParticleComponent& particle = particleComp.value();
			InitMatrix(particle);
		}
	}
}

void ParticleSystem::Update(EntityManager& entityManager, ComponentManager& componentManager)
{
	for (Entity entity : entityManager.GetActiveEntities()) {
		std::optional<std::reference_wrapper<ParticleComponent>> particleComp = componentManager.GetComponent<ParticleComponent>(entity);
		std::optional<std::reference_wrapper<EmitterComponent>> emitterComp = componentManager.GetComponent<EmitterComponent>(entity);
		if (particleComp && emitterComp) {
			ParticleComponent& particle = particleComp.value();
			EmitterComponent& emitter = emitterComp.value();
			UpdateMatrix(particle, emitter);
		}
	}

}

void ParticleSystem::SetPtrs(D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem)
{
	d3dCommand_ = d3dCommand;
	rvManager_ = rvManager;
	graphicsSystem_ = graphicsSystem;
}

void ParticleSystem::InitMatrix(ParticleComponent& particle)
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = d3dCommand_->GetCommand(CommandType::Compute).list.Get();

	commandList->SetComputeRootSignature(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::Init).rootSignature.Get());
	commandList->SetPipelineState(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::Init).Blend[kBlendModeNone].Get());
	commandList->SetComputeRootDescriptorTable(0, rvManager_->GetHandle(particle.uavIndex).GPUHandle);
	commandList->SetComputeRootDescriptorTable(1, rvManager_->GetHandle(particle.freeListIndex.uavIndex).GPUHandle);
	commandList->SetComputeRootDescriptorTable(2, rvManager_->GetHandle(particle.freeList.uavIndex).GPUHandle);
	commandList->Dispatch(1, 1, 1);
}

void ParticleSystem::UpdateMatrix(ParticleComponent& particle, EmitterComponent& emitter)
{
	particle.perFrame.time += DeltaTime();
	particle.perFrame.deltaTime = DeltaTime();

	// 転送
	TransferMatrix(particle);

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = d3dCommand_->GetCommand(CommandType::Compute).list.Get();

	/*Emit*/
	commandList->SetComputeRootSignature(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::Emit).rootSignature.Get());
	commandList->SetPipelineState(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::Emit).Blend[kBlendModeNone].Get());
	commandList->SetComputeRootDescriptorTable(0, rvManager_->GetHandle(particle.uavIndex).GPUHandle);
	commandList->SetComputeRootConstantBufferView(1, rvManager_->GetCBVResource(emitter.cbvIndex)->GetGPUVirtualAddress());
	commandList->SetComputeRootConstantBufferView(2, rvManager_->GetCBVResource(particle.perFrame.cbvIndex)->GetGPUVirtualAddress());
	commandList->SetComputeRootDescriptorTable(3, rvManager_->GetHandle(particle.freeListIndex.uavIndex).GPUHandle);
	commandList->SetComputeRootDescriptorTable(4, rvManager_->GetHandle(particle.freeList.uavIndex).GPUHandle);
	commandList->Dispatch(1, 1, 1);

	/*EmitとUpdateの並列を阻止*/
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.UAV.pResource = rvManager_->GetHandle(particle.uavIndex).resource.Get();
	commandList->ResourceBarrier(1, &barrier);

	D3D12_RESOURCE_BARRIER barrier3{};
	barrier3.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier3.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier3.UAV.pResource = rvManager_->GetHandle(particle.freeListIndex.uavIndex).resource.Get();
	commandList->ResourceBarrier(1, &barrier3);

	D3D12_RESOURCE_BARRIER barrier4{};
	barrier4.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier4.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier4.UAV.pResource = rvManager_->GetHandle(particle.freeList.uavIndex).resource.Get();
	commandList->ResourceBarrier(1, &barrier4);

	/*Update*/
	commandList->SetComputeRootSignature(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::ParticleUpdate).rootSignature.Get());
	commandList->SetPipelineState(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::ParticleUpdate).Blend[kBlendModeNone].Get());
	commandList->SetComputeRootDescriptorTable(0, rvManager_->GetHandle(particle.uavIndex).GPUHandle);
	commandList->SetComputeRootConstantBufferView(1, rvManager_->GetCBVResource(particle.perFrame.cbvIndex)->GetGPUVirtualAddress());
	commandList->SetComputeRootDescriptorTable(2, rvManager_->GetHandle(particle.freeListIndex.uavIndex).GPUHandle);
	commandList->SetComputeRootDescriptorTable(3, rvManager_->GetHandle(particle.freeList.uavIndex).GPUHandle);
	commandList->Dispatch(1, 1, 1);
}

void ParticleSystem::TransferMatrix(ParticleComponent& particle)
{
	particle.perFrame.constData->time = particle.perFrame.time;
	particle.perFrame.constData->deltaTime = particle.perFrame.deltaTime;
}
