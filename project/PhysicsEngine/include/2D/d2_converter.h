#ifndef D2_CONVERTER_H
#define D2_CONVERTER_H

// Box2D, ChoPhysics
enum b2BodyType;

#include <chomath.h>

namespace physics
{
	namespace d2
	{
		enum Id2BodyType;

		static b2BodyType ConvertBodyType(Id2BodyType type) noexcept;
	}
}

#endif // D2_CONVERTER_H
