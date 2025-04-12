#include "Test.h"

void Test::Start(ScriptContext& ctx)
{
    // 初期化処理
}

void Test::Update(ScriptContext& ctx)
{
    // 毎フレーム処理
    ctx.transform.rotation().x += 1.0f;
}

extern "C" __declspec(dllexport) IScript* CreateTestScript()
{
    return new Test();
}
