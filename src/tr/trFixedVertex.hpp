#pragma once

#include "trFixedVector2.hpp"
#include "trFixedVector3.hpp"

namespace tr
{
	struct FixedVertex
	{
		FixedVertex& operator+=(const FixedVertex& rhs);
		FixedVertex& operator-=(const FixedVertex& rhs);
		FixedVertex& operator/=(const Fixed& rhs);

		FixedVertex  operator+(const FixedVertex& rhs) const;
		FixedVertex  operator-(const FixedVertex& rhs) const;
		FixedVertex  operator*(const Fixed& rhs) const;
		FixedVertex  operator/(const Fixed& rhs) const;

		FixedVertex& normalize();

		FixedVector3 position;
		FixedVector3 normal;
		FixedVector2 textureCoord;

		Fixed        inverseW;
	};
}