#pragma once
#include "GameCore/ScriptSystem/ScriptContext.h"
class IScript
{
public:
    virtual ~IScript() = default;
    virtual void Start(const ScriptContext& ctx) = 0;
    virtual void Update(const ScriptContext& ctx) = 0;
};
