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
