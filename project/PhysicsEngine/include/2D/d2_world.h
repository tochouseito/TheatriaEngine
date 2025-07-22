#ifndef D2_WORLD_H
#define D2_WORLD_H

namespace physics
{
	namespace d2
	{
		class Id2Body;
		class Id2Gravity;
		class Id2ContactListener;

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
			virtual d2Backend GetBackend() const = 0;
			// 物理ボディの作成
			virtual Id2Body* CreateBody() = 0;
			// 物理ボディの削除
			virtual void DestroyBody(Id2Body* body) = 0;
			// シュミレーションのステップ
			virtual void Step(const float& deltaTime) = 0;
			// 重力を取得、設定
			virtual Id2Gravity GetGravity() const = 0;
			virtual void SetGravity(const Id2Gravity& gravity) = 0;
		};
	}
}

#endif // D2_WORLD_H
