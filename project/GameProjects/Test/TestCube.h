#pragma once
#include "IScript.h"
class TestCube : public IScript
{
public:
TestCube() = default;
~TestCube() override = default;
void Start(ScriptContext& ctx) override;
void Update(ScriptContext& ctx) override;
};
