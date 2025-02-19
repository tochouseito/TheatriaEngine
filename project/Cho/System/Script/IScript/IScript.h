#pragma once

#include<cstdint>
class ComponentManager;

class IScript {
public:
    virtual ~IScript() = default;

    // スクリプトの初期化
    virtual void Start() = 0;

    // スクリプトの毎フレーム処理
    virtual void Update() = 0;

    // エンティティ情報の設定
    virtual void SetEntityInfo(uint32_t id, uint32_t type,ComponentManager* ptr) = 0;
};