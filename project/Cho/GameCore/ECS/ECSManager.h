#pragma once
#include "Core/Utility/Components.h"

// コンポーネント型のみ許可する
template<typename T>
concept ComponentType = std::derived_from<T, IComponentTag>;

enum SystemState
{
	Initialize = 0,
	Update,
	Finalize,
};

class ECSManager
{
public:
    /*---------------------------------------------------------------------
        エンティティ管理
    ---------------------------------------------------------------------*/
    ECSManager() : m_NextEntityID(static_cast<Entity>(-1)) {}
    ~ECSManager() = default;

    /*-------------------- Entity create / recycle ----------------------*/
    inline const Entity GenerateEntity()
    {
        Entity entity;
        if (!m_RecycleEntities.empty())
        {
            entity = m_RecycleEntities.back();
            m_RecycleEntities.pop_back();
        }
        else { entity = ++m_NextEntityID; }

        if (m_EntityToActive.size() <= entity)
            m_EntityToActive.resize(entity + 1, false);
        m_EntityToActive[entity] = true;
        return entity;
    }

    /*-------------------- Clear all components ------------------------*/
    inline void ClearEntity(const Entity& e)
    {
        if (e >= m_EntityToArchetype.size()) { return; }
        Archetype oldArch = m_EntityToArchetype[e];
        for (CompID id = 0; id < oldArch.size(); ++id)
        {
            if (oldArch.test(id))
            {
                m_TypeToComponents[id]->RemoveComponent(e);
            }
        }
        m_ArchToEntities[oldArch].Remove(e);
        m_EntityToArchetype[e].reset();
    }

    /*-------------------- Disable entity ------------------------------*/
    inline void RemoveEntity(const Entity& entity)
    {
		ClearEntity(entity); // Clear components first
        m_EntityToActive[entity] = false;
        m_ArchToEntities[m_EntityToArchetype[entity]].Remove(entity);
        m_RecycleEntities.emplace_back(entity);
    }

    /*-------------------- Copy whole entity ---------------------------*/
    Entity CopyEntity(const Entity& src)
    {
        Archetype arch = GetArchetype(src);      // copy value
        Entity   dst = GenerateEntity();
        for (CompID id = 0; id < arch.size(); ++id)
        {
            if (arch.test(id))
            {
                m_TypeToComponents[id]->CopyComponent(src, dst);
            }
        }

        if (m_EntityToArchetype.size() <= dst)
        {
            m_EntityToArchetype.resize(dst + 1);
        }
        m_EntityToArchetype[dst] = arch;
        m_ArchToEntities[arch].Add(dst);
        return dst;
    }

    /*-------------------- Copy selected components --------------------*/
    void CopyComponents(Entity src, Entity dst, bool overwrite = true)
    {
        const Archetype& archSrc = GetArchetype(src);
        Archetype& archDst = m_EntityToArchetype[dst];
        Archetype        oldArch = archDst;

        for (CompID id = 0; id < archSrc.size(); ++id)
        {
            if (!archSrc.test(id)) continue;                // src 未保持
            bool dstHas = archDst.test(id);
            if (!overwrite && dstHas) continue;             // skip
            m_TypeToComponents[id]->CopyComponent(src, dst);
            if (!dstHas) archDst.set(id);
        }
        if (archDst != oldArch)
        {
            m_ArchToEntities[oldArch].Remove(dst);
            m_ArchToEntities[archDst].Add(dst);
        }
    }

    /*-------------------- Add component -------------------------------*/
    template<ComponentType T>
    T* AddComponent(const Entity& entity)
    {
        CompID type = ComponentPool<T>::GetID();
        auto [it, _] = m_TypeToComponents.try_emplace(type, std::make_shared<ComponentPool<T>>(4096));
        auto pool = std::static_pointer_cast<ComponentPool<T>>(it->second);
        T* comp = pool->AddComponent(entity);

        if (m_EntityToArchetype.size() <= entity)
            m_EntityToArchetype.resize(entity + 1, Archetype{});
        Archetype& arch = m_EntityToArchetype[entity];
        if (!arch.test(type))
        {
            m_ArchToEntities[arch].Remove(entity);
            arch.set(type);
            m_ArchToEntities[arch].Add(entity);
        }
        if constexpr (HasInitialize<T>)
        {
            comp->Initialize(); // Initialize if applicable
        }
        return comp;
    }

    /*-------------------- Get component -------------------------------*/
    template<ComponentType T>
    T* GetComponent(const Entity& entity)
    {
        CompID type = ComponentPool<T>::GetID();
        if (entity >= m_EntityToArchetype.size() || !m_EntityToArchetype[entity].test(type)) return nullptr;
        auto it = m_TypeToComponents.find(type);
        if (it == m_TypeToComponents.end()) return nullptr;
        auto pool = std::static_pointer_cast<ComponentPool<T>>(it->second);
        return pool->GetComponent(entity);
    }

    /*-------------------- Get all (multi) -----------------------------*/
    template<ComponentType T>
    std::vector<T>* GetAllComponents(const Entity& entity) requires IsMultiComponent<T>::value
    {
        auto it = m_TypeToComponents.find(ComponentPool<T>::GetID());
        if (it == m_TypeToComponents.end()) return nullptr;
        auto pool = std::static_pointer_cast<ComponentPool<T>>(it->second);
        return pool->GetAllComponents(entity);
    }

    /*-------------------- Remove component ----------------------------*/
    template<ComponentType T>
    void RemoveComponent(const Entity& entity)
    {
        static_assert(!IsMultiComponent<T>::value, "Use RemoveAllComponents for multi-instance.");
        CompID type = ComponentPool<T>::GetID();
        if (entity >= m_EntityToArchetype.size() || !m_EntityToArchetype[entity].test(type)) return;
        auto pool = GetComponentPool<T>();
        if (!pool) return;
        pool->RemoveComponent(entity);
        Archetype& arch = m_EntityToArchetype[entity];
        m_ArchToEntities[arch].Remove(entity);
        arch.reset(type);
        m_ArchToEntities[arch].Add(entity);
    }

    /*-------------------- Remove ALL multi ----------------------------*/
    template<ComponentType T>
    void RemoveAllComponents(const Entity& entity) requires IsMultiComponent<T>::value
    {
        auto pool = GetComponentPool<T>();
        if (pool) pool->RemoveAll(entity);
    }

    /*-------------------- Accessors ----------------------------------*/
    inline const Archetype& GetArchetype(Entity e) const
    {
        static Archetype empty{};
        return (e < m_EntityToArchetype.size()) ? m_EntityToArchetype[e] : empty;
    }

    template<ComponentType T>
    auto GetComponentPool()
    {
        auto it = m_TypeToComponents.find(ComponentPool<T>::GetID());
        return (it == m_TypeToComponents.end()) ? nullptr : static_cast<ComponentPool<T>*>(it->second.get());
    }

    /*---------------------------------------------------------------------
        EntityContainer (archetype bucket)
    ---------------------------------------------------------------------*/
    class EntityContainer
    {
    public:
        void Add(Entity e)
        {
            m_Entities.emplace_back(e);
            if (m_EntityToIndex.size() <= e) m_EntityToIndex.resize(e + 1);
            m_EntityToIndex[e] = static_cast<uint32_t>(m_Entities.size() - 1);
        }
        void Remove(Entity e)
        {
            if (e >= m_EntityToIndex.size()) return;
            uint32_t idx = m_EntityToIndex[e];
            if (idx >= m_Entities.size()) return;
            uint32_t last = static_cast<uint32_t>(m_Entities.size() - 1);
            Entity   back = m_Entities[last];
            if (e != back)
            {
                m_Entities[idx] = back;
                m_EntityToIndex[back] = idx;
            }
            m_Entities.pop_back();
        }
        const std::vector<Entity>& GetEntities() const noexcept { return m_Entities; }
    private:
        std::vector<Entity>  m_Entities;
        std::vector<uint32_t> m_EntityToIndex;
    };

    /*---------------------------------------------------------------------
        IComponentPool (interface)
    ---------------------------------------------------------------------*/
    class IComponentPool
    {
    public:
        virtual ~IComponentPool() = default;
        virtual void CopyComponent(Entity src, Entity dst) = 0;
        virtual void RemoveComponent(Entity e) = 0;
        virtual void* GetRawComponent(Entity e) const = 0;
        virtual std::shared_ptr<void> CloneComponent(CompID id, void* ptr) = 0;
        virtual bool IsMultiComponentTrait(CompID id) const = 0;
    };

    IComponentPool* GetRawComponentPool(CompID id)
    {
        auto it = m_TypeToComponents.find(id);
        return (it != m_TypeToComponents.end()) ? it->second.get() : nullptr;
    }

    /*---------------------------------------------------------------------
        ComponentPool  (vector‑backed)
    ---------------------------------------------------------------------*/
    template<ComponentType T>
    class ComponentPool : public IComponentPool
    {
        using Storage = std::vector<T>;                    // ★ vector 版
        static constexpr uint32_t kInvalid = ~0u;
    public:
        explicit ComponentPool(size_t reserveEntities = 0) { m_Storage.reserve(reserveEntities); }

        // 登録された型がマルチかどうかを返す
        bool IsMultiComponentTrait(CompID) const override
        {
            return IsMultiComponent<T>::value;
        }

        /*-------------------- add ---------------------*/
        T* AddComponent(Entity e)
        {
            if constexpr (IsMultiComponent<T>::value)
            {
                return &m_Multi[e].emplace_back();          // vector of components per entity
            }
            else
            {
                if (m_EntityToIndex.size() <= e) m_EntityToIndex.resize(e + 1, kInvalid);
                uint32_t idx = m_EntityToIndex[e];
                if (idx == kInvalid)
                {
                    idx = static_cast<uint32_t>(m_Storage.size());
                    m_Storage.emplace_back();              // default construct
                    if (m_IndexToEntity.size() <= idx) m_IndexToEntity.resize(idx + 1, kInvalid);
                    m_EntityToIndex[e] = idx;
                    m_IndexToEntity[idx] = e;
                }
                return &m_Storage[idx];
            }
        }

        /*-------------------- get ---------------------*/
        T* GetComponent(Entity e)
        {
            if constexpr (IsMultiComponent<T>::value)
            {
                auto it = m_Multi.find(e);
                return (it != m_Multi.end() && !it->second.empty()) ? &it->second.front() : nullptr;
            }
            else
            {
                if (e >= m_EntityToIndex.size()) return nullptr;
                uint32_t idx = m_EntityToIndex[e];
                return (idx != kInvalid) ? &m_Storage[idx] : nullptr;
            }
        }

        /*-------------------- remove ------------------*/
        void RemoveComponent(Entity e) override
        {
            if constexpr (IsMultiComponent<T>::value)
            {
                m_Multi.erase(e);
            }
            else
            {
                if (e >= m_EntityToIndex.size()) return;
                uint32_t idx = m_EntityToIndex[e];
                if (idx == kInvalid) return;
                uint32_t last = static_cast<uint32_t>(m_Storage.size() - 1);
                if (idx != last)
                {
                    m_Storage[idx] = std::move(m_Storage[last]);
                    Entity movedEnt = m_IndexToEntity[last];
                    m_IndexToEntity[idx] = movedEnt;
                    m_EntityToIndex[movedEnt] = idx;
                }
                m_Storage.pop_back();
                m_EntityToIndex[e] = kInvalid;
                m_IndexToEntity[last] = kInvalid;
            }
        }

        /*-------------------- multi helpers -----------*/
        std::vector<T>* GetAllComponents(Entity e) requires IsMultiComponent<T>::value
        {
            auto it = m_Multi.find(e);
            return (it != m_Multi.end()) ? &it->second : nullptr;
        }
        void RemoveAll(Entity e) requires IsMultiComponent<T>::value { m_Multi.erase(e); }

        /*-------------------- copy --------------------*/
        void CopyComponent(Entity src, Entity dst) override
        {
            if constexpr (IsMultiComponent<T>::value)
            {
                auto it = m_Multi.find(src);
                if (it == m_Multi.end() || it->second.empty()) { return; }
                // shallow copy
				auto& vecSrc = it->second;
				auto& vecDst = m_Multi[dst] = vecSrc; // copy vector
				// コピー直後にInitialize()を呼ぶ
                if constexpr (HasInitialize<T>)
                {
                    for (auto& comp : vecDst)
                    {
                        //comp.Initialize();
                        comp;
                    }
				}
            }
            else
            {
                uint32_t idxSrc = m_EntityToIndex[src];
                if (idxSrc == kInvalid) { return; }
                // すでに dst にコンポーネントがある場合
                if (dst < m_EntityToIndex.size() && m_EntityToIndex[dst] != kInvalid)
                {
                    auto& c = m_Storage[m_EntityToIndex[dst]];
                    c = m_Storage[idxSrc];
                    if constexpr (HasInitialize<T>)
                    {
                        //c.Initialize(); // Initialize if applicable
                    }
                    return;
                }

                // 新規にコピーする場合
                m_Storage.emplace_back(m_Storage[idxSrc]);
                auto& newComp = m_Storage.back();
                newComp;
                if constexpr (HasInitialize<T>)
                {
                    //newComp.Initialize(); // Initialize if applicable
                }

                uint32_t idxDst = static_cast<uint32_t>(m_Storage.size() - 1);
                if (m_EntityToIndex.size() <= dst)
                {
                    m_EntityToIndex.resize(dst + 1, kInvalid);
                }
                if (m_IndexToEntity.size() <= idxDst)
                {
                    m_IndexToEntity.resize(idxDst + 1, kInvalid);
                }
                m_EntityToIndex[dst] = idxDst;
                m_IndexToEntity[idxDst] = dst;
            }
        }

        /*-------------------- static ID --------------*/
        static CompID GetID() { static CompID id = ++ECSManager::m_NextCompTypeID; return id; }

        /*-------------------- expose map -------------*/
        auto& Map() { return m_Multi; }
        const auto& Map() const { return m_Multi; }

        // 単一コンポーネントを void* で取得
        void* GetRawComponent(Entity e) const override
        {
            if constexpr (IsMultiComponent<T>::value)
            {
                auto it = m_Multi.find(e);
                return (it != m_Multi.end() && !it->second.empty()) ? (void*)&it->second : nullptr;
            }
            else
            {
                if (e >= m_EntityToIndex.size()) return nullptr;
                uint32_t idx = m_EntityToIndex[e];
                return (idx != kInvalid) ? (void*)&m_Storage[idx] : nullptr;
            }
        }

        // 任意の型を shared_ptr<void> に包んでPrefabにコピー
        std::shared_ptr<void> CloneComponent(CompID, void* ptr) override
        {
            if constexpr (IsMultiComponent<T>::value)
            {
                auto src = static_cast<std::vector<T>*>(ptr);
                return std::make_shared<std::vector<T>>(*src); // Deep copy
            }
            else
            {
                T* src = static_cast<T*>(ptr);
                return std::make_shared<T>(*src); // Deep copy
            }
        }
    private:
        Storage                         m_Storage;          // dense
        std::vector<uint32_t>           m_EntityToIndex;    // entity -> index
        std::vector<Entity>             m_IndexToEntity;    // index  -> entity
        std::unordered_map<Entity, std::vector<T>> m_Multi; // multi‑instance
    };

    class ISystem
    {
    public:
        ISystem() = default;
        virtual void Update(ECSManager* ecs) = 0;
        virtual ~ISystem() = default;
        virtual int GetPriority() const { return priority; }
        virtual void SetPriority(int p) { priority = p; }
        virtual bool IsEnabled() const { return enabled; }
        virtual void SetEnabled(bool e) { enabled = e; }
    protected:
        uint32_t priority = 0;// 優先度
        bool enabled = true;// 有効フラグ
    };

    /*---------------------------------------------------------------------
        System & MultiSystem  (unchanged logic)
    ---------------------------------------------------------------------*/
    template<ComponentType... T>
    class System : public ISystem
    {
        static constexpr bool kNoMulti = (!IsMultiComponent<T>::value && ...);
        static_assert(kNoMulti, "System<T...> cannot include multi-instance components");
    public:
        using FuncType = std::function<void(Entity, T&...)>;
        explicit System(FuncType f) : m_Func(f) { (m_Required.set(ComponentPool<T>::GetID()), ...); }
        void Update(ECSManager* ecs) override
        {
            for (auto& [arch, bucket] : ecs->GetArchToEntities())
                if ((arch & m_Required) == m_Required)
                    for (Entity e : bucket.GetEntities())
                        m_Func(e, *ecs->GetComponent<T>(e)...);
        }
        const Archetype& GetRequired() const { return m_Required; }
    private:
        Archetype m_Required; FuncType m_Func;
    };

    // マルチコンポーネントシステムの基底クラス
    class IMultiSystem
    {
    public:
        virtual ~IMultiSystem() = default;
        virtual void Update(ECSManager* ecs) = 0;
        virtual int GetPriority() const { return priority; }
        virtual void SetPriority(int p) { priority = p; }
        virtual bool IsEnabled() const { return enabled; }
        virtual void SetEnabled(bool e) { enabled = e; }
    protected:
        int priority = 0;
        bool enabled = true;
    };

    template<ComponentType T>
        requires IsMultiComponent<T>::value
    class MultiComponentSystem : public IMultiSystem
    {
        using FuncType = std::function<void(Entity, std::vector<T>&)>;
    public:
        explicit MultiComponentSystem(FuncType f) : m_Func(std::move(f)) {}
        void Update(ECSManager* ecs) override
        {
            auto* pool = ecs->GetComponentPool<T>(); if (!pool) return;
            for (auto& [e, comps] : pool->Map()) if (!comps.empty()) m_Func(e, comps);
        }
    private:
        FuncType m_Func;
    };

    std::unordered_map<Archetype, EntityContainer>& GetArchToEntities() { return m_ArchToEntities; }

private:
    /*-------------------- data members --------------------------------*/
    std::unordered_map<CompID, std::shared_ptr<IComponentPool>> m_TypeToComponents;
    std::vector<Archetype> m_EntityToArchetype;
    std::vector<bool>      m_EntityToActive;
    std::unordered_map<Archetype, EntityContainer> m_ArchToEntities;
    Entity                 m_NextEntityID = 0;
    std::vector<Entity>    m_RecycleEntities;
    static inline CompID   m_NextCompTypeID = 0;
};