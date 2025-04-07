#pragma once
#include "GameCore/IScript/IScript.h"
#include "GameCore/GameCore.h"
class ScriptSystem
{
public:
	// Constructor
	ScriptSystem() = default;
	// Destructor
	~ScriptSystem() = default;

    ScriptContext MakeScriptContext(Entity entity, ECSManager* ecs)
    {
        ScriptContext ctx;
        
		ctx.transform = ecs->GetComponent<TransformComponent>(entity);
		ctx.camera = ecs->GetComponent<CameraComponent>(entity);
		ctx.meshFilter = ecs->GetComponent<MeshFilterComponent>(entity);
		ctx.meshRenderer = ecs->GetComponent<MeshRendererComponent>(entity);

        return ctx;
    }
};

