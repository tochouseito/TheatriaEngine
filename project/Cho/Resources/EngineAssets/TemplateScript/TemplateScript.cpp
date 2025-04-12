#include "IScript.h"

class { SCRIPT_NAME } : public IScript
{
public:

    void Start(ScriptContext & ctx) override
    {
        // 初期化処理
    }

    void Update(ScriptContext & ctx) override
    {
        // 毎フレーム処理
    }
};

extern "C" __declspec(dllexport) IScript* Create{ SCRIPT_NAME }Script()
{
    return new { SCRIPT_NAME }();
}
