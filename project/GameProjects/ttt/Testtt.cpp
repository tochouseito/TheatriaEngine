#include "Testtt.h"

void Testtt::Start(ScriptContext& ctx)
{
    // 初期化処理
}

void Testtt::Update(ScriptContext& ctx)
{
    // 毎フレーム処理
}

extern "C" __declspec(dllexport) IScript* Create TestttScript()
{
    return newTesttt();
}
