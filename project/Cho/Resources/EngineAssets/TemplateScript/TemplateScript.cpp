#include "IScript.h"
using namespace ChoSystem;

class { SCRIPT_NAME } : public IScript
{
public:

    { SCRIPT_NAME }(GameObject& object) : IScript(object)
    {
	    // コンストラクタ
    }

    void Start() override
    {
        // 初期化処理
    }

    void Update() override
    {
        // 毎フレーム処理
    }
};

REGISTER_SCRIPT_FACTORY({ SCRIPT_NAME });
