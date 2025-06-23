#pragma once
#include "GameCore/ECS/ECSManager.h"
class Prefab
{
public:
    Prefab() = default;

	void SetName(const std::wstring& name) { m_Name = name; }
	const std::wstring& GetName() const { return m_Name; }
	void SetType(ObjectType type) { m_Type = type; }
	ObjectType GetType() const { return m_Type; }
	void SetTag(const std::string& tag) { m_Tag = tag; }
	const std::string& GetTag() const { return m_Tag; }

    //――――――――――――――――――
    // ① 事前登録：扱う型すべてに対して呼ぶ
    // Prefab::RegisterCopyFunc<YourComponent>();
    //――――――――――――――――――
    template<ComponentType T>
    static void RegisterCopyFunc()
    {
        CompID id = ECSManager::ComponentPool<T>::GetID();

        if constexpr (IsMultiComponent<T>::value)
        {
            m_MultiCopyFuncs[id] = [](Entity e, ECSManager& ecs, void* rawVec) {
                auto* vec = static_cast<std::vector<T>*>(rawVec);
                for (auto const& comp : *vec)
                {
                    T* dst = ecs.AddComponent<T>(e);
                    *dst = comp;
                }
                };
        }
        else
        {
            m_CopyFuncs[id] = [](Entity e, ECSManager& ecs, void* raw) {
                T* dst = ecs.AddComponent<T>(e);
                *dst = *static_cast<T*>(raw);
                };
        }
    }

    //――――――――――――――――――
    // ② 既存エンティティから Prefab を作る
    //――――――――――――――――――
    static Prefab FromEntity(ECSManager& ecs, Entity e)
    {
        Prefab prefab;
        const Archetype& arch = ecs.GetArchetype(e);

        for (size_t id = 0; id < arch.size(); ++id)
        {
            if (!arch.test(id)) continue;
            auto* pool = ecs.GetRawComponentPool(id);
            void* raw = pool->GetRawComponent(e);
            if (!raw) continue;

            // 深いコピーを shared_ptr<void> で受け取る
            auto clonePtr = pool->CloneComponent(id, raw);
            if (pool->IsMultiComponentTrait(id))
                prefab.m_MultiComponents[id] = std::move(clonePtr);
            else
                prefab.m_Components[id] = std::move(clonePtr);

            prefab.m_Archetype.set(id);
        }
        return prefab;
    }

    //――――――――――――――――――
    // ③ Instantiate するとき
    //――――――――――――――――――
    Entity Instantiate(ECSManager& ecs) const
    {
        Entity e = ecs.GenerateEntity();

        // 単一コンポーネントを戻す
        for (auto const& [id, raw] : m_Components)
        {
            auto it = m_CopyFuncs.find(id);
            if (it != m_CopyFuncs.end())
                it->second(e, ecs, raw.get());
        }

        // マルチコンポーネントを戻す
        for (auto const& [id, rawVec] : m_MultiComponents)
        {
            auto it = m_MultiCopyFuncs.find(id);
            if (it != m_MultiCopyFuncs.end())
                it->second(e, ecs, rawVec.get());
        }

        return e;
    }

    //――――――――――――――――――
    // ④ Prefab にコンポーネントを追加するとき
    //――――――――――――――――――
    template<ComponentType T>
    void AddComponent(const T& comp)
    {
        CompID id = ECSManager::ComponentPool<T>::GetID();
        if constexpr (IsMultiComponent<T>::value)
        {
            auto& ptr = m_MultiComponents[id];
            if (!ptr) ptr = std::make_shared<std::vector<T>>();
            auto& vec = *std::static_pointer_cast<std::vector<T>>(ptr);
            vec.push_back(comp);
        }
        else
        {
            m_Components[id] = std::make_shared<T>(comp);
        }
        m_Archetype.set(id);
    }

    //――――――――――――――――――
    // コンポーネントを 1 つだけ持つ場合の参照取得
    //――――――――――――――――――
    template<ComponentType T>
    T& GetComponent()
    {
        CompID id = ECSManager::ComponentPool<T>::GetID();
        auto it = m_Components.find(id);
        if (it == m_Components.end())
            throw std::runtime_error("Prefab にそのコンポーネントはありません");
        return *std::static_pointer_cast<T>(it->second);
    }

    //――――――――――――――――――
    // マルチコンポーネントをすべて参照
    //――――――――――――――――――
    template<ComponentType T>
    std::vector<T>& GetAllComponents()
        requires IsMultiComponent<T>::value
    {
        CompID id = ECSManager::ComponentPool<T>::GetID();
        auto it = m_MultiComponents.find(id);
        if (it == m_MultiComponents.end())
            throw std::runtime_error("Prefab にそのマルチコンポーネントはありません");
        return *std::static_pointer_cast<std::vector<T>>(it->second);
    }

    //――――――――――――――――――
    // 単一コンポーネントを上書きする
    //――――――――――――――――――
    template<ComponentType T>
    void SetComponent(const T& comp)
    {
        CompID id = ECSManager::ComponentPool<T>::GetID();
        m_Components[id] = std::make_shared<T>(comp);
        m_Archetype.set(id);
    }

    //――――――――――――――――――
    // 単一コンポーネントを Prefab から削除
    //――――――――――――――――――
    template<ComponentType T>
    void RemoveComponent()
    {
        CompID id = ECSManager::ComponentPool<T>::GetID();
        // マップから単一コンポーネントのエントリを消す
        m_Components.erase(id);
        // Archetype からビットをクリア
        m_Archetype.reset(id);
    }

    //――――――――――――――――――
    // マルチコンポーネントの特定インスタンスを Prefab から削除
    //――――――――――――――――――
    template<ComponentType T>
    void RemoveComponentInstance(size_t idx)
        requires IsMultiComponent<T>::value
    {
        CompID id = ECSManager::ComponentPool<T>::GetID();
        auto it = m_MultiComponents.find(id);
        if (it == m_MultiComponents.end()) return;

        auto& vec = *std::static_pointer_cast<std::vector<T>>(it->second);
        if (idx >= vec.size()) return;
        vec.erase(vec.begin() + idx);

        // もし空になったら Prefab から完全に消す
        if (vec.empty())
        {
            m_MultiComponents.erase(id);
            m_Archetype.reset(id);
        }
    }

    //――――――――――――――――――
    // マルチコンポーネント全体を Prefab から削除
    //――――――――――――――――――
    template<ComponentType T>
    void ClearAllComponents()
        requires IsMultiComponent<T>::value
    {
        CompID id = ECSManager::ComponentPool<T>::GetID();
        m_MultiComponents.erase(id);
        m_Archetype.reset(id);
    }

private:
    Archetype m_Archetype;
    // void ポインタで型消去したストレージ
    std::unordered_map<CompID, std::shared_ptr<void>> m_Components;
    std::unordered_map<CompID, std::shared_ptr<void>> m_MultiComponents;

    // インスタンス化用マップ
    inline static std::unordered_map<CompID,
        std::function<void(Entity, ECSManager&, void*)>> m_CopyFuncs;
    inline static std::unordered_map<CompID,
        std::function<void(Entity, ECSManager&, void*)>> m_MultiCopyFuncs;

	std::wstring m_Name = L"Prefab"; // Prefab の名前（必要なら設定可能）
    ObjectType m_Type = ObjectType::None;
	std::string m_Tag = "Default"; // Prefab のタグ（必要なら設定可能）
};