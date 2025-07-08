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
	Marionnette(GameObject& object) : gameObject(object) {}
    virtual ~Marionnette() = default;
    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void OnCollisionEnter(GameObject&) {}
    virtual void OnCollisionStay(GameObject&) {}
    virtual void OnCollisionExit(GameObject&) {}
    template<Cho::ComponentInterface::Type T>
    T GetComponent() const
    {
        T comp(gameObject.GetHandle().entity, ecsManager);
        return comp;
    }
	template<Cho::ComponentInterface::MarionnetteInterface T>
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
    }
protected:
	GameObject& gameObject;
	TransformComponent* transform = nullptr;
private:
	void SetECSPtr(ECSManager* ecs) { ecsManager = ecs; }
	ECSManager* ecsManager = nullptr;
};
