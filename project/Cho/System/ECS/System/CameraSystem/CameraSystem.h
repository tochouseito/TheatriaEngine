#pragma once

// ComponentManager
#include"ECS/ComponentManager/ComponentManager.h"

// ECS
class EntityManager;
class ComponentManager;

class CameraSystem
{
public:

	void Initialize(EntityManager& entityManager, ComponentManager& componentManager);

	// 更新
	void Update(EntityManager& entityManager, ComponentManager& componentManager);

	void UpdateEditor(EntityManager& entityManager, ComponentManager& componentManager);

	// デバッグカメラ初期化
	void InitDebugCamera(CameraComponent& camera);
	// デバッグカメラ更新
	void UpdateDebugCamera(CameraComponent& camera);
private:
	void InitMatrix(CameraComponent& compo);

	// 行列更新
    void UpdateMatrix(CameraComponent& compo);
    void TransferMatrix(CameraComponent& compo);
};

