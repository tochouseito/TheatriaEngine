#pragma once
#include"Scene/BaseScene/BaseScene.h"
class MainScene :
    public BaseScene
{
public:
    // BaseScene を介して継承されました
    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void ChangeScene() override;
};

