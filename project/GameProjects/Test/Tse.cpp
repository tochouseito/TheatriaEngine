#include "Tse.h"

void Tse::Start(ScriptContext& ctx)
{
    // 初期化処理
}

void Tse::Update(ScriptContext& ctx)
{
    // 毎フレーム処理
	ctx.transform->degrees.x += 1.0f;
}

extern "C" __declspec(dllexport) IScript* CreateTseScript()
{
    return new Tse();
}
