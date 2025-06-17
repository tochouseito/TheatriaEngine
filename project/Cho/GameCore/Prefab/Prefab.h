#pragma once
#include "GameCore/ECS/ECSManager.h"
class Prefab
{
    template<typename T>
    using TypeVec = std::vector<T>;
public:
    Prefab() = default;

    /*------------------ AddComponent ------------------*/
    template<ComponentType T>
    void AddComponent(const T& component)
    {
        CompID id = ECSManager::ComponentPool<T>::GetID();

        if constexpr (IsMultiComponent<T>::value)
        {
            auto& ptr = m_MultiComponents[id];
            if (!ptr) { ptr = std::make_shared<TypeVec<T>>(); }
            auto& vec = *std::static_pointer_cast<TypeVec<T>>(ptr);
            vec.push_back(component);
            m_Archetype.set(id);
        }
        else
        {
            m_Components[id] = std::make_shared<T>(component);
            m_Archetype.set(id);
        }
    }


    /*------------------ Instantiate -------------------*/
    Entity Instantiate(ECSManager& ecs) const
    {
        Entity e = ecs.GenerateEntity();

        // 通常の単一コンポーネント
        for (const auto& [id, compPtr] : m_Components)
        {
            auto it = m_CopyFuncs.find(id);
            if (it != m_CopyFuncs.end())
            {
                it->second(e, ecs, compPtr.get());
            }
        }

        // マルチコンポーネント
        for (const auto& [id, ptr] : m_MultiComponents)
        {
            auto it = m_MultiCopyFuncs.find(id);
            if (it != m_MultiCopyFuncs.end())
            {
                // Tの型情報がないので事前登録した関数で受け取り側でキャストする
                it->second(e, ecs, ptr.get());
            }
        }

        return e;
    }

    /*------------------ 登録関数 ------------------*/
    template<ComponentType T>
    static void RegisterCopyFunc()
    {
        CompID id = ECSManager::ComponentPool<T>::GetID();

        if constexpr (IsMultiComponent<T>::value)
        {
            m_MultiCopyFuncs[id] = [](Entity e, ECSManager& ecs, void* ptr) {
                auto* vec = static_cast<std::vector<T>*>(ptr);
                for (const auto& comp : *vec)
                {
                    *ecs.AddComponent<T>(e) = comp;
                }
                };
        }
        else
        {
            m_CopyFuncs[id] = [](Entity e, ECSManager& ecs, void* ptr) {
                T* dst = ecs.AddComponent<T>(e);
                *dst = *static_cast<T*>(ptr);
                };
        }
    }

    static Prefab FromEntity(ECSManager& ecs, Entity e)
    {
        Prefab prefab;
        const Archetype& arch = ecs.GetArchetype(e);

        for (CompID id = 0; id < arch.size(); ++id)
        {
            if (!arch.test(id)) { continue; }

            auto* pool = ecs.GetRawComponentPool(id);
            if (!pool) { continue; }

            void* comp = pool->GetRawComponent(e);
            if (!comp) { continue; }

            if (pool->IsMultiComponentTrait(id))
            {
                prefab.m_MultiComponents[id] = pool->CloneComponent(id, comp);
            }
            else
            {
                prefab.m_Components[id] = pool->CloneComponent(id, comp);
            }

            prefab.m_Archetype.set(id);
        }

        return prefab;
    }

private:
    Archetype m_Archetype;

    // 通常のコンポーネント：1つだけ保持
    std::unordered_map<CompID, std::shared_ptr<void>> m_Components;
    std::unordered_map<CompID, std::shared_ptr<void>> m_MultiComponents;

    // コピー関数
    inline static std::unordered_map<CompID, std::function<void(Entity, ECSManager&, void*)>> m_CopyFuncs;
    inline static std::unordered_map<CompID, std::function<void(Entity, ECSManager&, void*)>> m_MultiCopyFuncs;
};