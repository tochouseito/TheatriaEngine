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
	// Constructor
	ECSManager()
		: m_NextEntityID(static_cast<Entity>(-1))
	{
	}
	// Destructor
	~ECSManager()
	{

	}

	// エンティティを新規作成
	inline const Entity GenerateEntity()
	{
		Entity entity;
		// リサイクル待ちエンティティがあればそこから1つ取り出す
		if (m_RecycleEntities.size())
		{
			entity = m_RecycleEntities.back();
			m_RecycleEntities.pop_back();
		}
		// 無ければ新規発行
		else
		{
			entity = ++m_NextEntityID;
		}
		// エンティティを有効にする
		if (m_EntityToActive.size() <= entity)
		{
			m_EntityToActive.resize(entity + 1, false);
		}
		m_EntityToActive[entity] = true;
		// 生成したエンティティを返す
		return entity;
	}

	// Entityをコピーして新しいEntityを生成する
	Entity CopyEntity(const Entity& src)
	{
		const Archetype& arch = GetArchetype(src);
		Entity dst = GenerateEntity();

		for (CompID id = 0; id < arch.size(); ++id)
		{
			if (arch.test(id))
			{
				m_TypeToComponents[id]->CloneComponent(src, dst);
			}
		}
		// Archetype / ArchToEntities の更新
		m_EntityToArchetype[dst] = arch;
		m_ArchToEntities[arch].Add(dst);
		return dst;
	}

	// 既存Entity間でコンポーネントをコピーする
	void CopyComponents(Entity src, Entity dst, bool overwrite = true)
	{
		const Archetype& archSrc = GetArchetype(src);
		Archetype& archDst = m_EntityToArchetype[dst];

		for (CompID id = 0; id < archSrc.size(); ++id)
		{
			if (!archSrc.test(id)) continue;
			if (!overwrite && archDst.test(id)) continue; // 既存は保持

			m_TypeToComponents[id]->CloneComponent(src, dst);
			archDst.set(id);
		}
	}

	// コンポーネントの全削除
	inline void ClearEntity(const Entity& e)
	{
		if (e >= m_EntityToArchetype.size()) return;
		Archetype oldArch = m_EntityToArchetype[e];
		for (CompID id = 0; id < oldArch.size(); ++id)
		{
			if (oldArch.test(id))
			{
				m_TypeToComponents[id]->RemoveComponent(e);
			}
		}
		m_ArchToEntities[oldArch].Remove(e);
		m_EntityToArchetype[e].reset();          // からっぽ
	}

	// コンポーネントを追加する
	template<ComponentType T>
	T* AddComponent(const Entity& entity)
	{
		// コンポーネントの型のIDを取得
		CompID type = ComponentPool<T>::GetID();
		// 初めて生成するコンポーネントなら
		if (!m_TypeToComponents[type])
		{
			// ComponentPoolを実体化する
			m_TypeToComponents[type] = std::make_shared<ComponentPool<T>>(4096);
		}
		// 追加するコンポーネントを格納するコンテナクラスを取得
		std::shared_ptr<ComponentPool<T>> spCompPool = std::static_pointer_cast<ComponentPool<T>>(m_TypeToComponents[type]);
		// コンポーネントを追加し取得
		T* pResultComp = spCompPool->AddComponent(entity);
		if constexpr (!IsMultiComponent<T>::value)
		{
			// コンポーネントを追加する前のアーキタイプを取得
			Archetype arch;
			if (m_EntityToArchetype.size() > entity)
			{
				arch = m_EntityToArchetype[entity];
			} else
			{
				m_EntityToArchetype.resize(entity + 1, Archetype());
			}
			// 前アーキタイプのエンティティリストからエンティティを削除
			m_ArchToEntities[arch].Remove(entity);
			// アーキタイプを編集
			arch.set(type);
			// 新しいアーキタイプをセット
			m_ArchToEntities[arch].Add(entity);
			m_EntityToArchetype[entity] = arch;
		}
		// 追加したコンポーネントを返す
		return pResultComp;
	}

	// コンポーネントを取得する
	template<ComponentType T>
	T* GetComponent(const Entity& entity)
	{
		CompID type = ComponentPool<T>::GetID();

		//エンティティがそのコンポーネントを所有しているか確認
		if (entity >= m_EntityToArchetype.size() || !m_EntityToArchetype[entity].test(type))
		{
			return nullptr;
		}

		// コンポーネントのマップに存在するかチェック
		auto it = m_TypeToComponents.find(type);
		if (it != m_TypeToComponents.end())
		{
			// 取得したComponentPoolを適切な型にキャスト
			std::shared_ptr<ComponentPool<T>> spCompPool = std::static_pointer_cast<ComponentPool<T>>(it->second);

			// エンティティのコンポーネントを取得
			return spCompPool->GetComponent(entity);
		}

		// 該当するコンポーネントが存在しない場合は nullptr を返す
		return nullptr;
	}

	// コンポーネントを取得する（マルチコンポーネント用）
	template<ComponentType T>
	std::vector<T>* GetAllComponents(const Entity& entity)
		requires IsMultiComponent<T>::value
	{
		auto it = m_TypeToComponents.find(ComponentPool<T>::GetID());
		if (it == m_TypeToComponents.end())
		{
			return nullptr;
		}
		auto pool = std::static_pointer_cast<ComponentPool<T>>(it->second);
		return pool->GetAllComponents(entity);
	}

	// コンポーネントを削除する
	template<ComponentType T>
	void RemoveComponent(const Entity& entity)
	{
		if constexpr (IsMultiComponent<T>::value)
		{
			// 単一コンポーネントは許可しない
			static_assert(!IsMultiComponent<T>::value, "Use RemoveAllComponents() for multi-instance components.");
		} else
		{
			// コンポーネントの型のIDを取得
			CompID type = ComponentPool<T>::GetID();

			Archetype arch;
			// コンポーネントを削除する前のアーキタイプを取得
			if (m_EntityToArchetype.size() > entity)
			{
				arch = m_EntityToArchetype[entity];
			} else
			{
				return;
			}
			// エンティティが削除するコンポーネントを持ってなかったらreturn
			if (!arch.test(type))
			{
				return;
			}
			// 前アーキタイプのエンティティリストからエンティティを削除
			m_ArchToEntities[arch].Remove(entity);
			// アーキタイプを編集
			arch.reset(type);
			// 新しいアーキタイプをセット
			m_ArchToEntities[arch].Add(entity);
			m_EntityToArchetype[entity] = arch;
		}
	}

	// コンポーネントをすべて削除する(マルチコンポーネント用)
	template<ComponentType T>
	void RemoveAllComponents(const Entity& entity)
		requires IsMultiComponent<T>::value
	{
		// 単一コンポーネントは許可しない
		static_assert(IsMultiComponent<T>::value, "RemoveAllComponents is only for multi-instance components.");
		auto it = m_TypeToComponents.find(ComponentPool<T>::GetID());
		if (it == m_TypeToComponents.end()) { return; }

		auto pool = std::static_pointer_cast<ComponentPool<T>>(it->second);
		pool->RemoveAll(entity);
	}

	// エンティティを無効にする
	inline void RemoveEntity(const Entity& entity)
	{
		m_EntityToActive[entity] = false;
		m_ArchToEntities[m_EntityToArchetype[entity]].Remove(entity);
		m_RecycleEntities.emplace_back(entity);
	}

	inline const Archetype& GetArchetype(Entity entity) const
	{
		static Archetype empty{};
		if (entity < m_EntityToArchetype.size())
		{
			return m_EntityToArchetype[entity];
		}
		return empty;
	}

	// エンティティを管理するためのコンテナクラス
	class EntityContainer
	{
	public:
		// エンティティを追加
		inline void Add(const Entity& entity)
		{
			m_Entities.emplace_back(entity);
			if (m_EntityToIndex.size() <= entity)
			{
				m_EntityToIndex.resize(entity + 1);
			}
			m_EntityToIndex[entity] = static_cast<uint32_t>(m_Entities.size() - 1);
		}
		// エンティティを削除
		inline void Remove(const Entity& entity)
		{
			// 削除対象が空 or 登録されていなければ return
			if (m_Entities.empty() || entity >= m_EntityToIndex.size())
			{
				return;
			}

			uint32_t removeIndex = m_EntityToIndex[entity];
			uint32_t backIndex = static_cast<uint32_t>(m_Entities.size() - 1);
			Entity backEntity = m_Entities[backIndex];

			if (entity != backEntity)
			{
				m_Entities[removeIndex] = backEntity;
				m_EntityToIndex[backEntity] = removeIndex;
			}

			m_Entities.pop_back();
		}
	private:
		std::vector<Entity> m_Entities;
		std::vector<uint32_t> m_EntityToIndex;
	public:
		inline const std::vector<Entity>& GetEntities()const noexcept
		{
			return m_Entities;
		}
	};

	// ComponentPoolを同じコンテナで扱うための基底クラス
	class IComponentPool
	{
	public:
		virtual void CloneComponent(Entity src, Entity dst) = 0;
		virtual void RemoveComponent(Entity e) = 0;
		virtual ~IComponentPool() = default;
	private:
	};

	// コンポーネントを管理するコンテナクラス
	template<ComponentType T, size_t ChunkSize = 1024>
	class ComponentPool :public IComponentPool
	{
		using Storage = cho::chunk_vector<T, ChunkSize>;
	public:
		// コンテナのメモリを確保
		ComponentPool(const size_t& reserveEntities = 0)
		{
			// 余裕を持ってチャンク確保（任意）
			size_t chunks = (reserveEntities + ChunkSize - 1) / ChunkSize;
			storage_.reserve_chunks(chunks);
		}
		// コンポーネントを追加
		T* AddComponent(const Entity& e)
		{
			if constexpr (IsMultiComponent<T>::value)
			{
				return &multi_[e].emplace_back();          // vector と同じ感覚
			}
			else
			{
				if (entityToIndex_.size() <= e) entityToIndex_.resize(e + 1, kInvalid);
				// すでに所持していれば上書き
				uint32_t idx = entityToIndex_[e];
				if (idx == kInvalid)
				{
					idx = storage_.emplace_back();         // 新規確保
					entityToIndex_[e] = idx;
				}
				return &storage_[idx];
			}
		}

		// コンポーネントを取得する
		T* GetComponent(const Entity& e)
		{
			if constexpr (IsMultiComponent<T>::value)
			{
				auto it = multi_.find(e);
				return (it != multi_.end() && !it->second.empty()) ? &it->second.front() : nullptr;
			}
			else
			{
				if (e >= entityToIndex_.size()) return nullptr;
				uint32_t idx = entityToIndex_[e];
				return idx != kInvalid ? &storage_[idx] : nullptr;
			}
		}

		// コンポーネントを削除する
		void RemoveComponent(const Entity& e) override
		{
			if constexpr (IsMultiComponent<T>::value)
			{
				multi_.erase(e);
			}
			else
			{
				if (e >= entityToIndex_.size()) return;
				uint32_t idx = entityToIndex_[e];
				if (idx != kInvalid)
				{
					storage_.erase_swap_and_pop(idx);      // O(1) 削除
					entityToIndex_[e] = kInvalid;
				}
			}
		}

		//// すべて取得する（マルチコンポーネント用）
		//std::vector<T>* GetAllComponents(const Entity& entity)
		//	requires IsMultiComponent<T>::value// 単一コンポーネントの場合は使用できない
		//{
		//	auto it = m_Multi.find(entity);
		//	if (it != m_Multi.end())
		//	{
		//		return &it->second;
		//	}
		//	return nullptr;
		//}

		//// コンポーネントを削除する（マルチコンポーネント用）
		//void RemoveAll(const Entity& entity)
		//	requires IsMultiComponent<T>::value
		//{
		//	m_Multi.erase(entity); // そのEntityに属するすべてのTを削除
		//}


		//// CompTypeIDを取得する関数
		//static CompID GetID()
		//{
		//	// この関数を初めて読んだ時にIDを発行
		//	if (!m_CompTypeID)
		//	{
		//		m_CompTypeID = ++ECSManager::m_NextCompTypeID;
		//	}
		//	return m_CompTypeID;
		//}

		// コンポーネントのコピー
		void CopyComponent(const Entity& src, const Entity& dst) override
		{
			if constexpr (IsMultiComponent<T>::value)
			{
				multi_[dst] = multi_[src];
			}
			else
			{
				uint32_t idxSrc = entityToIndex_[src];
				if (idxSrc == kInvalid) return;
				if (entityToIndex_.size() <= dst) entityToIndex_.resize(dst + 1, kInvalid);

				uint32_t idxDst = storage_.emplace_back(storage_[idxSrc]);
				entityToIndex_[dst] = idxDst;
			}
		}

	private:
		friend class ECSManager;
		/* 単一コンポーネント用ストレージ */
		Storage storage_;
		std::vector<uint32_t> entityToIndex_;
		static constexpr uint32_t kInvalid = ~0u;
		/* マルチコンポーネント用ストレージ */
		std::unordered_map<Entity, Storage> multi_;
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

	template<ComponentType... T>
	class System : public ISystem
	{
	public:
		using FuncType = std::function<void(Entity, T&...)>;

		// マルチコンポーネントは対象外
		static_assert((!IsMultiComponent<T>::value && ...),
			"System cannot be used with multi-instance components like LineRendererComponent");

		System(FuncType func)
			: m_Func(func)
		{
			// m_Required をここでセット
			(m_Required.set(ECSManager::ComponentPool<T>::GetID()), ...);
		}

		void Update(ECSManager* ecs) override
		{
			for (const auto& [arch, container] : ecs->GetArchToEntities())
			{
				if ((arch & m_Required) == m_Required)
				{
					std::vector<Entity> copy = container.GetEntities();  // ← コピーを取得
					for (Entity e : copy)
					{
						m_Func(e, *ecs->GetComponent<T>(e)...);
					}
				}
			}
		}

		const Archetype& GetRequired() const { return m_Required; }

	protected:
		Archetype m_Required;
	private:
		FuncType m_Func;
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

	// マルチコンポーネント専用システム
	template<ComponentType T>
		requires IsMultiComponent<T>::value// マルチコンポーネントのみ
	class MultiComponentSystem : public IMultiSystem
	{
	public:
		using FuncType = std::function<void(Entity, std::vector<T>&)>;

		MultiComponentSystem(FuncType func) : m_Func(func) {}

		void Update(ECSManager* ecs) override
		{
			for (const auto& [arch, container] : ecs->GetArchToEntities())
			{
				for (Entity e : container.GetEntities())
				{
					auto* comps = ecs->GetAllComponents<T>(e);
					if (comps && !comps->empty())
					{
						m_Func(e, *comps);
					}
				}
			}
		}

	private:
		FuncType m_Func;
	};



	std::unordered_map<Archetype, EntityContainer>& GetArchToEntities()
	{
		return m_ArchToEntities;
	}
private:
	// コンポーネントをタイプ別で管理するコンテナ
	std::unordered_map<CompID, std::shared_ptr<IComponentPool>> m_TypeToComponents;
	// エンティティとアーキタイプを紐づけるコンテナ
	std::vector<Archetype> m_EntityToArchetype;
	// エンティティと有効フラグを紐づけるコンテナ
	std::vector<bool> m_EntityToActive;
	// エンティティをアーキタイプごとに分割したコンテナ
	std::unordered_map<Archetype, EntityContainer> m_ArchToEntities;
	// 次に生成するエンティティのID
	Entity m_NextEntityID = 0;
	// 再利用待ちのEntityのリスト
	std::vector<Entity> m_RecycleEntities;
	// 次に発行するCompTypeID
	static inline CompID m_NextCompTypeID;
public:

};

//// Initialize static member
//template<ComponentType T>
//size_t ECSManager::ComponentPool<T>::m_CompTypeID = 0;