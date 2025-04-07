#pragma once
#include "IScript.h"
class { SCRIPT_NAME } : public IScript
{
public:
{SCRIPT_NAME}() = default;
~{SCRIPT_NAME}() override = default;
void Start(const ScriptContext& ctx) override;
void Update(const ScriptContext& ctx) override;
};
