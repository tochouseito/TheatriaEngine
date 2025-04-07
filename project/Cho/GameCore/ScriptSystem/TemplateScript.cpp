#include "{SCRIPT_NAME}.h"

void{ SCRIPT_NAME }::Start(const ScriptContext& ctx)
{
    // 初期化処理
}

void{ SCRIPT_NAME }::Update(const ScriptContext& ctx)
{
    // 毎フレーム処理
}

extern "C" __declspec(dllexport) IScript* Create { SCRIPT_NAME }Script()
{
    return new{ SCRIPT_NAME }();
}
