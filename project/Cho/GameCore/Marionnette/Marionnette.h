#pragma once
#include "GameCore/GameObject/GameObject.h"
#include "Core/Utility/Components.h"
#define USE_CHOENGINE_SCRIPT
#include "ChoEngineAPI.h"
class GameObject;
class CHO_API Marionnette : public IComponentTag
{
    friend class ScriptInstanceGenerateSystem;
    friend class ScriptSystem;
public:
	Marionnette(GameObject& object,ECSManager* ecs) : gameObject(object),ecsManager(ecs) {}
    virtual ~Marionnette() = default;
    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void OnCollisionEnter(GameObject&) {}
    virtual void OnCollisionStay(GameObject&) {}
    virtual void OnCollisionExit(GameObject&) {}
    template<Cho::ComponentInterface::Type T>
    T GetComponent() const
    {
        T comp(ecsManager);
        if constexpr (std::is_base_of_v<Marionnette, T>)
        {
            auto* script = ecsManager->GetComponent<ScriptComponent>(gameObject.GetHandle().entity);
            return script ? static_cast<T*>(script->instance) : nullptr;
        }
        else
        {
			return ecsManager->GetComponent<T>(gameObject.GetHandle().entity);
        }
    }
protected:
	GameObject& gameObject;
	TransformComponent* transform = nullptr;
private:
	ECSManager* ecsManager = nullptr;
};
