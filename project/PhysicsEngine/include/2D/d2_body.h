#ifndef D2_BODY_H
#define D2_BODY_H

#include <stdint.h>
#include <chomath.h>

// Box2D, ChoPhysics
struct b2BodyId;
struct b2WorldId;
struct b2BodyDef;
struct b2ShapeId;

namespace physics
{
	namespace d2
	{
		class Id2World;
		class box2dWorld;

		enum Id2BodyType
		{
			Id2_staticBody = 0,    // 静的ボディ
			Id2_kinematicBody,     // キネマティックボディ
			Id2_dynamicBody         // ダイナミックボディ
		};

		struct Id2BodyDef
		{
			void* userData; // ユーザーデータ
			Id2BodyType type = Id2_dynamicBody; // ボディタイプ
			float gravityScale = 1.0f; // 重力スケール
			bool fixedRotation = false; // 回転を固定するかどうか
			Vector2 position = Vector2(0.0f, 0.0f); // 位置
			float angle = 0.0f; // 角度（ラジアン）
		};

		class Id2Body
		{
		public:
			virtual ~Id2Body() = default;
			// 作成
			virtual void Create(Id2World*, const Id2BodyDef&) {}
			// 削除
			virtual void Destroy() {}
			// 
			virtual void SetAwake(bool) {}

			virtual Vector2 GetPosition() const { return Vector2(0.0f, 0.0f); }
			virtual Vector2 GetLinearVelocity() const { return Vector2(0.0f, 0.0f); } // 速度を取得（デフォルトは(0,0)）
			virtual void SetLinearVelocity(const Vector2&) {} // 速度を設定
			virtual void SetTransform(const Vector2&,const float&) {} // 位置と角度を設定
			virtual float GetAngle() const { return 0.0f; } // 角度を取得（デフォルトは0.0f）

			bool IsActive() const { return isActive; } // 有効フラグの取得
		protected:
			bool isActive = true; // 有効フラグ
		};

		// box2d, ChoPhysicsのボディクラス
		class box2dBody : public Id2Body
		{
			friend class box2dShape;
			friend class box2dPolygon;
		public:
			box2dBody();
			~box2dBody() override;
			// Id2Body を介して継承されました
			void Create(Id2World* world, const Id2BodyDef& bodyDef) override;
			void Destroy() override;
			void SetAwake(bool flag) override;
			Vector2 GetPosition() const override;
			Vector2 GetLinearVelocity() const override;
			void SetLinearVelocity(const Vector2& velocity) override;
			void SetTransform(const Vector2& position, const float& angle) override;
			float GetAngle() const override; // 角度を取得
		private:
			b2BodyId GetBody();
			box2dWorld* GetWorld() const;
			void SetShapeId(b2ShapeId* shapeId); // 形状IDを設定
			void RemoveShapeId(); // 形状IDを削除
			struct Impl; // 実装の詳細を隠蔽するための前方宣言
			std::unique_ptr<Impl> impl;// 実装のポインタ
			box2dWorld* pWorld = nullptr; // 所属するワールド
		};

		class choPhysicsBody : public Id2Body
		{
			public:
			choPhysicsBody() = default;
			~choPhysicsBody() override = default;
			void Create(Id2World* world, const Id2BodyDef& bodyDef) override
			{
				world; bodyDef; // ここで実際の処理を実装する
			}
			void Destroy() override
			{
				// ChoPhysicsのボディを削除する処理を実装
			}
			void SetAwake(bool flag) override
			{
				flag; // ここで実際の処理を実装する
			}
			Vector2 GetPosition() const override
			{
				// 位置を取得する処理を実装
				return Vector2(0.0f, 0.0f); // 仮の値を返す
			}
		};

	}
}

#endif // D2_BODY_H
