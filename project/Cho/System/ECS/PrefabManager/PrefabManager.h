#pragma once

// GameObject
#include"ECS/GameObject/GameObject.h"

// C++
#include<typeindex>

struct Prefab {
    std::unordered_map<std::type_index, std::shared_ptr<void>> components;  // コンポーネントのテンプレート
    std::vector<Prefab> children;  // 子Prefabのリスト
};

class PrefabManager {

public:
    // Prefabを登録
    void RegisterPrefab(const std::string& prefabName, const Prefab& prefab);

    // PrefabからGameObjectを作成
    GameObject* InstantiatePrefab(const std::string& prefabName, EntityManager& entityManager, ComponentManager& componentManager);

private:
    GameObject* InstantiatePrefab(const Prefab& prefab, EntityManager& entityManager, ComponentManager& componentManager);
private:
    std::unordered_map<std::string, Prefab> prefabs;  // 各Prefabを名前で管理
};

