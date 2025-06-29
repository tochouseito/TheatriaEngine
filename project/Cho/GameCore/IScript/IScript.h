#pragma once
#include "GameCore/GameObject/GameObject.h"
#define USE_CHOENGINE_SCRIPT
#include "ChoEngineAPI.h"
class GameObject;
class CHO_API IScript
{
    friend class ScriptInstanceGenerateSystem;
    friend class ScriptSystem;
public:
	IScript(GameObject& object) : gameObject(object) {}
    virtual ~IScript() = default;
    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void OnCollisionEnter(GameObject&) {}
    virtual void OnCollisionStay(GameObject&) {}
    virtual void OnCollisionExit( GameObject&) {}
    GameObject* FindGameObjectByName(std::wstring_view name);
    /*template<typename T>
    static std::vector<ScriptMember> BuildReflectedMembers()
    {
        std::vector<ScriptMember> result;
        T::RegisterMembers(result);
        return result;
    }
    template<typename T>
    static std::vector<ScriptFunction> BuildReflectedFunctions()
    {
        std::vector<ScriptFunction> result;
        T::RegisterFunctions(result);
        return result;
    }*/
protected:
	GameObject& gameObject;
	TransformComponent* transform = nullptr;
};
