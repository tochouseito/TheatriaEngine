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
#include "{SCRIPT_NAME}.h"

void{ SCRIPT_NAME }::Start(ScriptContext& ctx)
{
    // 初期化処理
}

void{ SCRIPT_NAME }::Update(ScriptContext& ctx)
{
    // 毎フレーム処理
}

extern "C" __declspec(dllexport) IScript* Create { SCRIPT_NAME }Script()
{
    return new{ SCRIPT_NAME }();
}