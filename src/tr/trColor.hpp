#pragma once

#include "../matrix/Vectors.h"

namespace tr
{
#pragma pack(push,1)

	struct Color
	{
		Vector4 ToVector() const
		{
			return Vector4(
				float(b),
				float(g),
				float(r),
				float(a)
			);
		}

		Color()
		{
		}

		Color(const uint8_t b, const uint8_t g, const uint8_t r, const uint8_t a) :
			b(b),
			g(g),
			r(r),
			a(a)
		{
		}

		Color(const Vector4& vector) :
			b((uint8_t)vector[0]),
			g((uint8_t)vector[1]),
			r((uint8_t)vector[2]),
			a((uint8_t)vector[3])
		{
		}

		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t a;
	};

#pragma pack(pop)
}
