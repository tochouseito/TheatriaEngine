#pragma once
#include <vector>
#include <array>
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <functional>
#include <bitset>
#include "Cho/Core/Utility/Components.h"

using Entity = uint32_t;
using CompID = size_t;
using Archetype = std::bitset<256>;

class ECSManager
{
public:
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
		if (m_EntityToActive.size() < entity)
		{
			m_EntityToActive.resize(entity + 1, false);
		}
		m_EntityToActive[entity] = true;
		// 生成したエンティティを返す
		return entity;
	}

	// コンポーネントを追加する
	template<typename T>
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
		// 追加したコンポーネントを返す
		return pResultComp;
	}

	// コンポーネントを取得する
	template<typename T>
	T* GetComponent(const Entity& entity)
	{
		CompID type = ComponentPool<T>::GetID();

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

	// コンポーネントを削除する
	template<typename T>
	void RemvoeComponent(const Entity& entity)
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

	// エンティティを無効にする
	inline void RemoveEntity(const Entity& entity)
	{
		m_EntityToActive[entity] = false;
		m_ArchToEntities[m_EntityToArchetype[entity]].Remove(entity);
		m_RecycleEntities.emplace_back(entity);
	}

	// 処理を実行する
	template<typename ...T>
	void RunFunction(std::function<void(T&...)> func)
	{
		// 処理に必要なコンポーネントのアーキタイプを取得
		Archetype arch;
		(arch.set(ComponentPool<T>::GetID()), ...);
		// 処理に必要なアーキタイプを含むアーキタイプを持つエンティティのリストを検索
		for (auto&& entities : m_ArchToEntities)
		{
			if ((entities.first & arch) == arch)
			{
				for (auto&& entity : entities.second.GetEntities())
				{
					func(std::static_pointer_cast<ComponentPool<T>>(m_TypeToComponents[ComponentPool<T>::GetID()])->m_Components[entity]...);
				}
			}
		}
	}

	// エンティティを管理するためのコンテナクラス
	class EntityContainer
	{
	public:
		// エンティティを追加
		inline void Add(const Entity& entity)
		{
			m_Entities.emplace_back(entity);
			if (m_EntityToIndex.size() < entity)
			{
				m_EntityToIndex.resize(entity + 1);
			}
			m_EntityToIndex[entity] = static_cast<uint32_t>(m_Entities.size() - 1);
		}
		// エンティティを削除
		inline void Remove(const Entity& entity)
		{
			if (m_EntityToIndex.size() < entity)
			{
				return;
			}
			size_t backIndex = m_Entities.size() - 1;
			Entity backEntity = m_Entities.back();
			uint32_t removeIndex = m_EntityToIndex[entity];
			// 削除する要素が最後の要素でなければ
			if (entity != m_Entities.back())
			{
				m_Entities[removeIndex] = backEntity;
				m_EntityToIndex[backIndex] = removeIndex;
			}
			// 最後尾のEntityを削除
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
	private:
	};

	// コンポーネントを管理するコンテナクラス
	template<typename T>
	class ComponentPool :public IComponentPool
	{
	public:
		// コンテナのメモリを確保
		ComponentPool(const size_t size)
			:m_Components(size)
		{
		}
		// コンポーネントを追加
		inline T* AddComponent(const Entity& entity)
		{
			if (m_Components.size() < entity)
			{
				m_Components.resize(entity, T());
			}
			m_Components[entity] = T();
			return &m_Components[entity];
		}

		// コンポーネントを取得する
		inline T* GetComponent(const Entity& entity)noexcept
		{
			// エンティティが有効なら
			if (m_Components.size() >= entity)
			{
				return &m_Components[entity];
			}
			return nullptr;
		}
	private:
		friend class ECSManager;
		// コンポーネントのインスタンスを管理するコンテナ
		std::vector<T> m_Components;
		// このコンテナクラスが管理するコンポーネントが持つ一意なID
		static CompID m_CompTypeID;
	public:
		// CompTypeIDを取得する関数
		static inline const CompID GetID()
		{
			// この関数を初めて読んだ時にIDを発行
			if (!ComponentPool<T>::m_CompTypeID)
			{
				ComponentPool<T>::m_CompTypeID = ++ECSManager::m_NextCompTypeID;
			}
			return ComponentPool<T>::m_CompTypeID;
		}
	};

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
	static CompID m_NextCompTypeID;
public:

};

// Initialize static member
template<typename CompType>
size_t ECSManager::ComponentPool<CompType>::m_CompTypeID = 0;