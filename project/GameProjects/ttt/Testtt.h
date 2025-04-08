#pragma once
#include "IScript.h"
class Testtt : public IScript
{
public:
Testtt() = default;
~Testtt() override = default;
void Start(ScriptContext& ctx) override;
void Update(ScriptContext& ctx) override;
};
