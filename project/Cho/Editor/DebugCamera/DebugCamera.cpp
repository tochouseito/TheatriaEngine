#include "pch.h"
#include "DebugCamera.h"
#include "Editor/EditorManager/EditorManager.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "ChoMath.h"
#include "Platform/InputManager/InputManager.h"

void DebugCamera::Initialize()
{
	// バッファの作成
	ResourceManager* resourceManager = m_pEditorManager->GetEngineCommand()->GetResourceManager();
	m_TransformComponent.mapID = resourceManager->CreateConstantBuffer<BUFFER_DATA_TF>();
	m_CameraComponent.bufferIndex = resourceManager->CreateConstantBuffer<BUFFER_DATA_VIEWPROJECTION>();
	//m_pTransformBuffer = dynamic_cast<ConstantBuffer<BUFFER_DATA_TF>*>(resourceManager->GetBuffer<IConstantBuffer>(m_TransformComponent.mapID));
	m_pCameraBuffer = dynamic_cast<ConstantBuffer<BUFFER_DATA_VIEWPROJECTION>*>(resourceManager->GetBuffer<IConstantBuffer>(m_CameraComponent.bufferIndex));
	resourceManager->SetDebugCameraBuffer(m_pCameraBuffer);
	// 初期値
	m_TransformComponent.position = Vector3(0.0f, 0.0f, -30.0f);
}

void DebugCamera::Update()
{
	// 各フラグ
	static bool isLeftClicked = false;
	static bool isWheelClicked = false;

	// 度数からラジアンに変換
	Vector3 radians = ChoMath::DegreesToRadians(m_TransformComponent.degrees);

	// 差分計算
	Vector3 diff = m_TransformComponent.preRot - radians;

	// 各軸のクオータニオンを作成
	Quaternion qx = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), diff.x);
	Quaternion qy = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), diff.y);
	Quaternion qz = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), diff.z);

	// 同時回転を累積(順序は気にしなくていい)
	Quaternion rotation = m_TransformComponent.rotation * qx * qy * qz;

	// 回転を考慮する
	Matrix4 rotationMatrix = ChoMath::MakeRotateMatrix(rotation);
	Vector3 X = { 1.0f, 0.0f, 0.0f };
	Vector3 Y = { 0.0f, 1.0f, 0.0f };
	Vector3 Z = { 0.0f, 0.0f, -1.0f };

	Vector3 rotatedX = ChoMath::Transform(X, rotationMatrix);
	Vector3 rotatedY = ChoMath::Transform(Y, rotationMatrix);
	Vector3 rotatedZ = ChoMath::Transform(Z, rotationMatrix);

	// カメラの操作
	InputManager* inputManager = m_pEditorManager->GetInputManager();
	// 回転
	if (inputManager->IsTriggerMouse(MouseButton::Right))
	{
		// マウスがクリックされたときに現在のマウス位置を保存する
		mousePos = inputManager->GetMouseWindowPosition();
		preMousePos = inputManager->GetMouseWindowPosition();
	}
	if (inputManager->IsPressMouse(MouseButton::Right))
	{
		// マウスの移動量を取得
		deltaMousePos.x = inputManager->GetMouseWindowPosition().x - preMousePos.x;
		deltaMousePos.y = inputManager->GetMouseWindowPosition().y - preMousePos.y;
		preMousePos = inputManager->GetMouseWindowPosition();
		// カメラ回転を更新
		m_TransformComponent.degrees.x -= deltaMousePos.y * mouseSensitivity;
		m_TransformComponent.degrees.y += deltaMousePos.x * mouseSensitivity;
	}
	// 位置
	if (inputManager->IsTriggerMouse(MouseButton::Center))
	{
		// マウスがクリックされたときに現在のマウス位置を保存する
		mousePos = inputManager->GetMouseWindowPosition();
		preMousePos = inputManager->GetMouseWindowPosition();
	}
	if (inputManager->IsPressMouse(MouseButton::Center))
	{
		// マウスの移動量を取得
		deltaMousePos.x = inputManager->GetMouseWindowPosition().x - preMousePos.x;
		deltaMousePos.y = inputManager->GetMouseWindowPosition().y - preMousePos.y;
		preMousePos = inputManager->GetMouseWindowPosition();
		// カメラ回転を更新
		m_TransformComponent.position -= rotatedX * deltaMousePos.x * mouseSensitivity;
		m_TransformComponent.position += rotatedY * deltaMousePos.y * mouseSensitivity;
	}
	// 前後移動
	// マウスホイールの移動量を取得する
	int32_t wheelDelta = -inputManager->GetWheel();
	// マウスホイールの移動量に応じてカメラの移動を更新する
	m_TransformComponent.position += rotatedZ * float(wheelDelta) * moveSpeed;
	// デルタマウス位置を初期化
	deltaMousePos.Initialize();
	UpdateMatrix();
}

void DebugCamera::UpdateMatrix()
{
	// 度数からラジアンに変換
	Vector3 radians = ChoMath::DegreesToRadians(m_TransformComponent.degrees);

	// 変更がなければreturn
	/*if (compo.translation==compo.prePos&&
		radians == compo.preRot) {
		return;
	}*/

	// 差分計算
	Vector3 diff = m_TransformComponent.preRot - radians;

	// 各軸のクオータニオンを作成
	Quaternion qx = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), diff.x);
	Quaternion qy = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, -1.0f, 0.0f), diff.y);
	Quaternion qz = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, -1.0f), diff.z);

	// 同時回転を累積
	m_TransformComponent.rotation = m_TransformComponent.rotation * qx * qy * qz;//*compo.rotation;

	// アフィン変換
	m_TransformComponent.matWorld = ChoMath::MakeAffineMatrix(Scale(1.0f, 1.0f, 1.0f), m_TransformComponent.rotation, m_TransformComponent.position);

	// 次のフレーム用に保存する
	m_TransformComponent.prePos = m_TransformComponent.position;
	m_TransformComponent.preRot = radians;

	TransferMatrix();
}

void DebugCamera::TransferMatrix()
{
	// カメラの行列を転送
	m_ViewProjectionData.matWorld = m_TransformComponent.matWorld;
	m_ViewProjectionData.view = Matrix4::Inverse(m_TransformComponent.matWorld);
	m_ViewProjectionData.projection = ChoMath::MakePerspectiveFovMatrix(
		m_CameraComponent.fovAngleY, m_CameraComponent.aspectRatio, m_CameraComponent.nearZ, m_CameraComponent.farZ);
	m_ViewProjectionData.projectionInverse = Matrix4::Inverse(m_ViewProjectionData.projection);
	m_ViewProjectionData.cameraPosition = m_TransformComponent.position;
	// 転送
	m_pCameraBuffer->UpdateData(m_ViewProjectionData);
}
