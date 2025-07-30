#pragma once

#include "d3_body.h"
#include <memory>

namespace physics::d3
{
	enum class d3Backend
	{
		bullet,  // Bullet Physicsを使用
		chophysics // ChoPhysicsを使用
	};

	class Id3Shape;

	class Id3World
	{
		public:
		virtual ~Id3World() = default;
		// 物理ワールドのバックエンドを取得
		d3Backend GetBackend() const { return backend; }
		// シュミレーションのステップ
		virtual void Step(const float& deltaTime) = 0;
		// ボディを作成
		virtual Id3Body* CreateBody(const Id3BodyDef& bodyDef) = 0;
		// ボディを削除
		virtual void DestroyBody(Id3Body* body) = 0;
	protected:
		d3Backend backend; // 物理エンジンのバックエンド
		std::vector<std::unique_ptr<Id3Body>> bodies; // ワールド内のボディリスト
	};

	class bulletWorld : public Id3World
	{
	public:
		bulletWorld();
		~bulletWorld() override;
		void Step(const float& deltaTime) override;
		Id3Body* CreateBody(const Id3BodyDef& bodyDef)override;
		void DestroyBody(Id3Body* body)override;
	private:
		struct Impl;
		std::unique_ptr<Impl> impl;
	};

	class chophysicsWorld : public Id3World
	{
	public:
		chophysicsWorld() {}
		~chophysicsWorld() override {}
		void Step(const float&) override {}
		Id3Body* CreateBody(const Id3BodyDef&) override { return nullptr; } // ChoPhysicsの実装ではボディを作成しない
		void DestroyBody(Id3Body*) override {} // ChoPhysicsの実装ではボディを削除しない
	};

	Id3World* CreateWorld(d3Backend backend);
	void DestroyWorld(Id3World* world) noexcept;
}

