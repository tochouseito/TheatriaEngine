#include "{SCRIPT_NAME}.h"

void{ SCRIPT_NAME }::Start()
{
    // 初期化処理
}

void{ SCRIPT_NAME }::Update()
{
    // 毎フレーム処理
}

void{ SCRIPT_NAME }::SetEntityInfo(uint32_t id, uint32_t type, ComponentManager* ptr)
{
    entityId = id;
    entityType = type;
    componentPtr = ptr;
}

extern "C" __declspec(dllexport) IScript* Create { SCRIPT_NAME }Script()
{
    return new{ SCRIPT_NAME }();
}
