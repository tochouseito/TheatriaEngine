#ifndef D2_FIXTURE_H
#define D2_FIXTURE_H

namespace physics
{
	namespace d2
	{
		class Id2Shape;

		struct Id2FixtureDef
		{
			Id2Shape* shape = nullptr; // 形状
			float density = 0.0f; // 密度
			float friction = 0.2f; // 摩擦係数
			float restitution = 0.0f; // 反発係数
			bool isSensor = false; // センサーかどうか
		};

		class Id2Fixture
		{

		};
	}
}

#endif // D2_FIXTURE_H
