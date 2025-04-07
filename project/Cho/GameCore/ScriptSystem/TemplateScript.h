#pragma once

#include "IScript.h"

class { SCRIPT_NAME } : public IScript
{
private:
    uint32_t entityId = 0;
    uint32_t entityType = 0;
    ComponentManager* componentPtr = nullptr;

public:
{SCRIPT_NAME}() = default;
~{SCRIPT_NAME}() override = default;

void Start() override;
void Update() override;
void SetEntityInfo(uint32_t id, uint32_t type, ComponentManager * ptr) override;
};
