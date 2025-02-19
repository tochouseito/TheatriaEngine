#include "PrecompiledHeader.h"
#include "RenderSystem.h"

#include "D3D12/D3DCommand/D3DCommand.h"

#include "D3D12/ResourceViewManager/ResourceViewManager.h"
#include "D3D12/DSVManager/DSVManager.h"
#include "D3D12/DrawExecution/DrawExecution.h"
#include "Graphics/GraphicsSystem/GraphicsSystem.h"
#include "Load/TextureLoader/TextureLoader.h"

#include "ChoMath.h"
#include "SystemState/SystemState.h"

void RenderSystem::GBufferRender(PSOMode psoMode, EntityManager& entityManager, ComponentManager& componentManager, D3DCommand* d3dCommand, ResourceViewManager* rvManager,DSVManager* dsvManager, GraphicsSystem* graphicsSystem, TextureLoader* texLoad)
{
    ObjectRender(psoMode,entityManager, componentManager, d3dCommand, rvManager,dsvManager, graphicsSystem, texLoad);
    //ParticleRender(entityManager, componentManager, d3dCommand, rvManager, graphicsSystem, texLoad);
}

void RenderSystem::Render(
    PSOMode psoMode,
    EntityManager& entityManager,
    ComponentManager& componentManager,
    D3DCommand* d3dCommand,
    ResourceViewManager* rvManager,
    GraphicsSystem* graphicsSystem,
    TextureLoader* texLoad
)
{
    psoMode;
	ObjectRender(psoMode, entityManager, componentManager, d3dCommand, rvManager,nullptr, graphicsSystem, texLoad);
	ParticleRender(entityManager, componentManager, d3dCommand, rvManager, graphicsSystem, texLoad);
	EffectRender(entityManager, componentManager, d3dCommand, rvManager, graphicsSystem, texLoad);
    SpriteRender(entityManager, componentManager, d3dCommand, rvManager, graphicsSystem, texLoad);
}

void RenderSystem::DebugRenderGBufferMix(EntityManager& entityManager, ComponentManager& componentManager, D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem, TextureLoader* texLoad,const CameraComponent& camera)
{
    ID3D12GraphicsCommandList6* commandList = d3dCommand->GetCommand(CommandType::Draw).list.Get();

    // 全てのエンティティ
    for (Entity entity : entityManager.GetActiveEntities()) {
        std::optional<std::reference_wrapper<RenderComponent>> renderComp = componentManager.GetComponent<RenderComponent>(entity);
        std::optional<std::reference_wrapper<MeshComponent>> meshComp = componentManager.GetComponent<MeshComponent>(entity);
        std::optional<std::reference_wrapper<MaterialComponent>> materialComp = componentManager.GetComponent<MaterialComponent>(entity);
        std::optional<std::reference_wrapper<TransformComponent>> transformComp = componentManager.GetComponent<TransformComponent>(entity);

        if (renderComp && meshComp && transformComp) {
            RenderComponent& render = renderComp.value();
            if (render.visible) {
                MeshComponent& mesh = meshComp.value();
                TransformComponent& transform = transformComp.value();
                // 頂点データ取得キー
                uint32_t meshesIndex;
                if (rvManager->GetModelData(mesh.meshesName)) {
                    meshesIndex = rvManager->GetModelData(mesh.meshesName)->meshIndex;
                }
                else {
                    meshesIndex = mesh.meshID;
                }
                //MeshShaderRenderTest(entityManager, componentManager, d3dCommand, rvManager, graphicsSystem, texLoad);
                // 描画処理: 描画コンポーネントに基づきリソースをバインドして描画
                if (transform.isInstance) { render.wireframe = false; }
                if (render.wireframe) {
                    commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Wireframe).rootSignature.Get());
                    commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Wireframe).Blend[kBlendModeNone].Get());
                }
                else {
                    if (transform.isInstance) {
                        commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::MapChip).rootSignature.Get());
                        commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::MapChip).Blend[kBlendModeNone].Get());
                    }
                    else {
                        commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Demo).rootSignature.Get());
                        commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Demo).Blend[kBlendModeNone].Get());
                    }
                }
                for (auto& meshData : rvManager->GetMesh(meshesIndex)->meshData) {
                    if (meshData.isAnimation) {
                        ModelData* model = rvManager->GetModelData(rvManager->GetMesh(meshesIndex)->meshesName);
                        // 頂点バッファビューをセット
                        commandList->IASetVertexBuffers(0, 1, &rvManager->GetMeshViewData(model->skinCluster.skinningData.outputMVIndex)->vbvData.vbv);
                    }
                    else {
                        // 頂点バッファビューをセット
                        commandList->IASetVertexBuffers(0, 1, &rvManager->GetMeshViewData(meshData.meshViewIndex)->vbvData.vbv);
                    }
                    // インデックスバッファビューをセット
                    commandList->IASetIndexBuffer(&rvManager->GetMeshViewData(meshData.meshViewIndex)->ibvData.ibv);

                    // ルートパラメータをセット
                    if (transform.isInstance) {
                        commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(camera.cbvIndex)->GetGPUVirtualAddress());
                        commandList->SetGraphicsRootDescriptorTable(1, rvManager->GetHandle(transform.srvIndex).GPUHandle);
                    }
                    else {
                        commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(transform.cbvIndex)->GetGPUVirtualAddress());
                        commandList->SetGraphicsRootConstantBufferView(1, rvManager->GetCBVResource(camera.cbvIndex)->GetGPUVirtualAddress());
                    }
                    if (materialComp && !render.wireframe) {
                        MaterialComponent& material = materialComp.value();
                        ModelData* model = rvManager->GetModelData(rvManager->GetMesh(meshesIndex)->meshesName);
                        if (model) {
                            std::string texName = rvManager->GetModelData(rvManager->GetMesh(meshesIndex)->meshesName)->objects[meshData.name].material.textureName;
                            commandList->SetGraphicsRootConstantBufferView(2, rvManager->GetCBVResource(material.cbvIndex)->GetGPUVirtualAddress());
                            commandList->SetGraphicsRootDescriptorTable(3, rvManager->GetHandle(texLoad->GetTexture(texName).rvIndex).GPUHandle);
                        }
                        else {
                            commandList->SetGraphicsRootConstantBufferView(2, rvManager->GetCBVResource(material.cbvIndex)->GetGPUVirtualAddress());
                            commandList->SetGraphicsRootDescriptorTable(3, rvManager->GetHandle(texLoad->GetTexture(material.textureID).rvIndex).GPUHandle);
                        }
                    }
                    // 描画
                    if (transform.isInstance) {
                        commandList->DrawIndexedInstanced(static_cast<UINT>(meshData.size.indices), transform.instanceCount, 0, 0, 0);
                    }
                    else {
                        commandList->DrawIndexedInstanced(static_cast<UINT>(meshData.size.indices), 1, 0, 0, 0);
                    }
                }
            }
        }
    }
}

void RenderSystem::DebugRender(
    EntityManager& entityManager,
    ComponentManager& componentManager,
    D3DCommand* d3dCommand,
    ResourceViewManager* rvManager,
    GraphicsSystem* graphicsSystem,
    TextureLoader* texLoad,
    const CameraComponent& camera
)
{
    ID3D12GraphicsCommandList6* commandList = d3dCommand->GetCommand(CommandType::Draw).list.Get();
    
    // 全てのエンティティ
    for (Entity entity : entityManager.GetActiveEntities()) {
        std::optional<std::reference_wrapper<RenderComponent>> renderComp = componentManager.GetComponent<RenderComponent>(entity);
        std::optional<std::reference_wrapper<MeshComponent>> meshComp = componentManager.GetComponent<MeshComponent>(entity);
        std::optional<std::reference_wrapper<MaterialComponent>> materialComp = componentManager.GetComponent<MaterialComponent>(entity);
        std::optional<std::reference_wrapper<TransformComponent>> transformComp = componentManager.GetComponent<TransformComponent>(entity);
        
        if (renderComp && meshComp && transformComp&&materialComp) {
            RenderComponent& render = renderComp.value();
            if (render.visible) {
                MeshComponent& mesh = meshComp.value();
				TransformComponent& transform = transformComp.value();
                // 頂点データ取得キー
                uint32_t meshesIndex;
                if (rvManager->GetModelData(mesh.meshesName)) {
                    meshesIndex = rvManager->GetModelData(mesh.meshesName)->meshIndex;
                }
                else {
                    meshesIndex = mesh.meshID;
                }
                //MeshShaderRenderTest(entityManager, componentManager, d3dCommand, rvManager, graphicsSystem, texLoad);
                // 描画処理: 描画コンポーネントに基づきリソースをバインドして描画
				if (transform.isInstance) { render.wireframe = false; }
                if (render.wireframe) {
                    commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Wireframe).rootSignature.Get());
                    commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Wireframe).Blend[kBlendModeNone].Get());
                }
                else {
					if (transform.isInstance) {
						commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::MapChip).rootSignature.Get());
						commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::MapChip).Blend[kBlendModeNone].Get());
					}
                    else {
                        commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Demo).rootSignature.Get());
                        commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Demo).Blend[kBlendModeNone].Get());
                    }
                }
                for (auto& meshData : rvManager->GetMesh(meshesIndex)->meshData) {
                    if (meshData.isAnimation) {
                        ModelData* model = rvManager->GetModelData(rvManager->GetMesh(meshesIndex)->meshesName);
                        // 頂点バッファビューをセット
                        commandList->IASetVertexBuffers(0, 1, &rvManager->GetMeshViewData(model->skinCluster.skinningData.outputMVIndex)->vbvData.vbv);
                    }
                    else {
                        // 頂点バッファビューをセット
                        commandList->IASetVertexBuffers(0, 1, &rvManager->GetMeshViewData(meshData.meshViewIndex)->vbvData.vbv);
                    }
                    // インデックスバッファビューをセット
                    commandList->IASetIndexBuffer(&rvManager->GetMeshViewData(meshData.meshViewIndex)->ibvData.ibv);

                    // ルートパラメータをセット
                    if (transform.isInstance) {
                        commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(camera.cbvIndex)->GetGPUVirtualAddress());
						commandList->SetGraphicsRootDescriptorTable(1, rvManager->GetHandle(transform.srvIndex).GPUHandle);
                    }
                    else {
                        commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(transform.cbvIndex)->GetGPUVirtualAddress());
                        commandList->SetGraphicsRootConstantBufferView(1, rvManager->GetCBVResource(camera.cbvIndex)->GetGPUVirtualAddress());
                    }
                    if (materialComp && !render.wireframe) {
                        MaterialComponent& material = materialComp.value();
                        ModelData* model = rvManager->GetModelData(rvManager->GetMesh(meshesIndex)->meshesName);
                        if (model) {
                            std::string texName = rvManager->GetModelData(rvManager->GetMesh(meshesIndex)->meshesName)->objects[meshData.name].material.textureName;
							commandList->SetGraphicsRootConstantBufferView(2, rvManager->GetCBVResource(material.cbvIndex)->GetGPUVirtualAddress());
                            commandList->SetGraphicsRootDescriptorTable(3, rvManager->GetHandle(texLoad->GetTexture(texName).rvIndex).GPUHandle);
                        }
                        else {
							commandList->SetGraphicsRootConstantBufferView(2, rvManager->GetCBVResource(material.cbvIndex)->GetGPUVirtualAddress());
                            commandList->SetGraphicsRootDescriptorTable(3, rvManager->GetHandle(texLoad->GetTexture(material.textureID).rvIndex).GPUHandle);
                        }
                    }
					// 描画
                    if (transform.isInstance) {
						commandList->DrawIndexedInstanced(static_cast<UINT>(meshData.size.indices), transform.instanceCount, 0, 0, 0);
					}
                    else {
                        commandList->DrawIndexedInstanced(static_cast<UINT>(meshData.size.indices), 1, 0, 0, 0);
                    }
                }
            }
        }
    }
	DebugDrawEffect(entityManager, componentManager, d3dCommand, rvManager, graphicsSystem, texLoad,camera);
    // ライン描画
	ColliderRender(entityManager, componentManager, d3dCommand, rvManager, graphicsSystem, camera);
	DrawGrid(entityManager, componentManager, d3dCommand, rvManager, graphicsSystem, camera);
	//DebugLineRender(entityManager, componentManager, d3dCommand, rvManager, graphicsSystem, texLoad, camera);
}

void RenderSystem::PostRender(EntityManager& entityManager, ComponentManager& componentManager, D3DCommand* d3dCommand, ResourceViewManager* rvManager)
{
    // 全てのエンティティ
    for (Entity entity : entityManager.GetActiveEntities()) {
		std::optional<std::reference_wrapper<MeshComponent>> meshComp = componentManager.GetComponent<MeshComponent>(entity);
		if (!meshComp) {
			continue;
		}
		MeshComponent& mesh = meshComp.value();
        //AnimationComponent& animation = componentManager.GetAnimation(entity);
        ModelData* model = rvManager->GetModelData(mesh.meshesName);
        // 頂点データ取得キー
        uint32_t meshesIndex;
        if (model) {
            meshesIndex = model->meshIndex;
        }
        else {
            continue;
            //meshesIndex = mesh->meshID;
        }
        //if (!model->animations.empty()) {
        //    for (auto& meshData : rvManager->GetMesh(meshesIndex)->meshData) {
        //        if (meshData.isAnimation) {
        //            // リソース遷移
        //            d3dCommand->BarrierTransition(
        //                CommandType::Draw,
        //                rvManager->GetHandle(
        //                    model->skinCluster.skinningData.outputUAVIndex).resource.Get(),
        //                D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
        //                D3D12_RESOURCE_STATE_UNORDERED_ACCESS

        //            );
        //        }
        //    }
        //}
    }
    for (auto& model : rvManager->GetModels()) {
        if (model.second->isBone) {
            // リソース遷移
            d3dCommand->BarrierTransition(
                CommandType::Draw,
                rvManager->GetHandle(
                    model.second->skinCluster.skinningData.outputUAVIndex).resource.Get(),
                D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
                D3D12_RESOURCE_STATE_UNORDERED_ACCESS

            );
        }
    }
}

void RenderSystem::ObjectRender(PSOMode psoMode, EntityManager& entityManager, ComponentManager& componentManager, D3DCommand* d3dCommand, ResourceViewManager* rvManager, DSVManager* dsvManager,GraphicsSystem* graphicsSystem, TextureLoader* texLoad)
{
    ID3D12GraphicsCommandList6* commandList = d3dCommand->GetCommand(CommandType::Draw).list.Get();

    // 今セットされているカメラのコンポーネント
    if (!componentManager.GetComponent<CameraComponent>(entityManager.GetNowCameraEntity())) {
        return;
    }
    CameraComponent& camera = componentManager.GetComponent<CameraComponent>(entityManager.GetNowCameraEntity()).value();

    if (psoMode == PSOMode::GBufferMix||psoMode==PSOMode::Dif_ToonLighting) {
		if (psoMode == PSOMode::GBufferMix)
		{
			commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::GBufferMix).rootSignature.Get());
			commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::GBufferMix).Blend[kBlendModeNormal].Get());
		}
		else if (psoMode == PSOMode::Dif_ToonLighting)
		{
			commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Dif_ToonLighting).rootSignature.Get());
			commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Dif_ToonLighting).Blend[kBlendModeNormal].Get());
		}

        commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(camera.cbvIndex)->GetGPUVirtualAddress());
        commandList->SetGraphicsRootConstantBufferView(1, rvManager->GetCBVResource(rvManager->GetLightCBVIndex())->GetGPUVirtualAddress());
        commandList->SetGraphicsRootDescriptorTable(2, rvManager->GetHandle(drawExecution_->GetAlbedoRTVIndex().texIndex).GPUHandle);
        commandList->SetGraphicsRootDescriptorTable(3, rvManager->GetHandle(drawExecution_->GetNormalRTVIndex().texIndex).GPUHandle);
        commandList->SetGraphicsRootDescriptorTable(4, rvManager->GetHandle(drawExecution_->GetPositionRTVIndex().texIndex).GPUHandle);
        commandList->SetGraphicsRootDescriptorTable(5, rvManager->GetHandle(drawExecution_->GetMaterialRTVIndex().texIndex).GPUHandle);
        commandList->SetGraphicsRootDescriptorTable(6, rvManager->GetHandle(dsvManager->GetSRVIndex()).GPUHandle);

        commandList->DrawInstanced(3, 1, 0, 0);

        return;
    }

    // 全てのエンティティ
    for (Entity entity : entityManager.GetActiveEntities()) {
        // 描画コンポーネント取得
        std::optional<std::reference_wrapper<RenderComponent>> renderComp = componentManager.GetComponent<RenderComponent>(entity);
        std::optional<std::reference_wrapper<MeshComponent>> meshComp = componentManager.GetComponent<MeshComponent>(entity);
        std::optional<std::reference_wrapper<MaterialComponent>> materialComp = componentManager.GetComponent<MaterialComponent>(entity);
        std::optional<std::reference_wrapper<TransformComponent>> transformComp = componentManager.GetComponent<TransformComponent>(entity);
        //AnimationComponent* animation = componentManager.GetAnimation(entity);

		// ディファードレンダリングでアルファ値が1未満の場合は描画しない
        if (psoMode == PSOMode::GBuffer&&materialComp&&materialComp.value().get().color.a < 1.0f) { continue; }
		// フォワードレンダリングでアルファ値が1未満の場合のみ描画
		if (psoMode == PSOMode::Demo && materialComp && materialComp.value().get().color.a >= 1.0f) { continue; }

        // 描画コンポーネントがない場合は次のエンティティへ
        if (!renderComp || !meshComp) { continue; }
		RenderComponent& render = renderComp.value();

        // 描画コンポーネントが非表示の場合は次のエンティティへ
        if (!render.visible) { continue; }
        MeshComponent& mesh = meshComp.value();
        TransformComponent& transform = transformComp.value();

		// マテリアルコンポネントがない場合は次のエンティティへ
		if (!materialComp) { continue; }

        // 頂点データ取得キー
        uint32_t meshesIndex;
        // モデルデータがある場合はモデルデータのメッシュインデックスを取得
        if (rvManager->GetModelData(mesh.meshesName)) {
            meshesIndex = rvManager->GetModelData(mesh.meshesName)->meshIndex;
        }
        else {
            meshesIndex = mesh.meshID;
        }

        // 描画処理: 描画コンポーネントに基づきリソースをバインドして描画

        // インスタンシング描画の場合はワイヤーフレーム描画を行わない(ワイヤーフレーム描画はインスタンシング描画に対応していない)
        if (transform.isInstance) { render.wireframe = false; }

        // ワイヤーフレーム描画の場合
        if (render.wireframe) {
            if (psoMode == PSOMode::Demo) {
                commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Wireframe).rootSignature.Get());
                commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Wireframe).Blend[kBlendModeNone].Get());
            }
            else if (psoMode == PSOMode::GBuffer)
            {
                commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::WireframeGB).rootSignature.Get());
                commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::WireframeGB).Blend[kBlendModeNone].Get());
            }
        }
        else {// ワイヤーフレーム描画でない場合
            if (psoMode == PSOMode::Demo) {

                if (transform.isInstance) {
                    commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::MapChip).rootSignature.Get());
                    commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::MapChip).Blend[kBlendModeNone].Get());
                }
                else {
                    commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Demo).rootSignature.Get());
                    commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Demo).Blend[kBlendModeNone].Get());
                }
            }
            else if (psoMode == PSOMode::GBuffer)
            {
                if (transform.isInstance) {
                    commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::MapChipGBuffer).rootSignature.Get());
                    commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::MapChipGBuffer).Blend[kBlendModeNone].Get());
                }
                else {
                    commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::GBuffer).rootSignature.Get());
                    commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::GBuffer).Blend[kBlendModeNone].Get());
                }
            }
        }
        for (auto& meshData : rvManager->GetMesh(meshesIndex)->meshData) {
            if (meshData.isAnimation) {
                ModelData* model = rvManager->GetModelData(rvManager->GetMesh(meshesIndex)->meshesName);
                // 頂点バッファビューをセット
                commandList->IASetVertexBuffers(0, 1, &rvManager->GetMeshViewData(model->skinCluster.skinningData.outputMVIndex)->vbvData.vbv);
            }
            else {
                // 頂点バッファビューをセット
                commandList->IASetVertexBuffers(0, 1, &rvManager->GetMeshViewData(meshData.meshViewIndex)->vbvData.vbv);
            }
            // インデックスバッファビューをセット
            commandList->IASetIndexBuffer(&rvManager->GetMeshViewData(meshData.meshViewIndex)->ibvData.ibv);

			// カメラとトランスフォームのルートパラメータをセット
            if (transform.isInstance) {
                commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(camera.cbvIndex)->GetGPUVirtualAddress());
                commandList->SetGraphicsRootDescriptorTable(1, rvManager->GetHandle(transform.srvIndex).GPUHandle);
            }
            else {
                commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(transform.cbvIndex)->GetGPUVirtualAddress());
                commandList->SetGraphicsRootConstantBufferView(1, rvManager->GetCBVResource(camera.cbvIndex)->GetGPUVirtualAddress());
            }

			// マテリアルのルートパラメータをセット
            if (!render.wireframe) {
                if (psoMode == PSOMode::Demo) {
                    if (materialComp) {
                        MaterialComponent& material = materialComp.value();
                        ModelData* model = rvManager->GetModelData(rvManager->GetMesh(meshesIndex)->meshesName);
                        if (model) {
                            std::string texName = rvManager->GetModelData(rvManager->GetMesh(meshesIndex)->meshesName)->objects[meshData.name].material.textureName;
                            if (texName == "") { texName = material.textureID; }
                            commandList->SetGraphicsRootConstantBufferView(2, rvManager->GetCBVResource(material.cbvIndex)->GetGPUVirtualAddress());
                            commandList->SetGraphicsRootDescriptorTable(3, rvManager->GetHandle(texLoad->GetTexture(texName).rvIndex).GPUHandle);
                        }
                        else {
                            commandList->SetGraphicsRootConstantBufferView(2, rvManager->GetCBVResource(material.cbvIndex)->GetGPUVirtualAddress());
                            commandList->SetGraphicsRootDescriptorTable(3, rvManager->GetHandle(texLoad->GetTexture(material.textureID).rvIndex).GPUHandle);
                        }
                    }
                    else {
                        assert(0 && "GBufferRender Must MaterialComponent");
                        commandList->SetGraphicsRootDescriptorTable(2, rvManager->GetHandle(texLoad->GetWhitePixel().rvIndex).GPUHandle);
                    }
                }
                else if (psoMode == PSOMode::GBuffer)
                {
                    if (materialComp) {
                        MaterialComponent& material = materialComp.value();
                        ModelData* model = rvManager->GetModelData(rvManager->GetMesh(meshesIndex)->meshesName);
                        if (model) {
                            std::string texName = rvManager->GetModelData(rvManager->GetMesh(meshesIndex)->meshesName)->objects[meshData.name].material.textureName;
                            if (texName == "") { texName = material.textureID; }
							commandList->SetGraphicsRootConstantBufferView(2, rvManager->GetCBVResource(material.cbvIndex)->GetGPUVirtualAddress());
                            commandList->SetGraphicsRootDescriptorTable(3, rvManager->GetHandle(texLoad->GetTexture(texName).rvIndex).GPUHandle);
                        }
                        else {
                            commandList->SetGraphicsRootConstantBufferView(2, rvManager->GetCBVResource(material.cbvIndex)->GetGPUVirtualAddress());
                            commandList->SetGraphicsRootDescriptorTable(3, rvManager->GetHandle(texLoad->GetTexture(material.textureID).rvIndex).GPUHandle);
                        }
                    }
                    else {
						assert(0&&"GBufferRender Must MaterialComponent");
                        //commandList->SetGraphicsRootDescriptorTable(2, rvManager->GetHandle(texLoad->GetWhitePixel().rvIndex).GPUHandle);
                    }
                }
            }
            else {
                if (psoMode == PSOMode::GBuffer)
                {
                    MaterialComponent& material = materialComp.value();
                    commandList->SetGraphicsRootDescriptorTable(2, rvManager->GetHandle(texLoad->GetTexture(material.textureID).rvIndex).GPUHandle);
                }
            }
            if (transform.isInstance) {
                commandList->DrawIndexedInstanced(static_cast<UINT>(meshData.size.indices), transform.instanceCount, 0, 0, 0);
            }
            else {
                commandList->DrawIndexedInstanced(static_cast<UINT>(meshData.size.indices), 1, 0, 0, 0);
            }
        }
    }
}

void RenderSystem::SpriteRender(EntityManager& entityManager, ComponentManager& componentManager, D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem, TextureLoader* texLoad)
{
    ID3D12GraphicsCommandList6* commandList = d3dCommand->GetCommand(CommandType::Draw).list.Get();

    // 全てのエンティティ
    for (Entity entity : entityManager.GetActiveEntities()) {
		std::optional<std::reference_wrapper<SpriteComponent>> spriteComp = componentManager.GetComponent<SpriteComponent>(entity);
        if (spriteComp) {
            SpriteComponent& sprite = spriteComp.value();
            if (sprite.render.visible) {
                // 頂点データ取得キー
                uint32_t meshViewIndex = rvManager->GetSpriteData(sprite.spriteIndex)->meshViewIndex;
                // 描画処理: 描画コンポーネントに基づきリソースをバインドして描画

                // ルートシグネイチャセット
                commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Sprite).rootSignature.Get());

                // パイプラインセット
                commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Sprite).Blend[sprite.blendMode].Get());

                // VBVセット
                commandList->IASetVertexBuffers(0, 1, &rvManager->GetMeshViewData(meshViewIndex)->vbvData.vbv);

                // IBVセット
                commandList->IASetIndexBuffer(&rvManager->GetMeshViewData(meshViewIndex)->ibvData.ibv);

                // ルートパラメータをセット
                commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(sprite.cbvIndex)->GetGPUVirtualAddress());
                commandList->SetGraphicsRootConstantBufferView(1, rvManager->GetCBVResource(sprite.material.cbvIndex)->GetGPUVirtualAddress());
                commandList->SetGraphicsRootDescriptorTable(2, rvManager->GetHandle(texLoad->GetTexture(sprite.material.textureID).rvIndex).GPUHandle);

                // DrawCall
                commandList->DrawIndexedInstanced(static_cast<UINT>(rvManager->GetSpriteData(sprite.spriteIndex)->size.indices), 1, 0, 0, 0);

            }
        }
    }
}

void RenderSystem::ParticleRender(EntityManager& entityManager, ComponentManager& componentManager, D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem, TextureLoader* texLoad)
{
    ID3D12GraphicsCommandList6* commandList = d3dCommand->GetCommand(CommandType::Draw).list.Get();

    // 今セットされているカメラのコンポーネント
    if (!componentManager.GetComponent<CameraComponent>(entityManager.GetNowCameraEntity())) {
        return;
    }
    CameraComponent& camera = componentManager.GetComponent<CameraComponent>(entityManager.GetNowCameraEntity()).value();

    // 全てのエンティティ
    for (Entity entity : entityManager.GetActiveEntities()) {
		std::optional<std::reference_wrapper<ParticleComponent>> particleComp = componentManager.GetComponent<ParticleComponent>(entity);
		std::optional<std::reference_wrapper<EmitterComponent>> emitterComp = componentManager.GetComponent<EmitterComponent>(entity);
        if (particleComp && emitterComp) {
			ParticleComponent& particle = particleComp.value();
            if (particle.render.visible) {
				EmitterComponent& emitter = emitterComp.value();
                emitter;
                // 頂点データ取得キー
                uint32_t meshesIndex = static_cast<uint32_t>(MeshPattern::Plane);
                // 描画処理: 描画コンポーネントに基づきリソースをバインドして描画

                // ルートシグネイチャセット
                commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Particle).rootSignature.Get());

                // パイプラインセット
                commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::Particle).Blend[kBlendModeAdd].Get());

                // VBVセット
                commandList->IASetVertexBuffers(0, 1, &rvManager->GetMeshViewData(rvManager->GetMesh(meshesIndex)->meshData[0].meshViewIndex)->vbvData.vbv);
                // IBVセット
                commandList->IASetIndexBuffer(&rvManager->GetMeshViewData(rvManager->GetMesh(meshesIndex)->meshData[0].meshViewIndex)->ibvData.ibv);
                // ルートパラメータをセット
                commandList->SetGraphicsRootDescriptorTable(1, rvManager->GetHandle(particle.uavIndex).GPUHandle);
                commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(camera.cbvIndex)->GetGPUVirtualAddress());
                commandList->SetGraphicsRootDescriptorTable(2, rvManager->GetHandle(texLoad->GetTexture(particle.material.textureID).rvIndex).GPUHandle);

                // DrawCall
                commandList->DrawIndexedInstanced(static_cast<UINT>(rvManager->GetMesh(meshesIndex)->meshData[0].size.indices), 1024, 0, 0, 0);
            }
        }
    }
}

void RenderSystem::EffectRender(EntityManager& entityManager, ComponentManager& componentManager, D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem, TextureLoader* texLoad)
{
    texLoad;
    ID3D12GraphicsCommandList6* commandList = d3dCommand->GetCommand(CommandType::Draw).list.Get();

    // 今セットされているカメラのコンポーネント
    if (!componentManager.GetComponent<CameraComponent>(entityManager.GetNowCameraEntity())) {
        return;
    }
    CameraComponent& camera = componentManager.GetComponent<CameraComponent>(entityManager.GetNowCameraEntity()).value();

    // 全てのエンティティ
    for (Entity entity : entityManager.GetActiveEntities()) {
        std::optional<std::reference_wrapper<EffectComponent>> effectComp = componentManager.GetComponent<EffectComponent>(entity);

        if (!effectComp) {
            continue;
        }
        EffectComponent& effect = effectComp.value();
        if (!effect.render.visible) {
            continue;
        }
        for (EffectNode& node : effect.effectNodes) {
            // 頂点データ取得キー
            uint32_t meshesIndex;
            if (node.draw.meshType == EffectMeshType::EffectMeshTypeSprite) {
                meshesIndex = static_cast<uint32_t>(MeshPattern::Plane);
            }
            else if (node.draw.meshType == EffectMeshType::EffectMeshTypeModel) {
                meshesIndex = node.draw.meshModel.meshIndex;
            }

            // 描画処理: 描画コンポーネントに基づきリソースをバインドして描画
            commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::EffectSprite).rootSignature.Get());
            commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::EffectSprite).Blend[kBlendModeAdd].Get());

            MeshData& meshData = rvManager->GetMesh(meshesIndex)->meshData[0];

            // VBVセット
            commandList->IASetVertexBuffers(0, 1, &rvManager->GetMeshViewData(meshData.meshViewIndex)->vbvData.vbv);
            // インデックスバッファビューをセット
            commandList->IASetIndexBuffer(&rvManager->GetMeshViewData(meshData.meshViewIndex)->ibvData.ibv);

            // ルートパラメータをセット
            commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(camera.cbvIndex)->GetGPUVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(1, rvManager->GetHandle(node.srvIndex).GPUHandle);
            commandList->SetGraphicsRootDescriptorTable(2, rvManager->GetHandle(node.particle.particleUAVIndex).GPUHandle);
            commandList->SetGraphicsRootDescriptorTable(3, rvManager->GetHandle(texLoad->GetTexture(node.drawCommon.textureName).rvIndex).GPUHandle);

            // DrawCall
            commandList->DrawIndexedInstanced(static_cast<UINT>(rvManager->GetMesh(meshesIndex)->meshData[0].size.indices), node.common.maxCount, 0, 0, 0);

            LoopEffectNodeDraw(node, effect, d3dCommand, rvManager, graphicsSystem, texLoad, camera);
        }
    }
}

void RenderSystem::MeshShaderRenderTest(EntityManager& entityManager, ComponentManager& componentManager, D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem, TextureLoader* texLoad)
{
    entityManager;
	componentManager;
    d3dCommand;
	rvManager;
	graphicsSystem;
	texLoad;
    //if (mesh->meshesName == "teapot") {
    //    // メッシュシェーダーのDemo描画
    //    commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::DemoMS).rootSignature.Get());
    //    commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::DemoMS).Blend[kBlendModeNone].Get());
    //    MeshData& meshData = rvManager->GetMesh(meshesIndex)->meshData[0];
    //    meshData;
    //    // ルートパラメータをセット
    //    commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(transform->cbvIndex)->GetGPUVirtualAddress());
    //    commandList->SetGraphicsRootConstantBufferView(1, rvManager->GetCBVResource(camera.cbvIndex)->GetGPUVirtualAddress());
    //    commandList->SetGraphicsRootDescriptorTable(2, rvManager->GetHandle(meshData.srvVBVIndex).GPUHandle);
    //    commandList->SetGraphicsRootDescriptorTable(3, rvManager->GetHandle(meshData.srvUniqueVertexIndex).GPUHandle);
    //    commandList->SetGraphicsRootDescriptorTable(4, rvManager->GetHandle(meshData.srvMeshletIndex).GPUHandle);
    //    commandList->SetGraphicsRootDescriptorTable(5, rvManager->GetHandle(meshData.srvPrimitiveIndex).GPUHandle);
    //    commandList->DispatchMesh(static_cast<UINT>(meshData.meshlets.size()), 1, 1);
    //    continue;
    //}
}

void RenderSystem::ColliderRender(EntityManager& entityManager, ComponentManager& componentManager, D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem, const CameraComponent& camera)
{
    Matrix4 viewProjectionMatrix = Multiply(camera.constData->view, camera.constData->projection);
    Matrix4 viewportMatrix = MakeViewportMatrix(0, 0, static_cast<float>(WindowWidth()), static_cast<float>(WindowHeight()), 0.0f, 1.0f);
    const uint32_t kSubdivision = 32;                          // 分割数
    float PI = ChoMath::Pi();
    const float kLonEvery = 2.0f * PI / kSubdivision; // 経度分割1つ分の角度
    const float kLatEvery = PI / kSubdivision;        // 緯度分割1つ分の角度
	Matrix4 mat = ChoMath::MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float>(WindowWidth()), static_cast<float>(WindowHeight()), 0.0f, 100.0f);
    rvManager->SetOrthographicConstBuffer(mat);
    // 全てのエンティティ
    for (Entity entity : entityManager.GetActiveEntities()) {
		std::optional<std::reference_wrapper<ColliderComponent>> colliderComp = componentManager.GetComponent<ColliderComponent>(entity);
        if (colliderComp) {
			ColliderComponent& collider = colliderComp.value();
            if (collider.visible) {
                std::vector<LineVertex> lineVertex;
                switch (collider.type)
                {
                case ColliderType::Sphere:

                    // 緯度の方向に分割　-π/2 ～ π/2
                    for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
                        float lat = -PI / 2.0f + kLatEvery * latIndex; // 現在の緯度

                        // 経度の方向に分割 0 ～ 2π
                        for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
                            float lon = lonIndex * kLonEvery; // 現在の経度

                            // 現在の点を求める
                            float x1 = collider.center.x + collider.radius * std::cosf(lat) * std::cosf(lon);
                            float y1 = collider.center.y + collider.radius * std::sinf(lat);
                            float z1 = collider.center.z + collider.radius * std::cosf(lat) * std::sinf(lon);

                            // 次の点を求める（経度方向）
                            float x2 = collider.center.x + collider.radius * std::cosf(lat) * std::cosf(lon + kLonEvery);
                            float y2 = collider.center.y + collider.radius * std::sinf(lat);
                            float z2 = collider.center.z + collider.radius * std::cosf(lat) * std::sinf(lon + kLonEvery);

                            // 次の点を求める（緯度方向）
                            float x3 = collider.center.x + collider.radius * std::cosf(lat + kLatEvery) * std::cosf(lon);
                            float y3 = collider.center.y + collider.radius * std::sinf(lat + kLatEvery);
                            float z3 = collider.center.z + collider.radius * std::cosf(lat + kLatEvery) * std::sinf(lon);

                            // 3D座標をVector3にセット
                            Vector3 start(x1, y1, z1);
                            Vector3 end1(x2, y2, z2);
                            Vector3 end2(x3, y3, z3);

                            // 座標変換を行う
                            start = Transform(start, viewProjectionMatrix);
                            start = Transform(start, viewportMatrix);
                            end1 = Transform(end1, viewProjectionMatrix);
                            end1 = Transform(end1, viewportMatrix);
                            end2 = Transform(end2, viewProjectionMatrix);
                            end2 = Transform(end2, viewportMatrix);

                            // 線を描画
                            lineVertex.push_back(LineVertex{ {start.x, start.y, 0.0f}, {1.0f,1.0f,1.0f,1.0f} });
                            lineVertex.push_back(LineVertex{ {end1.x, end1.y, 0.0f}, {1.0f,1.0f,1.0f,1.0f} });
                            lineVertex.push_back(LineVertex{ {start.x, start.y, 0.0f}, {1.0f,1.0f,1.0f,1.0f} });
                            lineVertex.push_back(LineVertex{ {end2.x, end2.y, 0.0f}, {1.0f,1.0f,1.0f,1.0f} });
                            //DrawLineRender(start, end1, d3dCommand, rvManager, graphicsSystem, camera);
                            //DrawLineRender(start, end2, d3dCommand, rvManager, graphicsSystem, camera);
                        }
                    }
                    DebugLinesRender(lineVertex, d3dCommand, rvManager, graphicsSystem, camera);
                    break;
                case ColliderType::Box:
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void RenderSystem::DrawLineRender(Vector3& start,Vector3& end, D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem, const CameraComponent& camera)
{
    ID3D12GraphicsCommandList6* commandList = d3dCommand->GetCommand(CommandType::Draw).list.Get();
    camera;
    // 頂点データ
    std::array vertices = {
		LineVertex{{start.x, start.y, 0.0f}, {1.0f,1.0f,1.0f,1.0f}},
		LineVertex{{end.x, end.y, 0.0f}, {1.0f,1.0f,1.0f,1.0f}}
    };

    assert(rvManager->GetIndexLine() < kMaxLineCount);

    size_t indexVertex = rvManager->GetIndexLine() * kVertexCountLine;

    assert(vertices.size() <= kVertexCountLine);
    // 頂点バッファへのデータ転送
    std::copy(vertices.begin(), vertices.end(), &rvManager->GetLineData()->mapped[indexVertex]);

    // 頂点データ取得キー
    //uint32_t meshesIndex = static_cast<uint32_t>(MeshPattern::Sphere);
    // 描画処理: 描画コンポーネントに基づきリソースをバインドして描画

    // ルートシグネイチャセット
    commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::DrawLine).rootSignature.Get());

    // パイプラインセット
    commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::DrawLine).Blend[kBlendModeNone].Get());

    // プリミティブ形状を設定
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

    // VBVセット
    commandList->IASetVertexBuffers(0, 1, &rvManager->GetMeshViewData(rvManager->GetLineData()->meshViewIndex)->vbvData.vbv);

    // ルートパラメータをセット
    commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(rvManager->GetOrthographicCBVIndex())->GetGPUVirtualAddress());

    // DrawCall
    commandList->DrawInstanced(kVertexCountLine, 1, static_cast<INT>(indexVertex), 0);

    rvManager->IndexLineAllocate();
}

void RenderSystem::DebugLinesRender(std::vector<LineVertex>& lineVertex, D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem, const CameraComponent& camera)
{
    camera;
    assert(lineVertex.size() % 1 == 0);
    [[maybe_unused]] auto numLines = lineVertex.size() / 2;
    assert((rvManager->GetIndexLine() + numLines) < kMaxLineCount);

    ID3D12GraphicsCommandList6* commandList = d3dCommand->GetCommand(CommandType::Draw).list.Get();

    size_t indexVertex = rvManager->GetIndexLine() * kVertexCountLine;

    //  頂点バッファへのデータ転送
    std::memcpy(
        &rvManager->GetLineData()->mapped[indexVertex], lineVertex.data(), sizeof(lineVertex[0]) * lineVertex.size());

    // ルートシグネイチャセット
    commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::DrawLine).rootSignature.Get());

    // パイプラインセット
    commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::DrawLine).Blend[kBlendModeNone].Get());

    // プリミティブ形状を設定
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

    // VBVセット
    commandList->IASetVertexBuffers(0, 1, &rvManager->GetMeshViewData(rvManager->GetLineData()->meshViewIndex)->vbvData.vbv);

    // ルートパラメータをセット
    //commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(rvManager->GetOrthographicCBVIndex())->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(camera.cbvIndex)->GetGPUVirtualAddress());
    // 描画コマンド
    commandList->DrawInstanced(UINT(lineVertex.size()), 1, static_cast<INT>(indexVertex), 0);
    // 使用カウント上昇
	rvManager->IndexLineAllocate(static_cast<uint32_t>(lineVertex.size()) / 2);
}

void RenderSystem::DrawGrid(
    EntityManager& entityManager,
    ComponentManager& componentManager,
    D3DCommand* d3dCommand,
    ResourceViewManager* rvManager,
    GraphicsSystem* graphicsSystem,
    const CameraComponent& camera
)
{
	entityManager, componentManager;
    //Matrix4 viewProjectionMatrix = Multiply(camera.constData->view, camera.constData->projection);
    //Matrix4 viewportMatrix = MakeViewportMatrix(0, 0, static_cast<float>(WindowWidth()), static_cast<float>(WindowHeight()), 0.0f, 1.0f);
    const float kGridHalfWidth = 80.0f;                                      // グリッドの半分の幅
    const uint32_t kSubdivision = 25;                                       // 分割数
    const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); // 1つの長さ
    //Matrix4 mat = ChoMath::MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float>(WindowWidth()), static_cast<float>(WindowHeight()), 0.0f, 100.0f);
    //rvManager->SetOrthographicConstBuffer(mat);
    std::vector<LineVertex> lineVertex;

    // 奥から手前への線を順々に引いていく
    for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
        // ワールド座標系上の始点と終点を求める
        Vector3 worldStartPos = { -kGridHalfWidth + xIndex * kGridEvery, 0.0f, -kGridHalfWidth };
        Vector3 worldEndPos = { -kGridHalfWidth + xIndex * kGridEvery, 0.0f, kGridHalfWidth };

        // ビュープロジェクションマトリックスを使ってクリップ座標系に変換
        //Vector3 clipStartPos = Transform(worldStartPos, viewProjectionMatrix);
        //Vector3 clipEndPos = Transform(worldEndPos, viewProjectionMatrix);

        // クリップ座標系からスクリーン座標系に変換
        //Vector3 screenStartPos = Transform(clipStartPos, viewportMatrix);
        //Vector3 screenEndPos = Transform(clipEndPos, viewportMatrix);

        // 真ん中の線を黒で描画
        if (xIndex == kSubdivision / 2 && kSubdivision % 2 == 0) {
			lineVertex.push_back(LineVertex{ worldStartPos, {1.0f,1.0f,1.0f,1.0f} });
			lineVertex.push_back(LineVertex{ worldEndPos, {1.0f,1.0f,1.0f,1.0f} });
			//lineVertex.push_back(LineVertex{ {screenStartPos.x, screenStartPos.y, 0.0f}, {1.0f,1.0f,1.0f,1.0f} });
			//lineVertex.push_back(LineVertex{ {screenEndPos.x, screenEndPos.y, 0.0f}, {1.0f,1.0f,1.0f,1.0f} });
            //Novice::DrawLine(int(screenStartPos.x), int(screenStartPos.y), int(screenEndPos.x), int(screenEndPos.y), 0x000000FF); // 黒色で描画
        }
        else {
			lineVertex.push_back(LineVertex{ worldStartPos, {1.0f,1.0f,1.0f,1.0f} });
			lineVertex.push_back(LineVertex{ worldEndPos, {1.0f,1.0f,1.0f,1.0f} });
			//lineVertex.push_back(LineVertex{ {screenStartPos.x, screenStartPos.y, 0.0f}, {1.0f,1.0f,1.0f,1.0f} });
			//lineVertex.push_back(LineVertex{ {screenEndPos.x, screenEndPos.y, 0.0f}, {1.0f,1.0f,1.0f,1.0f} });
            //Novice::DrawLine(int(screenStartPos.x), int(screenStartPos.y), int(screenEndPos.x), int(screenEndPos.y), 0xAAAAAAFF); // グレーで描画
        }
    }

    // 左右の線を引くためにyIndexのループも同様に処理
    for (uint32_t yIndex = 0; yIndex <= kSubdivision; ++yIndex) {
        // ワールド座標系上の始点と終点を求める
        Vector3 worldStartPos = { -kGridHalfWidth, 0.0f, -kGridHalfWidth + yIndex * kGridEvery };
        Vector3 worldEndPos = { kGridHalfWidth, 0.0f, -kGridHalfWidth + yIndex * kGridEvery };

        // 真ん中の線を黒で描画
        if (yIndex == kSubdivision / 2 && kSubdivision % 2 == 0) {
			lineVertex.push_back(LineVertex{ worldStartPos, {1.0f,1.0f,1.0f,1.0f} });
			lineVertex.push_back(LineVertex{ worldEndPos, {1.0f,1.0f,1.0f,1.0f} });
        }
        else {
			lineVertex.push_back(LineVertex{ worldStartPos, {1.0f,1.0f,1.0f,1.0f} });
			lineVertex.push_back(LineVertex{ worldEndPos, {1.0f,1.0f,1.0f,1.0f} });
        }
    }
    DebugLinesRender(lineVertex, d3dCommand, rvManager, graphicsSystem, camera);
}

void RenderSystem::DebugDrawEffect(EntityManager& entityManager, ComponentManager& componentManager, D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem, TextureLoader* texLoad, const CameraComponent& camera)
{
    texLoad;
    ID3D12GraphicsCommandList6* commandList = d3dCommand->GetCommand(CommandType::Draw).list.Get();

    // 全てのエンティティ
    for (Entity entity : entityManager.GetActiveEntities()) {
        std::optional<std::reference_wrapper<EffectComponent>> effectComp = componentManager.GetComponent<EffectComponent>(entity);

        if (!effectComp) {
            continue;
        }
        EffectComponent& effect = effectComp.value();
        if (!effect.render.visible) {
            continue;
        }
        for (EffectNode& node : effect.effectNodes) {
            // 頂点データ取得キー
            uint32_t meshesIndex = static_cast<uint32_t>(MeshPattern::Plane);

            // 描画処理: 描画コンポーネントに基づきリソースをバインドして描画
            commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::EffectSprite).rootSignature.Get());
            commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::EffectSprite).Blend[kBlendModeAdd].Get());

            MeshData& meshData = rvManager->GetMesh(meshesIndex)->meshData[0];

            // VBVセット
            commandList->IASetVertexBuffers(0, 1, &rvManager->GetMeshViewData(meshData.meshViewIndex)->vbvData.vbv);
            // インデックスバッファビューをセット
            commandList->IASetIndexBuffer(&rvManager->GetMeshViewData(meshData.meshViewIndex)->ibvData.ibv);

            // ルートパラメータをセット
            commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(camera.cbvIndex)->GetGPUVirtualAddress());
            commandList->SetGraphicsRootDescriptorTable(1, rvManager->GetHandle(node.srvIndex).GPUHandle);
            commandList->SetGraphicsRootDescriptorTable(2, rvManager->GetHandle(node.particle.particleUAVIndex).GPUHandle);
			commandList->SetGraphicsRootDescriptorTable(3, rvManager->GetHandle(texLoad->GetTexture(node.drawCommon.textureName).rvIndex).GPUHandle);

            // DrawCall
            commandList->DrawIndexedInstanced(static_cast<UINT>(rvManager->GetMesh(meshesIndex)->meshData[0].size.indices), node.common.maxCount, 0, 0, 0);

			LoopEffectNodeDraw(node, effect, d3dCommand, rvManager, graphicsSystem, texLoad, camera);
        }
    }
}

void RenderSystem::LoopEffectNodeDraw(EffectNode& effectNode, EffectComponent& effect, D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem, TextureLoader* texLoad,const CameraComponent& camera)
{
    ID3D12GraphicsCommandList6* commandList = d3dCommand->GetCommand(CommandType::Draw).list.Get();

    for (EffectNode& node : effectNode.children) {
        // 頂点データ取得キー
        uint32_t meshesIndex = static_cast<uint32_t>(MeshPattern::Plane);

        // 描画処理: 描画コンポーネントに基づきリソースをバインドして描画
        commandList->SetGraphicsRootSignature(graphicsSystem->GetPipeline()->GetPSO(PSOMode::EffectSprite).rootSignature.Get());
        commandList->SetPipelineState(graphicsSystem->GetPipeline()->GetPSO(PSOMode::EffectSprite).Blend[kBlendModeAdd].Get());

        MeshData& meshData = rvManager->GetMesh(meshesIndex)->meshData[0];

        // VBVセット
        commandList->IASetVertexBuffers(0, 1, &rvManager->GetMeshViewData(meshData.meshViewIndex)->vbvData.vbv);
        // インデックスバッファビューをセット
        commandList->IASetIndexBuffer(&rvManager->GetMeshViewData(meshData.meshViewIndex)->ibvData.ibv);

        // ルートパラメータをセット
        commandList->SetGraphicsRootConstantBufferView(0, rvManager->GetCBVResource(camera.cbvIndex)->GetGPUVirtualAddress());
        commandList->SetGraphicsRootDescriptorTable(1, rvManager->GetHandle(node.particle.particleUAVIndex).GPUHandle);
        commandList->SetGraphicsRootDescriptorTable(2, rvManager->GetHandle(texLoad->GetDummy().rvIndex).GPUHandle);

        // DrawCall
        commandList->DrawIndexedInstanced(static_cast<UINT>(rvManager->GetMesh(meshesIndex)->meshData[0].size.indices), node.common.maxCount, 0, 0, 0);

		LoopEffectNodeDraw(node, effect, d3dCommand, rvManager, graphicsSystem, texLoad, camera);
    }
}

