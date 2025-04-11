#include "test.h"

void test::Start(ScriptContext& ctx)
{
    // 初期化処理
    LineRendererComponent* lineRenderer = ctx.AddLineRenderer();
	lineRenderer->line.start = Vector3(0, 0, 0);
	lineRenderer->line.end = Vector3(1, 0, 0);
	lineRenderer->line.color = Color(1, 0, 0, 1);
}

void test::Update(ScriptContext& ctx)
{
    // 毎フレーム処理
}

extern "C" __declspec(dllexport) IScript* CreatetestScript()
{
    return new test();
}
