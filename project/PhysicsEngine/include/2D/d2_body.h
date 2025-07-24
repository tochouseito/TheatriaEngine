#ifndef D2_BODY_H
#define D2_BODY_H

#include <stdint.h>
#include <chomath.h>

// Box2D, ChoPhysics
struct b2BodyId;
struct b2WorldId;
struct b2BodyDef;

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
			virtual void Create(Id2World* world, const Id2BodyDef& bodyDef) = 0;
			// 削除
			virtual void Destroy() = 0;
			// 
			virtual void SetAwake(bool flag) = 0;
		};

		// box2d, ChoPhysicsのボディクラス
		class box2dBody : public Id2Body
		{
			friend class box2dShape;
			friend class box2dPolygon;
		public:
			box2dBody();
			~box2dBody() override = default;
			// Id2Body を介して継承されました
			void Create(Id2World* world, const Id2BodyDef& bodyDef) override;
			void Destroy() override;
			void SetAwake(bool flag) override;
		private:
			b2BodyId GetBody();
			struct Impl; // 実装の詳細を隠蔽するための前方宣言
			std::unique_ptr<Impl> impl;// 実装のポインタ
			box2dWorld* pWorld = nullptr; // 所属するワールド
		};

		class choPhysicsBody : public Id2Body
		{
			public:
			choPhysicsBody() = default;
			~choPhysicsBody() override = default;
		};

	}
}

#endif // D2_BODY_H
