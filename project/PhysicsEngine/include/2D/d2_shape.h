#ifndef D2_SHAPE_H
#define D2_SHAPE_H

#include <chomath.h>
#include <memory>

namespace physics
{
	namespace d2
	{
		class Id2Body;
		class Id2Polygon;

		struct Id2ShapeDef
		{
			float density = 1.0f; // 密度
			float friction = 0.5f; // 摩擦係数
			float restitution = 0.0f; // 反発係数
			bool isSensor = false; // センサーかどうか
		};

		class Id2Shape
		{
		public:
			virtual ~Id2Shape() = default;
			virtual void CreatePolygonShape(Id2Body*, Id2ShapeDef*, Id2Polygon*) {}
			virtual void Destroy() {} // 形状を削除
		};

		class box2dShape : public Id2Shape
		{
			public:
			box2dShape();
			~box2dShape() override = default;
			void CreatePolygonShape(Id2Body* body, Id2ShapeDef* shapeDef, Id2Polygon* polygon) override;
			void Destroy() override;
		private:
			struct Impl; // 実装の詳細を隠蔽するための前方宣言
			std::unique_ptr<Impl> impl; // 実装のポインタ
		};

		class choPhysicsShape : public Id2Shape
		{
			public:
			choPhysicsShape() = default;
			~choPhysicsShape() override = default;
			void CreatePolygonShape(Id2Body* body, Id2ShapeDef* shapeDef, Id2Polygon* polygon) override
			{
				body; shapeDef; polygon; // ここで実際の処理を実装する
			}
			void Destroy() override
			{
				// ChoPhysicsの形状を削除する処理を実装
			}
		};
	}
}

#endif // D2_SHAPE_H
