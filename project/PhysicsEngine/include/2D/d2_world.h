#ifndef D2_WORLD_H
#define D2_WORLD_H

#include <chomath.h>
#include <memory>
#include <functional>
#include <set>
#include <unordered_set>
#include <xhash>

// box2d, ChoPhysics
struct b2WorldId;
struct b2ShapeId;

namespace physics
{
	namespace d2
	{
		class Id2Body;
		class Id2ContactListener;
		struct Id2BodyDef;

		using OnContactFunc = std::function<void(void*, void*)>;

		enum class d2Backend
		{
			box2d,  // Box2Dを使用
			chophysics // ChoPhysicsを使用
		};

		// 2D物理エンジンの共通定義や関数をここに追加

		struct PairHash
		{
			template <typename T1, typename T2>
			std::size_t operator()(const std::pair<T1, T2>& p) const
			{
				return std::hash<T1>()(p.first.idx) ^ std::hash<T2>()(p.second.idx);
			}
		};

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
			// ステップ後に呼び出される衝突イベント
			virtual void ProcessEvents() = 0;
			// イベントコールバック関数登録
			virtual void SetBeginContactCallback(OnContactFunc) {}
			virtual void SetEndContactCallback(OnContactFunc) {}
			virtual void SetStayContactCallback(OnContactFunc) {}
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
			void ProcessEvents() override;
			// イベントコールバック関数の登録
			void SetBeginContactCallback(OnContactFunc callback) override
			{
				beginContactCallback = std::move(callback);
			}
			void SetEndContactCallback(OnContactFunc callback) override
			{
				endContactCallback = std::move(callback);
			}
			void SetStayContactCallback(OnContactFunc callback) override
			{
				stayContactCallback = std::move(callback);
			}
			void InsertShapeId(const b2ShapeId& shapeId);
			void RemoveShapeId(const b2ShapeId& shapeId);
		private:
			struct Impl; // 実装の詳細を隠蔽するための前方宣言
			std::unique_ptr<Impl> impl; // 実装のポインタ
			// コールバック関数
			OnContactFunc beginContactCallback; // 衝突開始時のコールバック
			OnContactFunc endContactCallback;   // 衝突終了時のコールバック
			OnContactFunc stayContactCallback;  // 衝突継続時のコールバック
			std::unordered_set<std::pair<int32_t, int32_t>, PairHash> currentContacts;
			std::unordered_set<std::pair<int32_t, int32_t>, PairHash> previousContacts;
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
			void ProcessEvents() override
			{
				// ChoPhysicsの衝突イベントを処理する実装をここに追加
			}
		private:
			struct Impl; // 実装の詳細を隠蔽するための前方宣言
			std::unique_ptr<Impl> impl; // 実装のポインタ
		};
	}
}

#endif // D2_WORLD_H
