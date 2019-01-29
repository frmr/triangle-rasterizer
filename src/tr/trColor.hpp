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
				float(blue),
				float(green),
				float(red),
				float(alpha)
			);
		}

		Color()
		{
		}

		Color(const unsigned char blue, const unsigned char green, const unsigned char red, const unsigned char alpha) :
			blue(blue),
			green(green),
			red(red),
			alpha(alpha)
		{
		}

		Color(const Vector4& vector) :
			blue((unsigned char)vector[0]),
			green((unsigned char)vector[1]),
			red((unsigned char)vector[2]),
			alpha((unsigned char)vector[3])
		{
		}

		unsigned char blue;
		unsigned char green;
		unsigned char red;
		unsigned char alpha;
	};

#pragma pack(pop)
}
