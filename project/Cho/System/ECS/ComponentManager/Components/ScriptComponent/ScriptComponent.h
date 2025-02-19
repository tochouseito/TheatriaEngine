#pragma once

#include <Windows.h>
#include <string>
#include <iostream>
#include <functional>
#include"Generator/ScriptProject/ScriptProject.h"
#include"Script/IScript/IScript.h"
#include"Script/ScriptStatus.h"
#include"SystemState/SystemState.h"

struct ScriptComponent {
public:
    HMODULE dllHandle=nullptr;

    uint32_t id=0;
    uint32_t type = 0;
    ScriptStatus status;
    ComponentManager* ptr = nullptr;

    using ScriptFunc = std::function<void(uint32_t, uint32_t, ComponentManager*)>; // スクリプト関数型

    ScriptFunc startFunc;  // Start関数
    ScriptFunc updateFunc; // Update関数
    std::function<void()> cleanupFunc; // 解放関数

    bool isScript = false;
};

