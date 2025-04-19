#pragma once
#include "GameCore/ScriptAPI/ScriptAPI.h"
#define USE_CHOENGINE_SCRIPT
#include "ChoEngineAPI.h"
class IScript
{
public:
    virtual ~IScript() = default;
    virtual void Start(ScriptContext& ctx) = 0;
    virtual void Update(ScriptContext& ctx) = 0;
    virtual void OnCollisionEnter(ScriptContext& ctx, ScriptContext& other) { ctx;other; }
    virtual void OnCollisionStay(ScriptContext& ctx, ScriptContext& other) { ctx;other; }
    virtual void OnCollisionExit(ScriptContext& ctx, ScriptContext& other) { ctx;other; }
};
