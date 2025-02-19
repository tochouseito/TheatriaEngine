#include "PrecompiledHeader.h"
#include "PrefabManager.h"
// Prefabを登録

void PrefabManager::RegisterPrefab(const std::string& prefabName, const Prefab& prefab) {
    prefabs[prefabName] = prefab;
}

// PrefabからGameObjectを作成

GameObject* PrefabManager::InstantiatePrefab(const std::string& prefabName, EntityManager& entityManager, ComponentManager& componentManager) {
    if (prefabs.find(prefabName) != prefabs.end()) {
        const Prefab& prefab = prefabs[prefabName];
        return InstantiatePrefab(prefab, entityManager, componentManager);
    }
    return nullptr;
}

GameObject* PrefabManager::InstantiatePrefab(const Prefab& prefab, EntityManager& entityManager, ComponentManager& componentManager) {
    GameObject* gameObject = new GameObject();

    gameObject->SetManager(&entityManager, &componentManager);

    gameObject->CreateEntity();

    // Prefabの各コンポーネントを新しいエンティティに追加
    for (const auto& [type, component] : prefab.components) {
        if (type == typeid(TransformComponent)) {
            gameObject->AddComponent(*std::static_pointer_cast<TransformComponent>(component));
        } else if (type == typeid(RenderComponent)) {
            gameObject->AddComponent(*std::static_pointer_cast<RenderComponent>(component));
        }
        // 他のコンポーネントも同様に処理
    }

    // 子Prefabのインスタンス化と親子関係の設定
    for (const Prefab& childPrefab : prefab.children) {
        GameObject* child = InstantiatePrefab(childPrefab, entityManager, componentManager);
        child->SetParent(gameObject);
    }

    return gameObject;
}
