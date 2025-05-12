#include "pch.h"
#include "GameObject.h"
#include "GameCore/ECS/ECSManager.h"
#include "Platform/InputManager/InputManager.h"
#include "GameCore/ObjectContainer/ObjectContainer.h"
#include "GameCore/PhysicsEngine/PhysicsEngine.h"

void GameObject::InitializeTransformAPI(bool isParentReset)
{
	if (!m_ECS || !m_ResourceManager)
	{
		transform.data = nullptr;
		transform.SetPosition = [](Entity, const Vector3&) {};
		transform.GetPosition = [](Entity) -> Vector3 { return {}; };
		return;
	}

	// TransformComponent を取得
	TransformComponent* tf = m_ECS->GetComponent<TransformComponent>(m_Entity);
	transform.data = tf;
	if (isParentReset)
	{
		tf->parent.reset();
	}

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

void GameObject::InitializeCameraAPI()
{
	if (!m_ECS || !m_ResourceManager)
	{
		camera.data = nullptr;
		return;
	}
	// CameraComponent を取得
	CameraComponent* cam = m_ECS->GetComponent<CameraComponent>(m_Entity);
	camera.data = cam;
	if (cam)
	{

	}
}

void GameObject::InitializeLineRendererAPI()
{
	if (!m_ECS || !m_ResourceManager)
	{
		lineRenderer.data = nullptr;
		return;
	}
	// LineRendererComponent を取得
	auto* line = m_ECS->GetAllComponents<LineRendererComponent>(m_Entity);
	lineRenderer.data = line;
	if (line)
	{

	}
}

void GameObject::InitializeRigidbody2DAPI()
{
	if (!m_ECS || !m_ResourceManager)
	{
		rigidbody2D.data = nullptr;
		return;
	}
	// Rigidbody2DComponent を取得
	Rigidbody2DComponent* rb = m_ECS->GetComponent<Rigidbody2DComponent>(m_Entity);
	rigidbody2D.data = rb;
	if (rb)
	{
		rigidbody2D.Reflect = [this](const b2Vec2& incident, const b2Vec2& normal) -> b2Vec2 {
			if (auto* t = m_ECS->GetComponent<Rigidbody2DComponent>(m_Entity))
			{
				return incident - 2.0f * b2Dot(incident, normal) * normal;
			}
			return {};
			};
		rigidbody2D.RaycastWithReflectionsOnce = [this](const b2Vec2& start, const b2Vec2& dir, const int ReflectionCount, const float maxLength,const std::string hitTag) -> b2Vec2 {
			b2Vec2 resultPoint = {};
			if (auto* t = m_ECS->GetComponent<Rigidbody2DComponent>(m_Entity))
			{
				b2Vec2 currentStart = start;
				b2Vec2 currentDir = dir;
				float remainingLength = maxLength;

				for (int i = 0; i < ReflectionCount; ++i)
				{
					RayCastCallback callback(m_ObjectContainer, hitTag);
					b2Vec2 end = currentStart + remainingLength * currentDir;
					rigidbody2D.data->world->RayCast(&callback, currentStart, end);

					if (callback.hit)
					{
						resultPoint = callback.point;

						float hitDistance = remainingLength * callback.fraction;
						remainingLength -= hitDistance;
						currentStart = callback.point;
						currentDir = rigidbody2D.Reflect(currentDir, callback.normal);
					} else
					{
						currentDir *= remainingLength;
						// 反射しない最終点を追加
						resultPoint = currentStart + currentDir;
						break;
					}
				}
			}
			return resultPoint;
			};
		rigidbody2D.MovePosition = [this](const Vector2& position) {
			if (auto* t = m_ECS->GetComponent<Rigidbody2DComponent>(m_Entity))
			{
				if (t->runtimeBody)
				{
					t->requestedPosition = b2Vec2(position.x, position.y);
				}
			}
			};
		rigidbody2D.Linecast = [this](const Vector2& start, const Vector2& end,const std::string hitTag) -> GameObject& {
			if (auto* t = m_ECS->GetComponent<Rigidbody2DComponent>(m_Entity))
			{
				RayCastCallback callback(m_ObjectContainer,hitTag);
				t->world->RayCast(&callback, b2Vec2(start.x, start.y), b2Vec2(end.x, end.y));
				if (callback.hit)
				{
					ObjectID id = static_cast<ObjectID>(callback.fixture->GetBody()->GetUserData().pointer);
					return m_ObjectContainer->GetGameObject(id);
				}
			}
			return m_ObjectContainer->GetDummyGameObject();
			};
	}
}

void GameObject::InitializeBoxCollider2DAPI()
{
	if (!m_ECS || !m_ResourceManager)
	{
		boxCollider2D.data = nullptr;
		return;
	}
	// BoxCollider2DComponent を取得
	BoxCollider2DComponent* box = m_ECS->GetComponent<BoxCollider2DComponent>(m_Entity);
	boxCollider2D.data = box;
	if (box)
	{
	}
}

void GameObject::InitializeMaterialAPI()
{
	if (!m_ECS || !m_ResourceManager)
	{
		material.data = nullptr;
		return;
	}
	// MaterialComponent を取得
	MaterialComponent* mat = m_ECS->GetComponent<MaterialComponent>(m_Entity);
	material.data = mat;
	if (mat)
	{
		
	}
}

void GameObject::InitializeInputAPI()
{
	if (!m_InputManager)
	{
		input.data = nullptr;
		return;
	}
	input.data = m_InputManager;
	input.PushKey = [this](const uint8_t& keyNumber) -> bool {
		return m_InputManager->PushKey(keyNumber);
		};
	input.TriggerKey = [this](const uint8_t& keyNumber) -> bool {
		return m_InputManager->TriggerKey(keyNumber);
		};
	input.GetAllMouse = [this]() -> const DIMOUSESTATE2& {
		return m_InputManager->GetAllMouse();
		};
	input.IsPressMouse = [this](const int32_t& mouseNumber) -> bool {
		return m_InputManager->IsPressMouse(mouseNumber);
		};
	input.IsTriggerMouse = [this](const int32_t& buttonNumber) -> bool {
		return m_InputManager->IsTriggerMouse(buttonNumber);
		};
	input.GetMouseMove = [this]() -> MouseMove {
		return m_InputManager->GetMouseMove();
		};
	input.GetMouseWindowPosition = [this]() -> const Vector2& {
		return m_InputManager->GetMouseWindowPosition();
		};
	input.GetMouseScreenPosition = [this]() -> Vector2 {
		return m_InputManager->GetMouseScreenPosition();
		};
	input.GetJoystickState = [this](const int32_t& stickNo, XINPUT_STATE& out) -> bool {
		return m_InputManager->GetJoystickState(stickNo, out);
		};
	input.GetJoystickStatePrevious = [this](const int32_t& stickNo, XINPUT_STATE& out) -> bool {
		return m_InputManager->GetJoystickStatePrevious(stickNo, out);
		};
	input.SetJoystickDeadZone = [this](const int32_t& stickNo, const int32_t& deadZoneL, const int32_t& deadZoneR) {
		m_InputManager->SetJoystickDeadZone(stickNo, deadZoneL, deadZoneR);
		};
	input.GetNumberOfJoysticks = [this]() -> size_t {
		return m_InputManager->GetNumberOfJoysticks();
		};
	input.IsTriggerPadButton = [this](const PadButton& button, int32_t stickNo) -> bool {
		return m_InputManager->IsTriggerPadButton(button, stickNo);
		};
	input.IsPressPadButton = [this](const PadButton& button, int32_t stickNo) -> bool {
		return m_InputManager->IsPressPadButton(button, stickNo);
		};
	input.GetStickValue = [this](const LR& padStick, int32_t stickNo) -> Vector2 {
		return m_InputManager->GetStickValue(padStick, stickNo);
		};
	input.GetLRTrigger = [this](const LR& LorR, int32_t stickNo) -> float {
		return m_InputManager->GetLRTrigger(LorR, stickNo);
		};
}
