#pragma once
#include "Cho/Core/Utility/FVector.h"
#include "Cho/Core/Utility/FArray.h"
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <functional>
#include <bitset>

using Entity = uint32_t;
using CompID = size_t;
using Archetype = std::bitset<128>;

// エンティティを管理するためのコンテナクラス
class EntityContainer
{
public:
	// エンティティを追加
	inline void Add(const size_t a_entity)
	{
		m_Entities.emplace_back(a_entity);
		if (m_EntityToIndex.size() < a_entity)
		{
			m_EntityToIndex.resize(a_entity + 1);
		}
		m_EntityToIndex[a_entity] = m_Entities.size() - 1;
	}
	// エンティティを削除
	inline void Remove(const size_t a_entity)
	{
		if (m_EntityToIndex.size() < a_entity)
		{
			return;
		}
		size_t backIndex = m_Entities.size() - 1;
		size_t backEntity = m_Entities.back();
		size_t removeIndex = m_EntityToIndex[a_entity];
		// 削除する要素が最後の要素でなければ
		if (a_entity != m_Entities.back())
		{
			m_Entities[removeIndex] = backEntity;
			m_EntityToIndex[backIndex] = removeIndex;
		}
		// 最後尾のEntityを削除
		m_Entities.pop_back();
	}
private:
	std::vector<size_t> m_Entities;
	std::vector<size_t> m_EntityToIndex;
public:
	inline const std::vector<size_t>& GetEntities()const noexcept
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
template<typename CompType>
class ComponentPool :public IComponentPool
{
public:
	// コンテナのメモリを確保
	ComponentPool(const size_t a_size)
		:m_Components(a_size)
	{
	}
	// コンポーネントを追加
	inline CompType* AddComponent(const size_t a_entity)
	{
		if (m_Components.size() < a_entity)
		{
			m_Components.resize(a_entity, CompType());
		}
		m_Components[a_entity] = CompType();
		return &m_Components[a_entity];
	}

	// コンポーネントを取得する
	inline CompType* GetComponent(const size_t a_entity)noexcept
	{
		// エンティティが有効なら
		if (m_Components.size() >= a_entity)
		{
			return &m_Components[a_entity];
		}
		return nullptr;
	}
private:
	friend class ECSManager;
	// コンポーネントのインスタンスを管理するコンテナ
	std::vector<CompType> m_Components;
	// このコンテナクラスが管理するコンポーネントが持つ一意なID
	static size_t m_CompTypeID;
public:
	// CompTypeIDを取得する関数
	static inline const size_t GetID()
	{
		// この関数を初めて読んだ時にIDを発行
		if (!ComponentPool<CompType>::m_CompTypeID)
		{
			ComponentPool<CompType>::m_CompTypeID = ++ECSManager::m_NextCompTypeID;
		}
		return ComponentPool<CompType>::m_CompTypeID;
	}
};

class ECSManager
{
public:
	// エンティティを新規作成
	inline const size_t GenerateEntity()
	{
		size_t nEntity;
		// リサイクル待ちエンティティがあればそこから1つ取り出す
		if (m_RecycleEntities.size())
		{
			nEntity = m_RecycleEntities.back();
			m_RecycleEntities.pop_back();
		}
		// 無ければ新規発行
		else
		{
			nEntity = ++m_NextID;
		}
		// エンティティを有効にする
		if (m_EntityToActive.size() < nEntity)
		{
			m_EntityToActive.resize(nEntity + 1, false);
		}
		m_EntityToActive[nEntity] = true;
		// 生成したエンティティを返す
		return nEntity;
	}

	// コンポーネントを追加する
	template<typename T>
	T* AddComponent(const size_t a_entity)
	{
		// コンポーネントの型のIDを取得
		size_t type = ComponentPool<T>::GetID();
		// 初めて生成するコンポーネントなら
		if (!m_TypeToComponents[type])
		{
			// ComponentPoolを実体化する
			m_TypeToComponents[type] = std::make_shared<ComponentPool<T>>(4096);
		}
		// 追加するコンポーネントを格納するコンテナクラスを取得
		std::shared_ptr<ComponentPool<T>> spCompPool = std::static_pointer_cast<ComponentPool<T>>(m_TypeToComponents[type]);
		// コンポーネントを追加し取得
		T* pResultComp = spCompPool->AddComponent(a_entity);

		// コンポーネントを追加する前のアーキタイプを取得
		Archetype arch;
		if (m_EntityToArchetype.size() > a_entity)
		{
			arch = m_EntityToArchetype[a_entity];
		} else
		{
			m_EntityToArchetype.resize(a_entity + 1, Archetype());
		}
		// 前アーキタイプのエンティティリストからエンティティを削除
		m_ArcheTypeToEntities[arch].Remove(a_entity);
		// アーキタイプを編集
		arch.set(type);
		// 新しいアーキタイプをセット
		m_ArcheTypeToEntities[arch].Add(a_entity);
		m_EntityToArchetype[a_entity] = arch;
		// 追加したコンポーネントを返す
		return pResultComp;
	}

	// コンポーネントを削除する
	template<typename T>
	void RemvoeComponent(const size_t a_entity)
	{
		// コンポーネントの型のIDを取得
		size_t type = ComponentPool<T>::GetID();

		Archetype arch;
		// コンポーネントを削除する前のアーキタイプを取得
		if (m_EntityToArchetype.size() > a_entity)
		{
			arch = m_EntityToArchetype[a_entity];
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
		m_ArcheTypeToEntities[arch].Remove(a_entity);
		// アーキタイプを編集
		arch.reset(type);
		// 新しいアーキタイプをセット
		m_ArcheTypeToEntities[arch].Add(a_entity);
		m_EntityToArchetype[a_entity] = arch;
	}

	// エンティティを無効にする
	inline void RemoveEntity(const size_t a_entity)
	{
		m_EntityToActive[a_entity] = false;
		m_ArcheTypeToEntities[m_EntityToArchetype[a_entity]].Remove(a_entity);
		m_RecycleEntities.emplace_back(a_entity);
	}

	// 処理を実行する
	template<typename ...T>
	void RunFunction(std::function<void(T&...)> a_func)
	{
		// 処理に必要なコンポーネントのアーキタイプを取得
		Archetype arch;
		(arch.set(ComponentPool<T>::GetID()), ...);
		// 処理に必要なアーキタイプを含むアーキタイプを持つエンティティのリストを検索
		for (auto&& entities : m_ArcheTypeToEntities)
		{
			if ((entities.first & arch) == arch)
			{
				for (auto&& entity : entities.second.GetEntities())
				{
					a_func(std::static_pointer_cast<ComponentPool<T>>(m_TypeToComponents[ComponentPool<T>::GetID()])->m_vComponents[entity]...);
				}
			}
		}
	}

private:
	// コンポーネントをタイプ別で管理するコンテナ
	std::unordered_map<size_t, std::shared_ptr<IComponentPool>> m_TypeToComponents;
	// エンティティとアーキタイプを紐づけるコンテナ
	std::vector<Archetype> m_EntityToArchetype;
	// エンティティと有効フラグを紐づけるコンテナ
	std::vector<bool> m_EntityToActive;
	// エンティティをアーキタイプごとに分割したコンテナ
	std::unordered_map<Archetype, EntityContainer> m_ArcheTypeToEntities;
	// 次に生成するエンティティのID
	size_t m_NextID = 0;
	// 再利用待ちのEntityのリスト
	std::vector<size_t> m_RecycleEntities;
	// 次に発行するCompTypeID
	static size_t m_NextCompTypeID;
public:

};

// スタティック変数の初期化
size_t ECSManager::m_NextCompTypeID = 0;
template<typename CompType>
size_t ComponentPool<CompType>::m_CompTypeID = 0;
