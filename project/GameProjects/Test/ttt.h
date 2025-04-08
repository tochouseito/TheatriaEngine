#pragma once
#include "IScript.h"
class ttt : public IScript
{
public:
ttt() = default;
~ttt() override = default;
void Start(ScriptContext& ctx) override;
void Update(ScriptContext& ctx) override;
};
