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

REGISTER_SCRIPT_FACTORY({ SCRIPT_NAME });
