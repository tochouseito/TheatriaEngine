#include "ttt.h"

void ttt::Start(ScriptContext& ctx)
{
    // 初期化処理
}

void ttt::Update(ScriptContext& ctx)
{
    // 毎フレーム処理
}

extern "C" __declspec(dllexport) IScript* Create ttt Script()
{
    return newttt();
}
