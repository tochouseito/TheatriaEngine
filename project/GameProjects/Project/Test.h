#pragma once
#include "IScript.h"
class Test : public IScript
{
public:
Test() = default;
~Test() override = default;
void Start(ScriptContext& ctx) override;
void Update(ScriptContext& ctx) override;
};
