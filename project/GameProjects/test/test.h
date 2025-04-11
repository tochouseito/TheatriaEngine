#pragma once
#include "IScript.h"
class test : public IScript
{
public:
test() = default;
~test() override = default;
void Start(ScriptContext& ctx) override;
void Update(ScriptContext& ctx) override;
};
