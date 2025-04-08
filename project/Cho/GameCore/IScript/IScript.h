#pragma once
#include "GameCore/ScriptContext/ScriptContext.h"
class IScript
{
public:
    virtual ~IScript() = default;
    virtual void Start(ScriptContext& ctx) = 0;
    virtual void Update(ScriptContext& ctx) = 0;
};
