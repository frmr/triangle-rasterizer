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
				double(b),
				double(g),
				double(r),
				double(a)
			);
		}

		Color()
		{
		}

		Color(const unsigned char b, const unsigned char g, const unsigned char r, const unsigned char a) :
			b(b),
			g(g),
			r(r),
			a(a)
		{
		}

		Color(const Vector4& vector) :
			b((unsigned char)vector[0]),
			g((unsigned char)vector[1]),
			r((unsigned char)vector[2]),
			a((unsigned char)vector[3])
		{
		}

		unsigned char b;
		unsigned char g;
		unsigned char r;
		unsigned char a;
	};

#pragma pack(pop)
}
