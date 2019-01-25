#pragma once

#include "trFixed.hpp"

namespace tr
{
	struct FixedVector3
	{
		FixedVector3& operator+=(const FixedVector3& rhs);
		FixedVector3& operator-=(const FixedVector3& rhs);
		FixedVector3& operator/=(const Fixed& rhs);

		FixedVector3  operator+(const FixedVector3& rhs) const;
		FixedVector3  operator-(const FixedVector3& rhs) const;
		FixedVector3  operator*(const Fixed& rhs) const;
		FixedVector3  operator/(const Fixed& rhs) const;

		Fixed         x;
		Fixed         y;
		Fixed         z;
	};
}