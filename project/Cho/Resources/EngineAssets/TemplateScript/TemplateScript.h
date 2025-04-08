#pragma once
#include "IScript.h"
class { SCRIPT_NAME } : public IScript
{
public:
{SCRIPT_NAME}() = default;
~{SCRIPT_NAME}() override = default;
void Start(ScriptContext& ctx) override;
void Update(ScriptContext& ctx) override;
};
