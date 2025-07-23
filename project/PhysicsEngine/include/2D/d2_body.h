#ifndef D2_BODY_H
#define D2_BODY_H

#include <stdint.h>
#include <chomath.h>

// Box2D, ChoPhysics
class b2BodyId;
class b2WorldId;
struct b2BodyDef;

namespace physics
{
	namespace d2
	{
		class Id2Fixture;
		struct Id2FixtureDef;

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
			friend class Id2World; // Id2Worldからのみアクセス可能
		protected:
			virtual ~Id2Body() = default;// 外部からのデストラクタを不可
		public:
			// Fixtureの作成
			virtual Id2Fixture* CreateFixture(const Id2FixtureDef& fixtureDef) = 0;
			// Fixtureの削除
			virtual void DestroyFixture(Id2Fixture* fixture) = 0;
			// 
			virtual void SetAwake(bool flag) = 0;
		};

		// box2d, ChoPhysicsのボディクラス
		class box2dBody : public Id2Body
		{
		public:
			box2dBody(b2WorldId worldId, const b2BodyDef* def);
			~box2dBody() override = default;
			Id2Fixture* CreateFixture(const Id2FixtureDef& fixtureDef) override;
			void DestroyFixture(Id2Fixture* fixture) override;
			void SetAwake(bool flag) override;
		public:
			b2BodyId* m_body; // Box2Dのボディ
		};

		class choPhysicsBody : public Id2Body
		{
			public:
			choPhysicsBody() = default;
			~choPhysicsBody() override = default;
			Id2Fixture* CreateFixture(const Id2FixtureDef& fixtureDef) override;
			void DestroyFixture(Id2Fixture* fixture) override;
			void SetAwake(bool flag) override;
		};

	}
}

#endif // D2_BODY_H
