#ifndef D2_WORLD_H
#define D2_WORLD_H

#include <chomath.h>
#include <memory>

// box2d, ChoPhysics
struct b2WorldId;

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
			// ワールド作成
			virtual void Create() = 0;
			// ワールド削除
			virtual void Destroy() = 0;
			// シュミレーションのステップ
			virtual void Step(const float& deltaTime,const uint32_t& subStepCount) = 0;
			// 重力を取得、設定
			virtual Vector2 GetGravity() const = 0;
			virtual void SetGravity(const Vector2& gravity) = 0;
		protected:
			d2Backend backend; // 物理エンジンのバックエンド
		};

		// Box2D
		class box2dWorld : public Id2World
		{
			friend class box2dBody;
		public:
			box2dWorld(d2Backend be);
			~box2dWorld();
			// Id2World を介して継承されました
			void Create() override;
			void Destroy() override;
			void Step(const float& deltaTime, const uint32_t& subStepCount) override;
			Vector2 GetGravity() const override;
			void SetGravity(const Vector2& gravity) override;
			b2WorldId GetWorld() const; // Box2Dのワールドを取得
		private:
			struct Impl; // 実装の詳細を隠蔽するための前方宣言
			std::unique_ptr<Impl> impl; // 実装のポインタ
		};

		// ChoPhysics
		class choPhysicsWorld : public Id2World
		{
		public:
			choPhysicsWorld(d2Backend be);

			// Id2World を介して継承されました
			void Create() override;
			void Destroy() override;
			void Step(const float& deltaTime, const uint32_t& subStepCount) override;
			Vector2 GetGravity() const override;
			void SetGravity(const Vector2& gravity) override;
		private:
			struct Impl; // 実装の詳細を隠蔽するための前方宣言
			std::unique_ptr<Impl> impl; // 実装のポインタ
		};
	}
}

#endif // D2_WORLD_H
