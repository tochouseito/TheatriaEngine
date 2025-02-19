#include "PrecompiledHeader.h"
#include "InfoView.h"

#include"imgui.h"
#include<memory>

#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"D3D12/RTVManager/RTVManager.h"
#include"D3D12/DrawExecution/DrawExecution.h"
#include"Load/TextureLoader/TextureLoader.h"

// SceneManager
#include"Scene/SceneManager/SceneManager.h"

#include"ECS/GameObject/GameObject.h"

#include"ECS/System/SystemManager/SystemManager.h"

#include"Editor/EditorManager/EditorManager.h"

#include"Generator/ScriptProject/ScriptProject.h"

#include"Script/ScriptManager/ScriptManager.h"

void InfoView::Initialize(
	ResourceViewManager* rvManager,
	RTVManager* rtvManager,
	DrawExecution* drawExe,
    TextureLoader* texLoader,
	EntityManager* entityManager,
	ComponentManager* componentManager,
	SystemManager* systemManager,
	PrefabManager* prefabManager,
	SceneManager* sceneManager,
    EditorManager* editManager,
    ScriptManager* scriptManager
)
{
    // D3D12
    rvManager_ = rvManager;
    rtvManager_ = rtvManager;
    drawExe_ = drawExe;

	texLoader_ = texLoader;

    // ECS
    entityManager_ = entityManager;
    componentManager_ = componentManager;
    systemManager_ = systemManager;
    prefabManager_ = prefabManager;

    // SceneManager
    sceneManager_ = sceneManager;

    editManager_ = editManager;

    scriptManager_ = scriptManager;
}

void InfoView::Update()
{
    // 移動を無効にするフラグ
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;
    ImGui::Begin("ObjectInfo",nullptr,windowFlags);
    GameObject* selectGO = editManager_->GetSelectedGO();
    if (selectGO) {
        // コンポーネント追加
        static bool isAdd = false;

        // 名前と EntityID を表示
        ImGui::Text("Name: %s EntityID: %d", editManager_->GetSelectedGOName().c_str(), selectGO->GetEntityID());
        
        switch (selectGO->GetObjectType())
        {
        case ObjectType::Object:

			// タイプを表示
			ImGui::Text("Type: Object");

            // コンポーネントがあれば表示
            if (componentManager_->GetComponent<TransformComponent>(selectGO->GetEntityID())) {
                TransformComponent& transform = componentManager_->GetComponent<TransformComponent>(selectGO->GetEntityID()).value();

                // Transformを表示
                ImGui::SeparatorText("Transform"); // ラインとテキスト表示

                // 平行移動の操作
                ColoredDragFloat3("Translation", &transform.translation.x, 0.01f, 0.0f, 0.0f, "%.1f");

                // 回転の操作
                ColoredDragFloat3("Rotation", &transform.degrees.x, 0.1f, 0.0f, 0.0f, "%.1f°");

                // スケールの操作
                ColoredDragFloat3("Scale", &transform.scale.x, 0.01f, 0.0f, 0.0f, "%.1f");
            }

            if (componentManager_->GetComponent<MeshComponent>(selectGO->GetEntityID())) {
				MeshComponent& meshComp = componentManager_->GetComponent<MeshComponent>(selectGO->GetEntityID()).value();
                meshComp;
                // メッシュ情報を表示
                ImGui::SeparatorText("MeshSelect");

                // コンボボックスでメッシュ形状を選択
                for (uint32_t index = 0;index < rvManager_->GetMeshes().size();index++) {
					if (ImGui::Selectable(rvManager_->GetMesh(index)->meshesName.c_str())) {
						// 選択されたメッシュの形状をセット
						meshComp.SetMeshID(index);
						meshComp.meshesName = rvManager_->GetMesh(index)->meshesName;
					}
                }
            }

            if (componentManager_->GetComponent<MaterialComponent>(selectGO->GetEntityID())) {
                MaterialComponent& materialComp = componentManager_->GetComponent<MaterialComponent>(selectGO->GetEntityID()).value();

                // マテリアル情報を表示
                ImGui::SeparatorText("Material");

                ImGui::Text("Texture : %s", materialComp.textureID.c_str());

    //            // コンボボックスでテクスチャを選択
				//for (auto& tex : texLoader_->GetTextureData())
				//{
				//	if (ImGui::Selectable(tex.first.c_str())) {
				//		// 選択されたテクスチャをセット
				//		materialComp.textureID = tex.first;
				//	}
				//}

                for (auto& tex : texLoader_->GetTextureData()) {
                    if (ImGui::Selectable(tex.first.c_str())) {
						materialComp.textureID = tex.first;
                    }
                }

				ImGui::DragFloat4("Color", &materialComp.color.r, 0.01f, 0.0f, 1.0f, "%.2f");

                bool enableLighting = false;
                if (materialComp.enableLighting != 0) {
                    enableLighting = true;
                }
                else {
					enableLighting = false;
                }
				ImGui::Checkbox("EnableLighting", &enableLighting);
				materialComp.enableLighting = enableLighting;

				ImGui::DragFloat("Shininess", &materialComp.shininess, 0.01f, 0.0f, 100.0f, "%.2f");
            }

            if (componentManager_->GetComponent<RenderComponent>(selectGO->GetEntityID())) {
                RenderComponent& render = componentManager_->GetComponent<RenderComponent>(selectGO->GetEntityID()).value();

                ImGui::Checkbox("Visible", &render.visible);
                ImGui::Checkbox("Wireframe", &render.wireframe);
            }

            if (componentManager_->GetComponent<ScriptComponent>(selectGO->GetEntityID())) {
                ScriptComponent& scriptComp = componentManager_->GetComponent<ScriptComponent>(selectGO->GetEntityID()).value();
                
                ImGui::SeparatorText("Script");

                // スクリプトのマップを取得
                std::unordered_map<ObjectType, std::unordered_map<std::string, ScriptStatus>> scripts = scriptManager_->GetScripts();
                // 対応するObjectTypeのスクリプトを検索
                auto it = scripts.find(static_cast<ObjectType>(scriptComp.type));
                if (it == scripts.end()) {
                    // 対応するスクリプトが見つからない場合
                    ImGui::Text("No scripts available for this ObjectType");
                }

                // 内部マップ（名前 -> ScriptStatus）を取得
                const auto& scriptMap = it->second;

                // 名前リストを構築
                std::vector<const char*> scriptNames;
                int currentIndex = -1;  // 現在選択中のスクリプトのインデックス

                for (const auto& pair : scriptMap) {
                    scriptNames.push_back(pair.first.c_str());  // 名前をリストに追加
                    if (scriptComp.status.name == pair.first) {
                        currentIndex = static_cast<int>(scriptNames.size()) - 1;  // 現在のスクリプト名に一致するインデックスを保存
                    }
                }

                if (scriptNames.empty()) {
                    ImGui::Text("No scripts available");
                } else {
                    // コンボボックスを表示
                    if (ImGui::Combo("ScriptName", &currentIndex, scriptNames.data(), static_cast<int>(scriptNames.size()))) {
                        // 選択が変更された場合、スクリプトコンポーネントを更新
                        scriptComp.status.name = scriptNames[currentIndex];
                        scriptComp.isScript = true;  // スクリプトが設定されているフラグを更新
                    }
                }
            }

			if (componentManager_->GetComponent<AnimationComponent>(selectGO->GetEntityID())) {
				AnimationComponent& animationComp = componentManager_->GetComponent<AnimationComponent>(selectGO->GetEntityID()).value();

				ImGui::SeparatorText("Animation");

                // 範囲と値
                int minValue = 0;
                int maxValue = animationComp.numAnimation - 1;;
                int currentValue = animationComp.animationIndex;

				if (maxValue < 0)
				{
					maxValue = 0;
				}
				if (currentValue > maxValue)
				{
					currentValue = maxValue;
				}

                ImGui::Text("Current Value: %d", currentValue);

                if (ImGui::Button("Decrease")) {
                    currentValue = std::max(currentValue - 1, minValue);
                }
                ImGui::SameLine();
                if (ImGui::Button("Increase")) {
                    currentValue = std::min(currentValue + 1, maxValue);
                }

				animationComp.animationIndex = currentValue;
			}

			if (componentManager_->GetComponent<ColliderComponent>(selectGO->GetEntityID()))
			{
				ColliderComponent& colliderComp = componentManager_->GetComponent<ColliderComponent>(selectGO->GetEntityID()).value();

				ImGui::SeparatorText("Collider");

				ImGui::DragFloat3("Center", &colliderComp.center.x, 0.01f);
				ImGui::DragFloat("Radius", &colliderComp.radius, 0.01f);
				//ImGui::Checkbox("IsCollision", &colliderComp.isCollision);
				ImGui::Checkbox("ColliderVisible", &colliderComp.visible);
			}
            
            if (isAdd) {
                if (!selectGO->GetComponent<MeshComponent>()) {
                    if (ImGui::Selectable("MeshComponent")) {
                        isAdd = false;
                        MeshComponent MeshCompo;

                        selectGO->AddComponent(MeshCompo);
                    }
                }
                if (!selectGO->GetComponent<TransformComponent>()) {
                    if (ImGui::Selectable("TransformComponent")) {
                        isAdd = false;
                        TransformComponent TFCompo;
                        selectGO->AddComponent(TFCompo);
                    }
                }
                if (!selectGO->GetComponent<RenderComponent>()) {
                    if (ImGui::Selectable("RenderComponent")) {
                        isAdd = false;
                        RenderComponent RenderCompo;

                        selectGO->AddComponent(RenderCompo);
                    }
                }
                if (!selectGO->GetComponent<MaterialComponent>()) {
                    if (ImGui::Selectable("MaterialComponent")) {
                        isAdd = false;
                        MaterialComponent MaterialComp;

                        selectGO->AddComponent(MaterialComp);
                    }
                }
                if (!selectGO->GetComponent<ScriptComponent>()) {
                    if (ImGui::Selectable("ScriptComponent")) {
                        isAdd = false;
                        ScriptComponent scriptComp;
                        scriptComp.id = selectGO->GetEntityID();
                        scriptComp.type = static_cast<uint32_t>(selectGO->GetObjectType());
                        scriptComp.ptr = componentManager_;
                        
                        selectGO->AddComponent(scriptComp);
                    }
                }
                if (!selectGO->GetComponent<AnimationComponent>()) {
					if (ImGui::Selectable("AnimationComponent")) {
						isAdd = false;
						AnimationComponent animationComp;
						selectGO->AddComponent(animationComp);
					}
                }
				if (!selectGO->GetComponent<ColliderComponent>())
				{
					if (ImGui::Selectable("ColliderComponent")) {
						isAdd = false;
						ColliderComponent colliderComp;
						selectGO->AddComponent(colliderComp);
					}
				}
            } else
            {
                if (ImGui::Button("AddComponent")) {
                    isAdd = true;
                }
            }
            break;
        case ObjectType::Camera:

			// タイプを表示
			ImGui::Text("Type: Camera");

            // コンポーネントがあれば表示
            if (componentManager_->GetComponent<CameraComponent>(selectGO->GetEntityID())) {
                CameraComponent& cameraCompo = componentManager_->GetComponent<CameraComponent>(selectGO->GetEntityID()).value();

                // Transformを表示
                ImGui::SeparatorText("Transform");// ラインとテキスト表示

                // 平行移動の操作
                ColoredDragFloat3("Translation", &cameraCompo.translation.x, 0.01f, 0.0f, 0.0f, "%.1f");

                // 回転の操作
                ColoredDragFloat3("Rotation", &cameraCompo.degrees.x, 0.01f, 0.0f, 0.0f, "%.1f°");
            }

            if (ImGui::Selectable("SetCamera")) {
				entityManager_->SetCameraID(selectGO->GetEntityID());
            }

            if (isAdd) {
                if (!selectGO->GetComponent<CameraComponent>()) {
                    if (ImGui::Selectable("CameraComponent")) {
                        isAdd = false;
                        CameraComponent cameraCompo;
                        selectGO->AddComponent(cameraCompo);
                    }
                }
            } else
            {
                if (ImGui::Button("AddComponent")) {
                    isAdd = true;
                }
            }
            break;
        case ObjectType::DirectionalLight:

			// コンポーネントがあれば表示
            if (componentManager_->GetComponent<DirectionalLightComponent>(selectGO->GetEntityID())) {
				DirectionalLightComponent& dirLight = componentManager_->GetComponent<DirectionalLightComponent>(selectGO->GetEntityID()).value();

				ImGui::SeparatorText("DirectionalLight");

                // ライトカラー
				ImGui::DragFloat3("LightColor", &dirLight.color.x, 0.01f, 0.0f, 1.0f, "%.2f");
				// ライト強度
                if (ImGui::DragFloat("LightIntensity", &dirLight.intensity, 0.01f, 0.0f, 10.0f, "%.2f")) {
					DirectionalLightComponent dirr = dirLight;
                }
				// ライト方向
				ImGui::DragFloat3("LightDirection", &dirLight.direction.x, 0.01f, -1.0f, 1.0f, "%.2f");
                // ライトの有効無効
                bool active = false;
                if (dirLight.active != 0) {
                    active = true;
                }
                else {
                    active = false;
                }
                ImGui::Checkbox("Active", &active);
                if (active) {
					dirLight.active = 1;
				}
                else
                {
                    dirLight.active = 0;
                }
            }

			if (isAdd) {
				if (!selectGO->GetComponent<DirectionalLightComponent>()) {
					if (ImGui::Selectable("DirectionalLightComponent")) {
						isAdd = false;
						DirectionalLightComponent dirLight;
						selectGO->AddComponent(dirLight);
					}
				}
			}
			else
			{
				if (ImGui::Button("AddComponent")) {
					isAdd = true;
				}
			}

            break;
		case ObjectType::SpotLight:

			// コンポーネントがあれば表示
            if (componentManager_->GetComponent<SpotLightComponent>(selectGO->GetEntityID())) {
                SpotLightComponent& spotLight = componentManager_->GetComponent<SpotLightComponent>(selectGO->GetEntityID()).value();

                ImGui::SeparatorText("SpotLight");

                // ライトカラー
                ImGui::DragFloat3("LightColor", &spotLight.color.x, 0.01f, 0.0f, 1.0f, "%.2f");
                // ライト強度
                ImGui::DragFloat("LightIntensity", &spotLight.intensity, 0.01f, 0.0f, 10.0f, "%.2f");
                // ライト方向
                ImGui::DragFloat3("LightDirection", &spotLight.direction.x, 0.01f, -1.0f, 1.0f, "%.2f");
                // ライトの届くの最大距離
                ImGui::DragFloat("Range", &spotLight.distance, 0.01f, 0.0f, 100.0f, "%.2f");
                // ライトの位置
                ImGui::DragFloat3("LightPosition", &spotLight.position.x, 0.01f, 0.0f, 0.0f, "%.2f");
                // 減衰率
                ImGui::DragFloat("decay", &spotLight.decay, 0.01f, 0.0f, 10.0f, "%.2f");
                // 余弦
                ImGui::DragFloat("cosAngle", &spotLight.cosAngle, 0.01f, 0.0f, 10.0f, "%.2f");
                // Falloff開始の角度
                ImGui::DragFloat("cosFalloffStart", &spotLight.cosFalloffStart, 0.01f, 0.0f, 10.0f, "%.2f");
                // ライトの有効無効
                bool active = false;
                if (spotLight.active != 0) {
                    active = true;
                }
                else {
                    active = false;
                }
                ImGui::Checkbox("Active", &active);
                if (active) {
                    spotLight.active = 1;
                }
                else
                {
                    spotLight.active = 0;
                }
            }

			if (isAdd) {
				if (!selectGO->GetComponent<SpotLightComponent>()) {
					if (ImGui::Selectable("SpotLightComponent")) {
						isAdd = false;
						SpotLightComponent spotLight;
						selectGO->AddComponent(spotLight);
					}
				}
			}
			else
			{
				if (ImGui::Button("AddComponent")) {
					isAdd = true;
				}
			}

            break;
		case ObjectType::PointLight:

			// コンポーネントがあれば表示
			if (componentManager_->GetComponent<PointLightComponent>(selectGO->GetEntityID())) {
				PointLightComponent& pointLight = componentManager_->GetComponent<PointLightComponent>(selectGO->GetEntityID()).value();
				ImGui::SeparatorText("PointLight");
				// ライトカラー
				ImGui::DragFloat3("LightColor", &pointLight.color.x, 0.01f, 0.0f, 1.0f, "%.2f");
				// ライト強度
				ImGui::DragFloat("LightIntensity", &pointLight.intensity, 0.01f, 0.0f, 10.0f, "%.2f");
				// ライトの位置
				ImGui::DragFloat3("LightPosition", &pointLight.position.x, 0.01f, 0.0f, 0.0f, "%.2f");
				// ライトの届くの最大距離
				ImGui::DragFloat("Range", &pointLight.radius, 0.01f, 0.0f, 100.0f, "%.2f");
				// 減衰率
				ImGui::DragFloat("decay", &pointLight.decay, 0.01f, 0.0f, 10.0f, "%.2f");
				// ライトの有効無効
				bool active = false;
				if (pointLight.active != 0) {
					active = true;
				}
				else {
					active = false;
				}
				ImGui::Checkbox("Active", &active);
				if (active) {
					pointLight.active = 1;
				}
				else
				{
					pointLight.active = 0;
				}
			}

			if (isAdd) {
				if (!selectGO->GetComponent<PointLightComponent>()) {
					if (ImGui::Selectable("PointLightComponent")) {
						isAdd = false;
						PointLightComponent pointLight;
						selectGO->AddComponent(pointLight);
					}
				}
			}
			else
			{
				if (ImGui::Button("AddComponent")) {
					isAdd = true;
				}
			}

			break;
        case ObjectType::Sprite:

			// タイプを表示
			ImGui::Text("Type: Sprite");

            // コンポーネントがあれば表示
            if (componentManager_->GetComponent<SpriteComponent>(selectGO->GetEntityID())) {
                SpriteComponent& spriteCompo = componentManager_->GetComponent<SpriteComponent>(selectGO->GetEntityID()).value();

                ImGui::Checkbox("visible", &spriteCompo.render.visible);

                // Transformを表示
                ImGui::SeparatorText("Transform");// ラインとテキスト表示

                // 平行移動の操作
                ImGui::DragFloat2("Position", &spriteCompo.position.x, 0.1f);

                // 回転の操作
                ImGui::DragFloat("ZRotate", &spriteCompo.rotation, 0.01f);

                // スケールの操作
                ImGui::DragFloat2("Scale", &spriteCompo.scale.x, 0.01f);

				// テクスチャの選択
                for (auto& tex : texLoader_->GetTextureData()) {
                    if (ImGui::Selectable(tex.first.c_str())) {
                        spriteCompo.material.textureID = tex.first;
                    }
                }

				ImGui::DragFloat4("Color", &spriteCompo.material.color.r, 0.01f, 0.0f, 1.0f, "%.2f");
            }

            if (isAdd) {
                
                if (!selectGO->GetComponent<SpriteComponent>()) {
                    if (ImGui::Selectable("SpriteComponent")) {
                        isAdd = false;
                        SpriteComponent spriteCompo;
                        selectGO->AddComponent(spriteCompo);
                    }
                }
            }
            else
            {
                if (ImGui::Button("AddComponent")) {
                    isAdd = true;
                }
            }
            break;
        case ObjectType::Particle:

			// タイプを表示
			ImGui::Text("Type: Particle");

            if (isAdd) {

                if (!selectGO->GetComponent<ParticleComponent>()) {
                    if (ImGui::Selectable("ParticleComponent")) {
                        isAdd = false;
                        ParticleComponent particleCompo;
                        selectGO->AddComponent(particleCompo);
                        systemManager_->Start(*entityManager_, *componentManager_);
                    }
                }

                if (!selectGO->GetComponent<EmitterComponent>()) {
                    if (ImGui::Selectable("EmitterComponent")) {
                        isAdd = false;
                        EmitterComponent emitterComp;
                        selectGO->AddComponent(emitterComp);
                    }
                }
            }
            else
            {
                if (ImGui::Button("AddComponent")) {
                    isAdd = true;
                }
            }
            break;
		case ObjectType::Effect:

			// タイプを表示
			ImGui::Text("Type: Effect");

			// コンポーネントがあれば表示
            if (componentManager_->GetComponent<EffectComponent>(selectGO->GetEntityID())) {
                EffectComponent& effect = componentManager_->GetComponent<EffectComponent>(selectGO->GetEntityID()).value();

                ImGui::SeparatorText("EffectComponent");

				if (ImGui::Button("AddNewEffectNode"))
				{
					EffectNode effectNode;
					rvManager_->CreateEffectNodeResource(effectNode);
					effect.effectNodes.push_back(effectNode);
                    //rvManager_->CreateEffectNodesResource(effect);
					editManager_->SetSelectedEffectNode(&effect.effectNodes.back());
					effect.isInit = true;
				}

				ImGui::Text("GlobalTime: %f", effect.timeManager->globalTime);

				ImGui::Checkbox("Loop", &effect.isLoop);

                if (ImGui::Button("Run")) {
                    effect.isRun = !effect.isRun;
                }

                if (ImGui::Button("1Frame+")) {
					effect.timeManager->globalTime++;
                }

                if (ImGui::Button("Reset")) {
					effect.timeManager->globalTime = 0.0f;
					effect.isInit = true;
					effect.isRun = false;
                }

                if (effect.isRun) {
                    ImGui::Text("Effect is Running");
				}
				else {
					ImGui::Text("Effect is Stopped");
				}

                ImGui::Checkbox("Visible", &effect.render.visible);

                ImGui::Separator();

                EffectNode* effectNode = editManager_->GetSelectedEffectNode();
                if (effectNode) {

                    if (ImGui::Button("AddChildEffectNode"))
                    {
                        EffectNode childEffectNode;
						rvManager_->CreateEffectNodeResource(childEffectNode);
						childEffectNode.parent = effectNode;
						effectNode->children.push_back(childEffectNode);
                        //rvManager_->CreateEffectNodesResource(effect);
                        editManager_->SetSelectedEffectNode(&effectNode->children.back());
						effect.isInit = true;
                    }

                    if (ImGui::BeginTabBar("EffectNode")) {
                        // 共通情報
                        if (ImGui::BeginTabItem("Common")) {
                            ImGui::Checkbox("Visible", &effectNode->common.visible);
                            char nameBuffer[128];
                            strncpy_s(nameBuffer, sizeof(nameBuffer), effectNode->common.name.c_str(), _TRUNCATE);
                            if (ImGui::InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer))) {
                                effectNode->common.name = std::string(nameBuffer);
                            }

                            int maxCount = static_cast<int>(effectNode->common.maxCount);
							int emitCount = static_cast<int>(effectNode->common.emitCount);
							ImGui::Text("Max Count: %d", maxCount);
                            if (ImGui::DragInt("EmitCount", &emitCount, 1, 0, 256, "EmitCount: %d")) {
								effectNode->common.emitCount = static_cast<uint32_t>(emitCount);
                            }
                            /*if (ImGui::DragInt("Max Count", &maxCount, 1, 0, 1024)) {
                                if (maxCount < effectNode->common.maxCount - 100) {
                                    effectNode->common.maxCount = static_cast<uint32_t>(maxCount);
                                    rvManager_->RemakeEffectNodeResource(*effectNode);
								}
								else if (maxCount > effectNode->common.maxCount + 100) {
									effectNode->common.maxCount = static_cast<uint32_t>(maxCount);
									rvManager_->RemakeEffectNodeResource(*effectNode);
								}
                            }*/

                            // InfluenceType 用の選択肢
                            static const char* influenceTypeStrings[] = {
                                "Always",       // InfluenceAlways
                                "Start Only",   // InfluenceStart
                                "None"          // InfluenceNone
                            };

                            // PosInfluenceType
                            int posInfluence = static_cast<int>(effectNode->common.PosInfluenceType);
                            if (ImGui::Combo("Position Influence", &posInfluence, influenceTypeStrings, IM_ARRAYSIZE(influenceTypeStrings))) {
                                effectNode->common.PosInfluenceType = static_cast<uint32_t>(posInfluence);
                            }

                            // RotInfluenceType
                            int rotInfluence = static_cast<int>(effectNode->common.RotInfluenceType);
                            if (ImGui::Combo("Rotation Influence", &rotInfluence, influenceTypeStrings, IM_ARRAYSIZE(influenceTypeStrings))) {
                                effectNode->common.RotInfluenceType = static_cast<uint32_t>(rotInfluence);
                            }

                            // SclInfluenceType
                            int sclInfluence = static_cast<int>(effectNode->common.SclInfluenceType);
                            if (ImGui::Combo("Scale Influence", &sclInfluence, influenceTypeStrings, IM_ARRAYSIZE(influenceTypeStrings))) {
                                effectNode->common.SclInfluenceType = static_cast<uint32_t>(sclInfluence);
                            }

                            ImGui::Checkbox("Delete on Lifetime End", &effectNode->common.deleteLifetime);
                            ImGui::Checkbox("Delete on Parent Delete", &effectNode->common.deleteParentDeleted);
                            ImGui::Checkbox("Delete on All Children Deleted", &effectNode->common.deleteAllChildrenDeleted);

                            // LifeTime
                            ImGui::SeparatorText("LifeTime");
                            //ImGui::DragFloat("Lifetime", &effectNode->common.lifeTime.lifeTime, 0.1f, 0.0f, 1000.0f);
                            // ランダム値の設定方法に応じた UI
                            if (effectNode->common.lifeTime.isMedian) {
                                ImGui::Text("Median Mode Settings");
                                ImGui::DragFloat("Median##LifeTime", &effectNode->common.lifeTime.randValue.first, 0.1f, 0.0f, 1000.0f, "Median: %.1f");
                                ImGui::DragFloat("Spread##LifeTime", &effectNode->common.lifeTime.randValue.second, 0.1f, 1.0f, 1000.0f, "Spread: %.1f");
                            }
                            else {
                                ImGui::Text("Min-Max Mode Settings");
                                ImGui::DragFloat("Max##LifeTime", &effectNode->common.lifeTime.randValue.first, 0.1f, 0.0f, 1000.0f, "Max: %.1f");
                                ImGui::DragFloat("Min##LifeTime", &effectNode->common.lifeTime.randValue.second, 0.1f, 0.0f, 1000.0f, "Min: %.1f");
                            }

                            // EmitTime
                            ImGui::SeparatorText("EmitTime");
                            //ImGui::DragFloat("EmitTime", &effectNode->common.emitTime.emitTime, 0.1f, 0.0f, 1000.0f);
                            // ランダム値の設定方法に応じた UI
                            if (effectNode->common.emitTime.isMedian) {
                                ImGui::Text("Median Mode Settings");
                                ImGui::DragFloat("Median##EmitTime", &effectNode->common.emitTime.randValue.first, 0.1f, 0.0f, 1000.0f, "Median: %.1f");
                                ImGui::DragFloat("Spread##EmitTime", &effectNode->common.emitTime.randValue.second, 0.1f, 1.0f, 1000.0f, "Spread: %.1f");
                            }
                            else {
                                ImGui::Text("Min-Max Mode Settings");
                                ImGui::DragFloat("Max##EmitTime", &effectNode->common.emitTime.randValue.first, 0.1f, 0.0f, 1000.0f, "Max: %.1f");
                                ImGui::DragFloat("Min##EmitTime", &effectNode->common.emitTime.randValue.second, 0.1f, 0.0f, 1000.0f, "Min: %.1f");
                            }

                            // EmitStartTime
                            ImGui::SeparatorText("EmitStartTime");
                           // ImGui::DragFloat("EmitStartTime", &effectNode->common.emitStartTime.emitStartTime, 0.1f, 0.0f, 1000.0f);
                            // ランダム値の設定方法に応じた UI
                            if (effectNode->common.emitStartTime.isMedian) {
                                ImGui::Text("Median Mode Settings");
                                ImGui::DragFloat("Median##EmitStartTime", &effectNode->common.emitStartTime.randValue.first, 0.1f, 0.0f, 1000.0f, "Median: %.1f");
                                ImGui::DragFloat("Spread##EmitStartTime", &effectNode->common.emitStartTime.randValue.second, 0.1f, 1.0f, 1000.0f, "Spread: %.1f");
                            }
                            else {
                                ImGui::Text("Min-Max Mode Settings");
                                ImGui::DragFloat("Max##EmitStartTime", &effectNode->common.emitStartTime.randValue.first, 0.1f, 0.0f, 1000.0f, "Max: %.1f");
                                ImGui::DragFloat("Min##EmitStartTime", &effectNode->common.emitStartTime.randValue.second, 0.1f, 0.0f, 1000.0f, "Min: %.1f");
                            }

							// TriggerType
                            static const char* triggerTypeStrings[] = {
                                "None",         // TriggerNone
								"Trigger1",     // Trigger1
								"Trigger2",     // Trigger2
								"Trigger3",     // Trigger3
								"Trigger4",     // Trigger4
                            };

							// EmitStartTrigger
							int emitStartTrigger = static_cast<int>(effectNode->common.emitStartTrigger);
							if (ImGui::Combo("EmitStartTrigger", &emitStartTrigger, triggerTypeStrings, IM_ARRAYSIZE(triggerTypeStrings))) {
								effectNode->common.emitStartTrigger = static_cast<uint32_t>(emitStartTrigger);
							}

							// EmitStopTrigger
							int emitStopTrigger = static_cast<int>(effectNode->common.emitStopTrigger);
							if (ImGui::Combo("EmitStopTrigger", &emitStopTrigger, triggerTypeStrings, IM_ARRAYSIZE(triggerTypeStrings))) {
								effectNode->common.emitStopTrigger = static_cast<uint32_t>(emitStopTrigger);
							}

							// deleteTrigger
							int deleteTrigger = static_cast<int>(effectNode->common.deleteTrigger);
							if (ImGui::Combo("DeleteTrigger", &deleteTrigger, triggerTypeStrings, IM_ARRAYSIZE(triggerTypeStrings))) {
								effectNode->common.deleteTrigger = static_cast<uint32_t>(deleteTrigger);
							}
                            
                            ImGui::EndTabItem();
                        }

                        // 位置
                        if (ImGui::BeginTabItem("Position##EffectNode")) {

                            // PositionType
							static const char* positionTypeStrings[] = {
								"Standard",     // PositionStandard
								"PVA",          // PositionPVA
								"Easing",       // PositionEasing
								"FCurve",       // PositionFCurve
								"NURBS",        // PositionNURBS
								"CameraOffset", // PositionCameraOffset
							};

							int positionType = static_cast<int>(effectNode->position.type);
							if (ImGui::Combo("Position Type", &positionType, positionTypeStrings, IM_ARRAYSIZE(positionTypeStrings))) {
								effectNode->position.type = static_cast<uint32_t>(positionType);
							}

                            if (effectNode->position.type == PositionType::PositionStandard) {
								ImGui::DragFloat3("Position", &effectNode->position.value.x, 0.1f);
							}
							else if (effectNode->position.type == PositionType::PositionPVA)
							{
								// PVA
								// 位置
                                if (effectNode->position.pva.value.isMedian) {
                                    ImGui::SeparatorText("Position##PVA");
									ImGui::DragFloat3("Median##PositionPVA", &effectNode->position.pva.value.first.x, 0.1f);
									ImGui::DragFloat3("Spread##PositionPVA", &effectNode->position.pva.value.second.x, 0.1f);
                                }
                                else {
									ImGui::SeparatorText("Position##PVA");
									ImGui::DragFloat3("Max##PositionPVA", &effectNode->position.pva.value.first.x, 0.1f);
									ImGui::DragFloat3("Min##PositionPVA", &effectNode->position.pva.value.second.x, 0.1f);
                                }

								// 速度
								if (effectNode->position.pva.velocity.isMedian) {
									ImGui::SeparatorText("Velocity##PVA");
									ImGui::DragFloat3("Median##VelocityPVA", &effectNode->position.pva.velocity.first.x, 0.1f);
									ImGui::DragFloat3("Spread##VelocityPVA", &effectNode->position.pva.velocity.second.x, 0.1f);
								}
								else {
									ImGui::SeparatorText("Velocity##PVA");
									ImGui::DragFloat3("Max##VelocityPVA", &effectNode->position.pva.velocity.first.x, 0.1f);
									ImGui::DragFloat3("Min##VelocityPVA", &effectNode->position.pva.velocity.second.x, 0.1f);
								}

								// 加速度
								if (effectNode->position.pva.acceleration.isMedian) {
									ImGui::SeparatorText("Acceleration##PVA");
									ImGui::DragFloat3("Median##AccelerationPVA", &effectNode->position.pva.acceleration.first.x, 0.1f);
									ImGui::DragFloat3("Spread##AccelerationPVA", &effectNode->position.pva.acceleration.second.x, 0.1f);
								}
								else {
									ImGui::SeparatorText("Acceleration##PVA");
									ImGui::DragFloat3("Max##AccelerationPVA", &effectNode->position.pva.acceleration.first.x, 0.1f);
									ImGui::DragFloat3("Min##AccelerationPVA", &effectNode->position.pva.acceleration.second.x, 0.1f);
								}
							}

                            ImGui::EndTabItem();
                        }

                        // 回転
                        if (ImGui::BeginTabItem("Rotate##EffectNode")) {
                            
							// RotationType
							static const char* rotationTypeStrings[] = {
								"Standard",     // RotationStandard
								"PVA",          // RotationPVA
								"Easing",       // RotationEasing
								"FCurve",       // RotationFCurve
								"NURBS",        // RotationNURBS
								"CameraOffset", // RotationCameraOffset
							};

							int rotationType = static_cast<int>(effectNode->rotation.type);
							if (ImGui::Combo("Rotation Type", &rotationType, rotationTypeStrings, IM_ARRAYSIZE(rotationTypeStrings))) {
								effectNode->rotation.type = static_cast<uint32_t>(rotationType);
							}

							if (effectNode->rotation.type == RotateType::RotateStandard) {
								ImGui::DragFloat3("Rotation", &effectNode->rotation.value.x, 0.1f);
							}
							else if (effectNode->rotation.type == RotateType::RotatePVA)
							{
								// PVA
								// 回転
								if (effectNode->rotation.pva.value.isMedian) {
									ImGui::SeparatorText("Rotation##PVA");
									ImGui::DragFloat3("Median##RotationPVA", &effectNode->rotation.pva.value.first.x, 0.01f);
									ImGui::DragFloat3("Spread##RotationPVA", &effectNode->rotation.pva.value.second.x, 0.01f);
								}
								else {
									ImGui::SeparatorText("Rotation##PVA");
									ImGui::DragFloat3("Max##RotationPVA", &effectNode->rotation.pva.value.first.x, 0.01f);
									ImGui::DragFloat3("Min##RotationPVA", &effectNode->rotation.pva.value.second.x, 0.01f);
								}

								// 速度
								if (effectNode->rotation.pva.velocity.isMedian) {
									ImGui::SeparatorText("Velocity##PVA");
									ImGui::DragFloat3("Median##VelocityPVA", &effectNode->rotation.pva.velocity.first.x, 0.01f);
									ImGui::DragFloat3("Spread##VelocityPVA", &effectNode->rotation.pva.velocity.second.x, 0.01f);
								}
								else {
									ImGui::SeparatorText("Velocity##PVA");
									ImGui::DragFloat3("Max##VelocityPVA", &effectNode->rotation.pva.velocity.first.x, 0.01f);
									ImGui::DragFloat3("Min##VelocityPVA", &effectNode->rotation.pva.velocity.second.x, 0.01f);
								}

								// 加速度
								if (effectNode->rotation.pva.acceleration.isMedian) {
									ImGui::SeparatorText("Acceleration##PVA");
									ImGui::DragFloat3("Median##AccelerationPVA", &effectNode->rotation.pva.acceleration.first.x, 0.01f);
									ImGui::DragFloat3("Spread##AccelerationPVA", &effectNode->rotation.pva.acceleration.second.x, 0.01f);
								}
								else {
									ImGui::SeparatorText("Acceleration##PVA");
									ImGui::DragFloat3("Max##AccelerationPVA", &effectNode->rotation.pva.acceleration.first.x, 0.01f);
									ImGui::DragFloat3("Min##AccelerationPVA", &effectNode->rotation.pva.acceleration.second.x, 0.01f);
								}
							}

                            ImGui::EndTabItem();
                        }

                        // スケール
                        if (ImGui::BeginTabItem("Scale##EffectNode")) {
                               
							// ScaleType
							static const char* scaleTypeStrings[] = {
								"Standard",     // ScaleStandard
								"PVA",          // ScalePVA
								"Easing",       // ScaleEasing
								"FCurve",       // ScaleFCurve
								"NURBS",        // ScaleNURBS
								"CameraOffset", // ScaleCameraOffset
							};

							int scaleType = static_cast<int>(effectNode->scale.type);
							if (ImGui::Combo("Scale Type", &scaleType, scaleTypeStrings, IM_ARRAYSIZE(scaleTypeStrings))) {
								effectNode->scale.type = static_cast<uint32_t>(scaleType);
							}

							if (effectNode->scale.type == ScaleType::ScaleStandard) {
								ImGui::DragFloat3("Scale", &effectNode->scale.value.x, 0.1f);
							}
							else if (effectNode->scale.type == ScaleType::ScalePVA)
							{
								// PVA
								// スケール
								if (effectNode->scale.pva.value.isMedian) {
									ImGui::SeparatorText("Scale##PVA");
									ImGui::DragFloat3("Median##ScalePVA", &effectNode->scale.pva.value.first.x, 0.1f);
									ImGui::DragFloat3("Spread##ScalePVA", &effectNode->scale.pva.value.second.x, 0.1f);
								}
								else {
									ImGui::SeparatorText("Scale##PVA");
									ImGui::DragFloat3("Max##ScalePVA", &effectNode->scale.pva.value.first.x, 0.1f);
									ImGui::DragFloat3("Min##ScalePVA", &effectNode->scale.pva.value.second.x, 0.1f);
								}
                                    
								// 速度
								if (effectNode->scale.pva.velocity.isMedian) {
									ImGui::SeparatorText("Velocity##PVA");
									ImGui::DragFloat3("Median##VelocityPVA", &effectNode->scale.pva.velocity.first.x, 0.1f);
									ImGui::DragFloat3("Spread##VelocityPVA", &effectNode->scale.pva.velocity.second.x, 0.1f);
                                       
								}
								else {
									ImGui::SeparatorText("Velocity##PVA");
									ImGui::DragFloat3("Max##VelocityPVA", &effectNode->scale.pva.velocity.first.x, 0.1f);
									ImGui::DragFloat3("Min##VelocityPVA", &effectNode->scale.pva.velocity.second.x, 0.1f);
								}

								// 加速度
								if (effectNode->scale.pva.acceleration.isMedian) {
									ImGui::SeparatorText("Acceleration##PVA");
									ImGui::DragFloat3("Median##AccelerationPVA", &effectNode->scale.pva.acceleration.first.x, 0.1f);
									ImGui::DragFloat3("Spread##AccelerationPVA", &effectNode->scale.pva.acceleration.second.x, 0.1f);
								}
								else {
									ImGui::SeparatorText("Acceleration##PVA");
									ImGui::DragFloat3("Max##AccelerationPVA", &effectNode->scale.pva.acceleration.first.x, 0.1f);
									ImGui::DragFloat3("Min##AccelerationPVA", &effectNode->scale.pva.acceleration.second.x, 0.1f);
								}
							}

                            ImGui::EndTabItem();
                        }

                        // 描画共通設定
                        if (ImGui::BeginTabItem("DrawCommon")) {
                            static const char* materialTypes[] = { "Standard" };
                            int materialType = static_cast<int>(effectNode->drawCommon.materialType);
                            if (ImGui::Combo("Material Type", &materialType, materialTypes, IM_ARRAYSIZE(materialTypes))) {
                                effectNode->drawCommon.materialType = static_cast<uint32_t>(materialType);
                            }

                            ImGui::DragFloat("Emissive Magnification", &effectNode->drawCommon.emissiveMagnification, 0.1f, 0.0f, 10.0f);

							// Texture
                            D3D12_GPU_DESCRIPTOR_HANDLE textureHandle; 
                            ImTextureID selectTex = nullptr;
                            if (effectNode->drawCommon.textureName != "") {
                                textureHandle = rvManager_->GetHandle(texLoader_->GetTexture(effectNode->drawCommon.textureName).rvIndex).GPUHandle;
								selectTex = (ImTextureID)textureHandle.ptr;
                            }
                            else {
                                textureHandle = {};
								selectTex = nullptr;
                            }
                            
                            static bool isOpenTexListWindow = false;
							if (ImGui::Button("Load Texture"))
							{
                                isOpenTexListWindow = true;
							}

							/*if (isOpenTexListWindow) {
                                ImGuiWindowFlags texListWindowFlags = ImGuiWindowFlags_NoDocking;
								if (ImGui::Begin("TextureList", &isOpenTexListWindow,texListWindowFlags)) {
									for (auto& tex : texLoader_->GetTextureData()) {
										if (ImGui::Selectable(tex.first.c_str())) {
											effectNode->drawCommon.textureName = tex.first;
                                            textureHandle = rvManager_->GetHandle(tex.second.rvIndex).GPUHandle;
                                            textureID = (ImTextureID)textureHandle.ptr;
											isOpenTexListWindow = false;
										}
									}
									ImGui::End();
								}
							}*/

                            if (isOpenTexListWindow) {
                                ImGuiWindowFlags texListWindowFlags = ImGuiWindowFlags_NoDocking;
                                if (ImGui::Begin("Texture List", &isOpenTexListWindow, texListWindowFlags)) {
                                    const int columns = 4; // 一行に表示する画像の数
                                    int count = 0;

                                    for (auto& tex : texLoader_->GetTextureData()) {
                                        // テクスチャの ImTextureID を取得
                                        textureHandle = rvManager_->GetHandle(tex.second.rvIndex).GPUHandle;
                                        selectTex = (ImTextureID)textureHandle.ptr;

                                        // ボタンのサイズ
                                        ImVec2 thumbnailSize(64, 64); // サムネイルサイズを調整

                                        // ユニークなIDを付ける（テクスチャ名を使う）
                                        std::string buttonID = "##" + tex.first;

                                        // 画像ボタンを配置（クリックすると選択）
                                        if (ImGui::ImageButton(buttonID.c_str(), selectTex, thumbnailSize)) {
                                            effectNode->drawCommon.textureName = tex.first;
                                            isOpenTexListWindow = false;
                                        }

                                        // 画像の名前を表示
                                        //ImGui::Text("%s", tex.first.c_str());

                                        // 横に並べる処理（columns個ごとに改行）
                                        if (++count % columns != 0) {
                                            ImGui::SameLine();
                                        }
                                    }
                                    ImGui::End();
                                }
                            }

                            if (selectTex&&ImGui::Button("DeleteTexture")) {
                                effectNode->drawCommon.textureName = "";
								selectTex = nullptr;
                            }
                            if (selectTex) {
								ImGui::Image(selectTex, ImVec2(100, 100));
                            }

							static const char* filterTypes[] = { "Nearest", "Linear" };
							int filterType = static_cast<int>(effectNode->drawCommon.filterType);
							if (ImGui::Combo("Filter Type", &filterType, filterTypes, IM_ARRAYSIZE(filterTypes))) {
								effectNode->drawCommon.filterType = static_cast<uint32_t>(filterType);
							}

							static const char* wrapTypes[] = { "Repeat", "Clamp" };
							int wrapType = static_cast<int>(effectNode->drawCommon.addressMode);
							if (ImGui::Combo("Wrap Type", &wrapType, wrapTypes, IM_ARRAYSIZE(wrapTypes))) {
								effectNode->drawCommon.addressMode = static_cast<uint32_t>(wrapType);
							}

                            static const char* blendTypes[] = { "None","Normal", "Add", "Sub" };
							int blendType = static_cast<int>(effectNode->drawCommon.blendMode);
							if (ImGui::Combo("Blend Type", &blendType, blendTypes, IM_ARRAYSIZE(blendTypes))) {
								effectNode->drawCommon.blendMode = static_cast<uint32_t>(blendType);
							}

                            ImGui::Checkbox("Depth Write", &effectNode->drawCommon.DepthWrite);
                            ImGui::Checkbox("Depth Test", &effectNode->drawCommon.DepthTest);
							ImGui::Checkbox("FadeIn", &effectNode->drawCommon.isFadeIn);

							static const char* fadeOutTypes[] = { "None", "InLifeTime", "Deleted" };
							int fadeOutType = static_cast<int>(effectNode->drawCommon.fadeOutType);
							if (ImGui::Combo("FadeOut Type", &fadeOutType, fadeOutTypes, IM_ARRAYSIZE(fadeOutTypes))) {
								effectNode->drawCommon.fadeOutType = static_cast<uint32_t>(fadeOutType);
							}

                            static const char* uvTypes[] = { "Standard","Const","Animation","Scroll","FCurve" };
							int uvType = static_cast<int>(effectNode->drawCommon.uvType);
							if (ImGui::Combo("UV Type", &uvType, uvTypes, IM_ARRAYSIZE(uvTypes))) {
								effectNode->drawCommon.uvType = static_cast<uint32_t>(uvType);
							}

							static const char* colorInfluenceTypes[] = { "None", "NoneRoot", "Emit", "Always" };
							int colorInfluenceType = static_cast<int>(effectNode->drawCommon.colorInfluenceType);
							if (ImGui::Combo("Color Influence Type", &colorInfluenceType, colorInfluenceTypes, IM_ARRAYSIZE(colorInfluenceTypes))) {
								effectNode->drawCommon.colorInfluenceType = static_cast<uint32_t>(colorInfluenceType);
							}

                            ImGui::EndTabItem();
                        }

                        // 描画設定
                        if (ImGui::BeginTabItem("Draw")) {
                            static const char* meshTypes[] = { "None", "Sprite", "Ribbon", "Trajectory", "Ring","Model" };
                            int meshType = static_cast<int>(effectNode->draw.meshType);
                            if (ImGui::Combo("Mesh Type", &meshType, meshTypes, IM_ARRAYSIZE(meshTypes))) {
                                effectNode->draw.meshType = static_cast<uint32_t>(meshType);
                            }

                            if (effectNode->draw.meshType == EffectMeshTypeSprite) {

                                static const char* colorType[] = {
                                    "Const",    // ColorConst
                                    "Random",   // ColorRandom
                                    "Easing",   // ColorEasing
                                    "FCurve",   // ColorFCurve
                                    "Gradient", // ColorGradient
                                };

								int colorTypeIndex = static_cast<int>(effectNode->draw.meshSprite.colorType);
								if (ImGui::Combo("Color Type", &colorTypeIndex, colorType, IM_ARRAYSIZE(colorType))) {
									effectNode->draw.meshSprite.colorType = static_cast<uint32_t>(colorTypeIndex);
								}

                                ImGui::ColorEdit4("Color", &effectNode->draw.meshSprite.color.r);

                                static const char* drawingOrders[] = { "Normal", "Inverse" };
                                int drawingOrder = static_cast<int>(effectNode->draw.meshSprite.drawingOrder);
                                if (ImGui::Combo("Drawing Order", &drawingOrder, drawingOrders, IM_ARRAYSIZE(drawingOrders))) {
                                    effectNode->draw.meshSprite.drawingOrder = static_cast<uint32_t>(drawingOrder);
                                }

                                static const char* placementTypes[] = {
                                    "Billboard",    // PlacementBillboard
                                    "ZAxisRotBillboard", // PlacementZAxisRotBillboard
                                    "YAxisConst",   // PlacementYAxisConst
                                    "Const",        // PlacementConst
                                };

								int placementType = static_cast<int>(effectNode->draw.meshSprite.placement);
								if (ImGui::Combo("Placement Type", &placementType, placementTypes, IM_ARRAYSIZE(placementTypes))) {
									effectNode->draw.meshSprite.placement = static_cast<uint32_t>(placementType);
								}

                                static const char* vertexColorType[] = {
                                    "Standard",     // VertexColorStandard
                                    "Const",        // VertexColorConst
                                };

								int vertexColorTypeIndex = static_cast<int>(effectNode->draw.meshSprite.vertexColorType);
                                if (ImGui::Combo("Vertex Color Type", &vertexColorTypeIndex, vertexColorType, IM_ARRAYSIZE(vertexColorType))) {
                                    effectNode->draw.meshSprite.vertexColorType = static_cast<uint32_t>(vertexColorTypeIndex);
                                }

								static const char* vertexPositionType[] = {
									"Standard",     // VertexPositionStandard
									"Const",        // VertexPositionConst
								};

								int vertexPositionTypeIndex = static_cast<int>(effectNode->draw.meshSprite.vertexPositionType);
                                if (ImGui::Combo("Vertex Position Type", &vertexPositionTypeIndex, vertexPositionType, IM_ARRAYSIZE(vertexPositionType))) {
                                    effectNode->draw.meshSprite.vertexPositionType = static_cast<uint32_t>(vertexPositionTypeIndex);
                                }
                            }
                            else if (effectNode->draw.meshType == EffectMeshTypeRibbon) {

							}
                            else if (effectNode->draw.meshType == EffectMeshTypeTrajectory) {

							}
                            else if (effectNode->draw.meshType == EffectMeshTypeRing) {

                            }
                            else if (effectNode->draw.meshType == EffectMeshTypeModel) {
                                // メッシュ情報を表示
                                ImGui::SeparatorText("MeshSelect");

                                // コンボボックスでメッシュ形状を選択
                                for (uint32_t index = 0;index < rvManager_->GetMeshes().size();index++) {
                                    if (ImGui::Selectable(rvManager_->GetMesh(index)->meshesName.c_str())) {
                                        // 選択されたメッシュの形状をセット
										effectNode->draw.meshModel.meshIndex = index;
                                    }
                                }
                            }

                            ImGui::EndTabItem();
                        }

                        ImGui::EndTabBar();
                    }
                }
            }


            if (isAdd) {

                if (!selectGO->GetComponent<EffectComponent>()) {
                    if (ImGui::Selectable("EffectComponent")) {
                        isAdd = false;
                        EffectComponent effectCompo;
                        selectGO->AddComponent(effectCompo);
                    }
                }
			}
			else
			{
				if (ImGui::Button("AddComponent")) {
					isAdd = true;
				}
			}
            break;
        default:
            break;
        }
    }
    ImGui::End();
}

bool InfoView::ColoredDragFloat3(const char* label, float* v, float v_speed, float v_min, float v_max, const char* format) {
    
    ImGui::PushID(label); // 識別子をプッシュ（同じ名前のコントロールが競合しないようにする）

    float item_width = ImGui::CalcItemWidth(); // 現在の項目幅を取得
    float single_item_width = (item_width - ImGui::GetStyle().ItemSpacing.x * 2) / 3; // 3分割

    // X軸（赤背景）
    ImGui::PushItemWidth(single_item_width);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.2f, 0.2f, 1.0f)); // 赤背景
    bool x_changed = ImGui::DragFloat("##X", &v[0], v_speed, v_min, v_max, format);
    ImGui::PopStyleColor();
    ImGui::PopItemWidth();

    ImGui::SameLine(); // 横並び

    // Y軸（緑背景）
    ImGui::PushItemWidth(single_item_width);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.5f, 0.2f, 1.0f)); // 緑背景
    bool y_changed = ImGui::DragFloat("##Y", &v[1], v_speed, v_min, v_max, format);
    ImGui::PopStyleColor();
    ImGui::PopItemWidth();

    ImGui::SameLine(); // 横並び

    // Z軸（青背景）
    ImGui::PushItemWidth(single_item_width);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.5f, 1.0f)); // 青背景
    bool z_changed = ImGui::DragFloat("##Z", &v[2], v_speed, v_min, v_max, format);
    ImGui::PopStyleColor();
    ImGui::PopItemWidth();

    ImGui::SameLine();        // ラベルとスライダーを横並びにする
    ImGui::Text("%s", label); // ラベルを表示

    ImGui::PopID(); // 識別子をポップ

    return x_changed || y_changed || z_changed;
}
