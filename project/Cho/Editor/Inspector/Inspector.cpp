#include "pch.h"
#include "Inspector.h"
#include "Editor/EditorManager/EditorManager.h"
#include "EngineCommand/EngineCommands.h"
#include "GameCore/GameCore.h"
#include "Core/ChoLog/ChoLog.h"
using namespace Cho;
#include "Resources/ResourceManager/ResourceManager.h"

void Inspector::Initialize()
{
}

void Inspector::Update()
{
	Window();
}

void Inspector::Window()
{
	// 移動を無効にするフラグ
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;
	ImGui::Begin("Inspector", nullptr, windowFlags);

	// 選択中のオブジェクトがある場合に情報を表示
	if (m_EngineCommand->GetSelectedObject())
	{
		// 選択中のオブジェクトを取得
		GameObject* object = m_EngineCommand->GetSelectedObject();
		// オブジェクトの名前を取得
		std::wstring objectName = object->GetName();
		// オブジェクトのタイプを取得
		ObjectType objectType = object->GetType();
		// オブジェクト名とタイプを表示
		ImGui::Text("Name: %s", ConvertString(objectName).c_str());
		// タイプを表示
		ImGui::Text("Type: %s", ObjectTypeToWString(objectType));
		// コンポーネントの情報を表示
		ComponentsView(object);
		// コンポーネントの追加ボタンを表示
		AddComponent(object);
	}

	ImGui::End();
}

void Inspector::ComponentsView(GameObject* object)
{
	TransformComponentView(object);
	MeshFilterComponentView(object);
	MeshRendererComponentView(object);
	CameraComponentView(object);
	UISpriteComponentView(object);
	MaterialComponentView(object);
	ScriptComponentView(object);
	LineRendererComponentView(object);
	Rigidbody2DComponentView(object);
	BoxCollider2DComponentView(object);
}

void Inspector::TransformComponentView(GameObject* object)
{
	// TransformComponentを取得
	TransformComponent* transform = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<TransformComponent>(object->GetEntity());
	if (transform)
	{
		// Transformを表示
		ImGui::SeparatorText("Transform"); // ラインとテキスト表示

		// 平行移動の操作
		ImGuiEx::ColoredDragFloat3("Translation", &transform->translation.x, 0.01f, 0.0f, 0.0f, "%.1f");

		// 回転の操作
		ImGuiEx::ColoredDragFloat3("Rotation", &transform->degrees.x, 0.1f, 0.0f, 0.0f, "%.1f°");

		// スケールの操作
		ImGuiEx::ColoredDragFloat3("Scale", &transform->scale.x, 0.01f, 0.0f, 0.0f, "%.1f");
	} 
}

void Inspector::MeshFilterComponentView(GameObject* object)
{
	// メッシュオブジェクトじゃないならスキップ
	if (object->GetType() != ObjectType::MeshObject)
	{
		return;
	}
	// MeshFilterComponentを取得
	MeshFilterComponent* mesh = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<MeshFilterComponent>(object->GetEntity());
	if (mesh)
	{
		ImGui::Text("Mesh Component");
		if (ImGui::BeginMenu("MeshSelect"))
		{
			for (const auto& modelName : m_EngineCommand->GetResourceManager()->GetModelManager()->GetModelNameContainer())
			{
				std::string name = ConvertString(modelName.first);
				if (ImGui::MenuItem(name.c_str()))
				{
					// Transformとの連携
					TransformComponent* transform = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<TransformComponent>(object->GetEntity());
					// モデル名を設定
					mesh->modelName = modelName.first;
					// モデルのIDを取得
					mesh->modelID = m_EngineCommand->GetResourceManager()->GetModelManager()->GetModelDataIndex(mesh->modelName);
					// モデルのUseListに登録
					m_EngineCommand->GetResourceManager()->GetModelManager()->RegisterModelUseList(mesh->modelID.value(), transform->mapID.value());
				}
			}

			ImGui::EndMenu();
		}
	}
}

void Inspector::MeshRendererComponentView(GameObject* object)
{
	// メッシュオブジェクトじゃないならスキップ
	if (object->GetType() != ObjectType::MeshObject)
	{
		return;
	}
	// MeshRendererComponentを取得
	MeshRendererComponent* render = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<MeshRendererComponent>(object->GetEntity());
	if (render)
	{
		ImGui::Text("Render Component");
	}
}

void Inspector::CameraComponentView(GameObject* object)
{
	// カメラオブジェクトじゃないならスキップ
	if (object->GetType() != ObjectType::Camera)
	{
		return;
	}
	// CameraComponentを取得
	CameraComponent* camera = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<CameraComponent>(object->GetEntity());
	if (camera)
	{
		ImGui::Text("Camera Component");
	}
}

void Inspector::UISpriteComponentView(GameObject* object)
{
	// UIオブジェクトじゃないならスキップ
	if (object->GetType() != ObjectType::UI)
	{
		return;
	}
	// UISpriteComponentを取得
	UISpriteComponent* sprite = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<UISpriteComponent>(object->GetEntity());
	if (sprite)
	{
		ImGui::Text("Sprite Component");
	}
}

void Inspector::MaterialComponentView(GameObject* object)
{
	// メッシュオブジェクトじゃないならスキップ
	if (object->GetType() != ObjectType::MeshObject)
	{
		return;
	}
	// MaterialComponentを取得
	MaterialComponent* material = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<MaterialComponent>(object->GetEntity());
	if (material)
	{
		ImGui::Text("Material Component");
		ImGui::ColorEdit4("Color", &material->color.r);
		ImGui::Checkbox("UseTexture", &material->enableTexture);
		if (material->enableTexture)
		{
			// テクスチャ名を表示
			if (material->textureName.empty())
			{
				ImGui::Text("Texture Name: None");
			} else
			{
				ImGui::Text("Texture Name: %s",ConvertString(material->textureName).c_str());
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Texture"))
				{
					// ドロップされたテクスチャのIDを取得
					const char* textureName = static_cast<const char*>(payload->Data);
					// テクスチャ名を設定
					material->textureName = ConvertString(textureName);
					// テクスチャIDを取得
					material->textureID = m_EngineCommand->GetResourceManager()->GetTextureManager()->GetTextureID(material->textureName);
				}
				ImGui::EndDragDropTarget();
			}
		}
	}
}

void Inspector::ScriptComponentView(GameObject* object)
{
	ECSManager* ecs = m_EngineCommand->GetGameCore()->GetECSManager();
	ScriptComponent* script = ecs->GetComponent<ScriptComponent>(object->GetEntity());
	if (!script) return;

	ScriptContainer* scriptContainer = m_EngineCommand->GetResourceManager()->GetScriptContainer();
	if (!scriptContainer) return;

	ImGui::Text("Script Component");

	// プルダウン用スクリプト名一覧作成
	std::vector<std::string> scriptNames = { "None" };
	for (size_t i = 0; i<scriptContainer->GetScriptCount();i++)
	{
		std::optional<std::string> data = scriptContainer->GetScriptDataByID(static_cast<uint32_t>(i));
		if (data)
		{
			scriptNames.push_back(data.value());
		}
	}

	// 現在の選択位置を求める
	int currentIndex = 0;// 0番目は"None"
	for (int i = 1; i < scriptNames.size(); ++i)// 1番目から開始
	{
		if (script->scriptName == scriptNames[i])
		{
			currentIndex = i;
			break;
		}
	}

	// プルダウン表示
	if (ImGui::BeginCombo("Script", scriptNames.empty() ? "None" : scriptNames[currentIndex].c_str()))
	{
		for (int i = 0; i < scriptNames.size(); ++i)
		{
			bool isSelected = (i == currentIndex);
			if (ImGui::Selectable(scriptNames[i].c_str(), isSelected))
			{
				currentIndex = i;
				if (!i)
				{
					// "None"が選択されたらスクリプト無効化
					script->scriptName.clear();
					script->isActive = false;
				} else
				{
					// 選択されたスクリプト名とIDを設定
					script->scriptName = scriptNames[i];
					script->isActive = false;
				}
			}
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void Inspector::LineRendererComponentView(GameObject* object)
{
	auto ecs = m_EngineCommand->GetGameCore()->GetECSManager();
	auto lines = ecs->GetAllComponents<LineRendererComponent>(object->GetEntity());

	if (!lines || lines->empty()) return;

	ImGui::Text("Line Renderer Components");

	// スクロール可能な子ウィンドウ
	ImGui::BeginChild("LineRendererList", ImVec2(0, 200), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

	int index = 0;
	for (LineRendererComponent& line : *lines)
	{
		ImGui::PushID(index); // 各項目に一意IDをつける

		// 折りたたみヘッダーで表示をまとめる
		if (ImGui::CollapsingHeader(("Line " + std::to_string(index)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Indent();

			ImGui::InputFloat3("Start", &line.line.start.x);
			ImGui::InputFloat3("End", &line.line.end.x);
			ImGui::ColorEdit4("Color", &line.line.color.r);

			if (line.mapID.has_value())
			{
				ImGui::Text("MapID: %d", line.mapID.value());
			} else
			{
				ImGui::Text("MapID: None");
			}

			ImGui::Unindent();
		}

		ImGui::PopID();
		++index;
	}

	ImGui::EndChild();
}

void Inspector::Rigidbody2DComponentView(GameObject* object)
{
	Rigidbody2DComponent* rigidbody = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<Rigidbody2DComponent>(object->GetEntity());
	if (rigidbody)
	{
		if (ImGui::TreeNode("Rigidbody2D"))
		{
			// Active
			ImGui::Checkbox("Active", &rigidbody->isActive);

			// Body Type Combo
			const char* bodyTypeItems[] = { "Static", "Kinematic", "Dynamic" };
			int currentType = static_cast<int>(rigidbody->bodyType);
			if (ImGui::Combo("Body Type", &currentType, bodyTypeItems, IM_ARRAYSIZE(bodyTypeItems)))
			{
				rigidbody->bodyType = static_cast<b2BodyType>(currentType);
			}

			// Gravity Scale
			ImGui::DragFloat("Gravity Scale", &rigidbody->gravityScale, 0.1f, 0.0f, 100.0f);

			// Fixed Rotation
			ImGui::Checkbox("Fixed Rotation", &rigidbody->fixedRotation);

			// Is Kinematic（参考用）
			ImGui::Checkbox("Is Kinematic", &rigidbody->isKinematic);

			ImGui::TreePop();
		}
		// 実行中の変更を反映させるためのボタン
		//if (ImGui::Button("Apply Rigidbody Changes"))
		//{
		//	if (rigidbody->runtimeBody)
		//	{
		//		b2World* world = GetPhysicsWorld(); // あなたのエンジンで取得
		//		world->DestroyBody(rigidbody->runtimeBody);
		//		rigidbody->runtimeBody = nullptr;
		//	}
		//}
	}
}

void Inspector::BoxCollider2DComponentView(GameObject* object)
{
	BoxCollider2DComponent* collider =
		m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<BoxCollider2DComponent>(object->GetEntity());

	if (collider)
	{
		if (ImGui::TreeNode("BoxCollider2D"))
		{
			ImGui::DragFloat2("Offset", reinterpret_cast<float*>(&collider->offsetX), 0.1f);
			ImGui::DragFloat2("Size", reinterpret_cast<float*>(&collider->width), 0.1f, 0.01f, 100.0f);

			ImGui::DragFloat("Density", &collider->density, 0.05f, 0.0f, 100.0f);
			ImGui::DragFloat("Friction", &collider->friction, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &collider->restitution, 0.05f, 0.0f, 1.0f);

			ImGui::Checkbox("Is Sensor", &collider->isSensor);

			ImGui::TreePop();
		}
	}
}

void Inspector::EmitterComponentView(GameObject* object)
{
	EmitterComponent* emitter = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<EmitterComponent>(object->GetEntity());
	if (emitter)
	{
		if (ImGui::TreeNode("Emitter"))
		{
			ImGui::DragFloat3("Position", &emitter->position.x, 0.1f);
			ImGui::DragFloat("radius", &emitter->radius, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("Emit Frequency", &emitter->frequency, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("Emit frequencyTime", &emitter->frequencyTime, 0.1f, 0.0f, 100.0f);
		}
	}
}

void Inspector::ParticleComponentView(GameObject* object)
{
	ParticleComponent* particle = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<ParticleComponent>(object->GetEntity());
	if (particle)
	{
		ImGui::Text("Particle Component");
		/*if (ImGui::TreeNode("Particle"))
		{
			ImGui::DragFloat3("Position", &particle->position.x, 0.1f);
			ImGui::DragFloat("radius", &particle->radius, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("lifeTime", &particle->lifeTime, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("speed", &particle->speed, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("angle", &particle->angle, 0.1f, 0.0f, 100.0f);
			ImGui::DragFloat("gravity", &particle->gravity, 0.1f, -100.0f, 100.0f);
			ImGui::TreePop();
		}*/
	}
}


void Inspector::AddComponent(GameObject* object)
{
	static bool isOpen = false;
	MeshFilterComponent* mesh;
	MeshRendererComponent* render;
	MaterialComponent* material;
	ScriptComponent* script;
	std::vector<LineRendererComponent>* lines;
	lines;
	Rigidbody2DComponent* rigidbody;
	BoxCollider2DComponent* boxCollider2d;
	EmitterComponent* emitter;
	ParticleComponent* particle;
	if (!isOpen)
	{
		// コンポーネントの追加
		if (ImGui::Button("Add Component"))
		{
			isOpen = true;
		}
	} else
	{
		ObjectType objectType = object->GetType();
		switch (objectType)
		{
		case ObjectType::MeshObject:
			// MeshFilterComponentがあるか
			mesh = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<MeshFilterComponent>(object->GetEntity());
			if (!mesh)
			{
				if (ImGui::Selectable("MeshFilterComponent"))
				{
					// MeshFilterComponentを追加
					std::unique_ptr<AddMeshFilterComponent> addMeshComp = std::make_unique<AddMeshFilterComponent>(object->GetEntity());
					m_EngineCommand->ExecuteCommand(std::move(addMeshComp));
					isOpen = false;
				}
			}
			// MeshRendererComponentがあるか
			render = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<MeshRendererComponent>(object->GetEntity());
			if (!render)
			{
				if (ImGui::Selectable("MeshRendererComponent"))
				{
					// MeshRendererComponentを追加
					std::unique_ptr<AddMeshRendererComponent> addRenderComp = std::make_unique<AddMeshRendererComponent>(object->GetEntity());
					m_EngineCommand->ExecuteCommand(std::move(addRenderComp));
					isOpen = false;
				}
			}
			// MaterialComponentがあるか
			material = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<MaterialComponent>(object->GetEntity());
			if (!material)
			{
				if (ImGui::Selectable("MaterialComponent"))
				{
					// MaterialComponentを追加
					std::unique_ptr<AddMaterialComponent> addMaterialComp = std::make_unique<AddMaterialComponent>(object->GetEntity());
					m_EngineCommand->ExecuteCommand(std::move(addMaterialComp));
					isOpen = false;
				}
			}
			// ScriptComponentがあるか
			script = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<ScriptComponent>(object->GetEntity());
			if (!script)
			{
				if (ImGui::Selectable("ScriptComponent"))
				{
					// ScriptComponentを追加
					std::unique_ptr<AddScriptComponent> addScriptComp = std::make_unique<AddScriptComponent>(object->GetEntity(),object->GetID().value());
					m_EngineCommand->ExecuteCommand(std::move(addScriptComp));
					isOpen = false;
				}
			}
			// LineRendererComponentがあるか
			//lines = m_EditorCommand->GetGameCoreCommandPtr()->GetECSManagerPtr()->GetAllComponents<LineRendererComponent>(object->GetEntity());
			//if (!lines || lines->empty())
			//{
			if (ImGui::Selectable("LineRendererComponent"))
			{
				// LineRendererComponentを追加
				std::unique_ptr<AddLineRendererComponent> addLineComp = std::make_unique<AddLineRendererComponent>(object->GetEntity());
				m_EngineCommand->ExecuteCommand(std::move(addLineComp));
				isOpen = false;
			}
			//}
			// Rigidbody2DComponentがあるか
			rigidbody = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<Rigidbody2DComponent>(object->GetEntity());
			if (!rigidbody)
			{
				if (ImGui::Selectable("Rigidbody2DComponent"))
				{
					// Rigidbody2DComponentを追加
					std::unique_ptr<AddRigidbody2DComponent> addRigidBodyComp = std::make_unique<AddRigidbody2DComponent>(object->GetEntity(),object->GetID().value());
					m_EngineCommand->ExecuteCommand(std::move(addRigidBodyComp));
					isOpen = false;
				}
			}
			// BoxCollider2DComponentがあるか
			boxCollider2d = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<BoxCollider2DComponent>(object->GetEntity());
			if (!boxCollider2d)
			{
				if (ImGui::Selectable("BoxCollider2DComponent"))
				{
					// BoxCollider2DComponentを追加
					std::unique_ptr<AddBoxCollider2DComponent> addBoxColliderComp = std::make_unique<AddBoxCollider2DComponent>(object->GetEntity());
					m_EngineCommand->ExecuteCommand(std::move(addBoxColliderComp));
					isOpen = false;
				}
			}
			break;
		case ObjectType::Camera:
			// ScriptComponentがあるか
			script = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<ScriptComponent>(object->GetEntity());
			if (!script)
			{
				if (ImGui::Selectable("ScriptComponent"))
				{
					// ScriptComponentを追加
					std::unique_ptr<AddScriptComponent> addScriptComp = std::make_unique<AddScriptComponent>(object->GetEntity(),object->GetID().value());
					m_EngineCommand->ExecuteCommand(std::move(addScriptComp));
					isOpen = false;
				}
			}
			// LineRendererComponentがあるか
			//lines = m_EditorCommand->GetGameCoreCommandPtr()->GetECSManagerPtr()->GetAllComponents<LineRendererComponent>(object->GetEntity());
			//if (!lines || lines->empty())
			//{
			if (ImGui::Selectable("LineRendererComponent"))
			{
				// LineRendererComponentを追加
				std::unique_ptr<AddLineRendererComponent> addLineComp = std::make_unique<AddLineRendererComponent>(object->GetEntity());
				m_EngineCommand->ExecuteCommand(std::move(addLineComp));
				isOpen = false;
			}
			//}
			break;
		case ObjectType::ParticleSystem:
			// EmitterComponentがあるか
			emitter = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<EmitterComponent>(object->GetEntity());
			if (!emitter)
			{
				if (ImGui::Selectable("EmitterComponent"))
				{
					// EmitterComponentを追加
					std::unique_ptr<AddEmitterComponent> addEmitterComp = std::make_unique<AddEmitterComponent>(object->GetEntity());
					m_EngineCommand->ExecuteCommand(std::move(addEmitterComp));
					isOpen = false;
				}
			}
			// ParticleComponentがあるか
			particle = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<ParticleComponent>(object->GetEntity());
			if (!particle)
			{
				if (ImGui::Selectable("ParticleComponent"))
				{
					// ParticleComponentを追加
					std::unique_ptr<AddParticleComponent> addParticleComp = std::make_unique<AddParticleComponent>(object->GetEntity());
					m_EngineCommand->ExecuteCommand(std::move(addParticleComp));
					isOpen = false;
				}
			}
			break;
		case ObjectType::UI:
			// MaterialComponentがあるか
			material = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<MaterialComponent>(object->GetEntity());
			if (!material)
			{
				if (ImGui::Selectable("MaterialComponent"))
				{
					// MaterialComponentを追加
					std::unique_ptr<AddMaterialComponent> addMaterialComp = std::make_unique<AddMaterialComponent>(object->GetEntity());
					m_EngineCommand->ExecuteCommand(std::move(addMaterialComp));
					isOpen = false;
				}
			}
			// ScriptComponentがあるか
			script = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<ScriptComponent>(object->GetEntity());
			if (!script)
			{
				if (ImGui::Selectable("ScriptComponent"))
				{
					// ScriptComponentを追加
					std::unique_ptr<AddScriptComponent> addScriptComp = std::make_unique<AddScriptComponent>(object->GetEntity(), object->GetID().value());
					m_EngineCommand->ExecuteCommand(std::move(addScriptComp));
					isOpen = false;
				}
			}
			break;
		case ObjectType::Count:
			break;
		default:
			break;
		}
	}
}
