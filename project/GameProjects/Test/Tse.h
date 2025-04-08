#pragma once
#include "IScript.h"
class Tse : public IScript
{
public:
Tse() = default;
~Tse() override = default;
void Start(ScriptContext& ctx) override;
void Update(ScriptContext& ctx) override;
};
