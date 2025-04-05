#include "pch.h"
#include "SystemManager.h"
#include "Resources/ResourceManager/ResourceManager.h"

void ObjectSystem::Update(ECSManager* ecs)
{
	std::vector<Entity> transforms;
	for (const auto& [arch, container] : ecs->GetArchToEntities())
	{
		if (arch.test(ECSManager::ComponentPool<TransformComponent>::GetID()))
		{
			for (Entity e : container.GetEntities())
			{
				transforms.push_back(e);
			}
		}
	}

	std::unordered_set<Entity> updated;

	for (Entity e : transforms)
	{
		UpdateRecursive(e, updated);
	}
}

void ObjectSystem::UpdateMatrix(TransformComponent& transform, TransformComponent* parent)
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
	if (parent)
	{
		transform.matWorld = ChoMath::Multiply(transform.matWorld, parent->matWorld);
	}

	// 行列の転送
	TransferMatrix(transform);
}

void ObjectSystem::TransferMatrix(TransformComponent& transform)
{
	// マップに登録
	BUFFER_DATA_TF data = {};
	data.matWorld = transform.matWorld;
	data.worldInverse = ChoMath::Transpose(Matrix4::Inverse(transform.matWorld));
	data.rootMatrix = transform.rootMatrix;
	m_pIntegrationBuffer->UpdateData(data, transform.mapID.value());
}

void ObjectSystem::UpdateRecursive(Entity entity, std::unordered_set<Entity>& updated)
{
	if (updated.contains(entity)) return;

	auto* transform = m_pECS->GetComponent<TransformComponent>(entity);
	if (!transform) return;

	// 親がいれば先に更新
	if (transform->parentEntity)
	{
		UpdateRecursive(transform->parentEntity.value(), updated);
		auto* parentTransform = m_pECS->GetComponent<TransformComponent>(transform->parentEntity.value());
		if (parentTransform)
		{
			UpdateMatrix(*transform, parentTransform);
		} else
		{
			// 親が無効な場合はローカル変換のみ
			UpdateMatrix(*transform);
		}
	} else
	{
		// 親がいないのでローカル変換
		UpdateMatrix(*transform);
	}

	updated.insert(entity);
}

void CameraSystem::UpdateMatrix(TransformComponent& transform, CameraComponent& camera)
{
	transform.translation.z = -30.0f;
	// アフィン変換
	transform.matWorld = ChoMath::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);

	TransferMatrix(transform, camera);
}

void CameraSystem::TransferMatrix(TransformComponent& transform, CameraComponent& camera)
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
