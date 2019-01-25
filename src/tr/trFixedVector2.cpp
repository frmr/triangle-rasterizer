#include "trFixedVector2.hpp"

tr::FixedVector2& tr::FixedVector2::operator+=(const FixedVector2& rhs)
{
	x += rhs.x;
	y += rhs.y;

	return *this;
}

tr::FixedVector2& tr::FixedVector2::operator-=(const FixedVector2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;

	return *this;
}

tr::FixedVector2& tr::FixedVector2::operator/=(const Fixed& rhs)
{
	x = sg14::divide(x, rhs);
	y = sg14::divide(y, rhs);

	return *this;
}

tr::FixedVector2 tr::FixedVector2::operator+(const FixedVector2& rhs) const
{
	return FixedVector2 {
		x + rhs.x,
		y + rhs.y
	};
}

tr::FixedVector2 tr::FixedVector2::operator-(const FixedVector2& rhs) const
{
	return FixedVector2 {
		x - rhs.x,
		y - rhs.y
	};
}

tr::FixedVector2 tr::FixedVector2::operator*(const Fixed& rhs) const
{
	return FixedVector2 {
		x * rhs,
		y * rhs
	};
}

tr::FixedVector2 tr::FixedVector2::operator/(const Fixed& rhs) const
{
	return FixedVector2 {
		sg14::divide(x, rhs),
		sg14::divide(y, rhs)
	};
}
