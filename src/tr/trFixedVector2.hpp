#pragma once

#include "trFixed.hpp"

namespace tr
{
	struct FixedVector2
	{
		FixedVector2& operator+=(const FixedVector2& rhs);
		FixedVector2& operator-=(const FixedVector2& rhs);
		FixedVector2& operator/=(const Fixed& rhs);

		FixedVector2  operator+(const FixedVector2& rhs) const;
		FixedVector2  operator-(const FixedVector2& rhs) const;
		FixedVector2  operator*(const Fixed& rhs) const;
		FixedVector2  operator/(const Fixed& rhs) const;

		Fixed         x;
		Fixed         y;
	};
}