#pragma once
#include "GameCore/GameObject/GameObject.h"
#include "Core/Utility/Components.h"
#define USE_CHOENGINE_SCRIPT
#include "ChoEngineAPI.h"
class CHO_API Marionnette
{
    friend class ScriptInstanceGenerateSystem;
    friend class ScriptSystem;
public:
    Marionnette(GameObject& object) : gameObject(object),transform(object.GetHandle().entity, nullptr)
    {
	}
    virtual ~Marionnette() = default;
    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void OnCollisionEnter(GameObject&) {}
    virtual void OnCollisionStay(GameObject&) {}
    virtual void OnCollisionExit(GameObject&) {}
    template<ChoSystem::Type T>
    T GetComponent() const
    {
        T comp(gameObject.GetHandle().entity, ecsManager);
        return comp;
    }
	/*template<cho::ComponentInterface::MarionnetteInterface T>
    T* GetMarionnette() const
    {
        if(ScriptComponent* script = ecsManager->GetComponent<ScriptComponent>(gameObject.GetHandle().entity))
        {
            return static_cast<T*>(script->instance);
        }
        else
        {
			return nullptr;
        }
    }*/
public:
	GameObject& gameObject;
	ChoSystem::Transform transform;
private:
	void SetECSPtr(ECSManager* ecs) 
    { 
        ecsManager = ecs;
		transform.m_ECS = ecs;
    }
	ECSManager* ecsManager = nullptr;
};
