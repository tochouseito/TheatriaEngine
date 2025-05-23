#pragma once
#include "IScript.h"

class { SCRIPT_NAME } : public IScript
{
public:
	// コンストラクタ
    { SCRIPT_NAME }(GameObject& object) : IScript(object) {}
    // 初期化処理
    void Start() override;
	// 毎フレーム処理
    void Update() override;
};