#ifndef D2_WORLD_H
#define D2_WORLD_H

#include <chomath.h>
#include <memory>

// box2d, ChoPhysics
class b2WorldId;

namespace physics
{
	namespace d2
	{
		class Id2Body;
		class Id2ContactListener;
		struct Id2BodyDef;

		enum class d2Backend
		{
			box2d,  // Box2Dを使用
			chophysics // ChoPhysicsを使用
		};

		// 2D物理エンジンの共通定義や関数をここに追加

		class Id2World
		{
		public:
			virtual ~Id2World() = default;
			// 物理ワールドのバックエンドを取得
			d2Backend GetBackend() const { return backend; }
			// 物理ボディの作成
			virtual Id2Body* CreateBody(const Id2BodyDef& bodyDef) = 0;
			// 物理ボディの削除
			void DestroyBody(Id2Body* body);
			// シュミレーションのステップ
			virtual void Step(const float& deltaTime) = 0;
			// 重力を取得、設定
			virtual Vector2 GetGravity() const = 0;
			virtual void SetGravity(const Vector2& gravity) = 0;
		protected:
			d2Backend backend; // 物理エンジンのバックエンド
		};

		// Box2D
		class box2dWorld : public Id2World
		{
		public:
			box2dWorld(d2Backend be);
			~box2dWorld();
			// Id2World を介して継承されました
			Id2Body* CreateBody(const Id2BodyDef& bodyDef) override;
			void Step(const float& deltaTime) override;
			Vector2 GetGravity() const override;
			void SetGravity(const Vector2& gravity) override;
		private:
			b2WorldId* world; // Box2Dのワールド
		};

		// ChoPhysics
		class choPhysicsWorld : public Id2World
		{
		public:
			choPhysicsWorld(d2Backend be) { backend = be; }
			// Id2World を介して継承されました
			Id2Body* CreateBody(const Id2BodyDef& bodyDef) override;
			void Step(const float& deltaTime) override;
			Vector2 GetGravity() const override;
			void SetGravity(const Vector2& gravity) override;
		};

		// ワールド作成
		Id2World* CreateWorld(d2Backend backend = d2Backend::box2d);
		// ワールド削除
		void DestroyWorld(Id2World* world);
	}
}

#endif // D2_WORLD_H
