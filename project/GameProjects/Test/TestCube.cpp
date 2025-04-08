#include "TestCube.h"

voidTestCube::Start(ScriptContext& ctx)
{
    // 初期化処理
}

voidTestCube::Update(ScriptContext& ctx)
{
    // 毎フレーム処理
}

extern "C" __declspec(dllexport) IScript* Create TestCubeScript()
{
    return newTestCube();
}
