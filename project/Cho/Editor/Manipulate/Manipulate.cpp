#include "pch.h"
#include "Manipulate.h"
#include "Editor/EditorManager/EditorManager.h"
#include "EngineCommand/EngineCommand.h"
#include "GameCore/GameCore.h"

void Manipulate::Initialize()
{
}

void Manipulate::Update()
{
	// 編集対象のオブジェクトを取得
	GameObject* target = m_EditorManager->GetSelectedGameObject();
	if (target == nullptr) return;
	// Transformコンポーネントを取得
	TransformComponent* transform = m_EngineCommand->GetGameCore()->GetECSManager()->GetComponent<TransformComponent>(target->GetHandle().entity);
	if (transform == nullptr) return;

	CameraComponent& camera = m_dCamera->GetCameraComponent();
	Matrix4 view = camera.viewMatrix;
	Matrix4 projection = camera.projectionMatrix;

	// 変換
	float viewOut[16];
	float projectionOut[16];
	float objectOut[16];
	view.ToArray16(viewOut);
	projection.ToArray16(projectionOut);
	transform->matWorld.ToArray16(objectOut);

	// ImGuizmoの描画設定
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

	// 操作タイプ（移動・回転・スケールを切り替えるフラグ）
	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE mode = ImGuizmo::WORLD;

	// ギズモ操作
	//bool manipulated = ImGuizmo::Manipulate(
	//	viewOut, projectionOut,
	//	operation, mode,
	//	objectOut, nullptr // nullptrにすればdelta行列不要
	//);
	float translation[3], rotation[3], scale[3];

	// 初期値
	translation[0] = transform->position.x;
	translation[1] = transform->position.y;
	translation[2] = transform->position.z;
	rotation[0] = ChoMath::DegreesToRadians(transform->degrees.x);
	rotation[1] = ChoMath::DegreesToRadians(transform->degrees.y);
	rotation[2] = ChoMath::DegreesToRadians(transform->degrees.z);
	scale[0] = transform->scale.x;
	scale[1] = transform->scale.y;
	scale[2] = transform->scale.z;

	ImGuizmo::Mani(
		view, projection,
		ImGuizmo::TRANSLATE, ImGuizmo::WORLD,
		translation, rotation, scale);

	// モデル行列が変更されたら、オブジェクトに反映
	if (manipulated)
	{
		// model (float[16]) → XMMATRIX へ変換
		Matrix4 newMatrix = Matrix4::FromArray16(objectOut);
		// Transformコンポーネントに新しい行列を設定
		transform->matWorld = newMatrix;
	}
}

void Manipulate::Window()
{
}
