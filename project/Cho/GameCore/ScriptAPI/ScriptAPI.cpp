#include "pch.h"
#include "ScriptAPI.h"
#include "GameCore/ECS/ECSManager.h"

void ScriptContext::InitializeTransformAPI()
{
	if (!m_Entity || !m_ECS || !m_ResourceManager)
	{
		transform.data = nullptr;
		transform.SetPosition = [](Entity, const Vector3&) {};
		transform.GetPosition = [](Entity) -> Vector3 { return {}; };
		return;
	}

	// TransformComponent を取得
	TransformComponent* tf = m_ECS->GetComponent<TransformComponent>(*m_Entity);
	transform.data = tf;

	// 関数でのアクセスも一応用意（なくても良い場合は省略OK）
	if (tf)
	{
		transform.SetPosition = [this](Entity e, const Vector3& v) {
			if (auto* t = m_ECS->GetComponent<TransformComponent>(e))
			{
				t->translation = v;
			}
			};
		transform.GetPosition = [this](Entity e) -> Vector3 {
			if (auto* t = m_ECS->GetComponent<TransformComponent>(e))
			{
				return t->translation;
			}
			return {};
			};
	} else
	{
		// 存在しないときは安全な空実装
		transform.SetPosition = [](Entity, const Vector3&) {};
		transform.GetPosition = [](Entity) -> Vector3 { return {}; };
	}
}

void ScriptContext::InitializeCameraAPI()
{
	if (!m_Entity || !m_ECS || !m_ResourceManager)
	{
		camera.data = nullptr;
		return;
	}
	// CameraComponent を取得
	CameraComponent* cam = m_ECS->GetComponent<CameraComponent>(*m_Entity);
	camera.data = cam;
	if (cam)
	{
		
	}
}

void ScriptContext::InitializeLineRendererAPI()
{
	if (!m_Entity || !m_ECS || !m_ResourceManager)
	{
		lineRenderer.data = nullptr;
		return;
	}
	// LineRendererComponent を取得
	auto* line = m_ECS->GetAllComponents<LineRendererComponent>(*m_Entity);
	lineRenderer.data = line;
	if (line)
	{

	}
}

void ScriptContext::InitializeRigidbody2DAPI()
{
	if (!m_Entity || !m_ECS || !m_ResourceManager)
	{
		rigidbody2D.data = nullptr;
		return;
	}
	// Rigidbody2DComponent を取得
	Rigidbody2DComponent* rb = m_ECS->GetComponent<Rigidbody2DComponent>(*m_Entity);
	rigidbody2D.data = rb;
	if (rb)
	{
		rigidbody2D.Reflect = [this](const b2Vec2& incident, const b2Vec2& normal) -> b2Vec2 {
			if (auto* t = m_ECS->GetComponent<Rigidbody2DComponent>(*m_Entity))
			{
				return incident - 2.0f * b2Dot(incident, normal) * normal;
			}
			return {};
			};
		rigidbody2D.RaycastWithReflections = [this](std::vector<b2Vec2>& outHits, const b2Vec2& start, const b2Vec2& dir, float maxLength) {
				if (auto* t = m_ECS->GetComponent<Rigidbody2DComponent>(*m_Entity))
				{
					outHits.clear(); // 必ず初期化してから使う

					b2Vec2 currentStart = start;
					b2Vec2 currentDir = dir;
					float remainingLength = maxLength;

					for (int i = 0; i < 2; ++i) // 2回反射まで
					{
						Rigidbody2DAPI::RayCastCallback callback;
						b2Vec2 end = currentStart + remainingLength * currentDir;
						rigidbody2D.data->world->RayCast(&callback, currentStart, end);

						if (callback.hit)
						{
							outHits.push_back(callback.point);

							float hitDistance = remainingLength * callback.fraction;
							remainingLength -= hitDistance;
							currentStart = callback.point;
							currentDir = rigidbody2D.Reflect(currentDir, callback.normal);
						} else
						{
							currentDir *= remainingLength;
							// 反射しない最終点を追加
							outHits.push_back(currentStart + currentDir);
							break;
						}
					}
				}
			};
		rigidbody2D.RaycastOnce = [this](const b2Vec2& start, const b2Vec2& dir, float maxLength, b2Vec2& outHitPoint) {
			if (auto* t = m_ECS->GetComponent<Rigidbody2DComponent>(*m_Entity))
			{
				if (!rigidbody2D.data || !rigidbody2D.data->world) return false;
				b2Vec2 end = start + (maxLength * dir);
				Rigidbody2DAPI::RayCastCallback callback;
				rigidbody2D.data->world->RayCast(&callback, start, end);

				if (callback.hit)
				{
					outHitPoint = callback.point;
					rigidbody2D.m_LastHitNormal = callback.normal;
					return true;
				}

				// ヒットしなかった場合、直進の終点を返す
				outHitPoint = end;
				rigidbody2D.m_LastHitNormal = b2Vec2(0.0f, 1.0f); // デフォルト上向き
				return false;
			}
			return false;
			};
	}
}
//
//b2Vec2 Rigidbody2DAPI::Reflect(const b2Vec2& incident, const b2Vec2& normal)
//{
//	return incident - 2.0f * b2Dot(incident, normal) * normal;
//}
//
//std::vector<b2Vec2> Rigidbody2DAPI::RaycastWithReflections(const b2Vec2& start, const b2Vec2& dir, float maxLength)
//{
//	std::vector<b2Vec2> hitPoints;
//	b2Vec2 currentStart = start;
//	b2Vec2 currentDir = dir;
//	float remainingLength = maxLength;
//
//	for (int i = 0; i < 2; ++i) // 2回反射まで
//	{
//		RayCastCallback callback;
//		b2Vec2 end = currentStart + remainingLength * currentDir;
//
//		data->world->RayCast(&callback, currentStart, end);
//
//		if (callback.hit)
//		{
//			hitPoints.push_back(callback.point);
//
//			float hitDistance = remainingLength * callback.fraction;
//			remainingLength -= hitDistance;
//
//			currentStart = callback.point;
//			currentDir = Reflect(currentDir, callback.normal);
//		} else
//		{
//			currentDir *= remainingLength;
//			// 当たらなければ直進で終了
//			hitPoints.push_back(currentStart + currentDir);
//			break;
//		}
//	}
//
//	return hitPoints;
//}
