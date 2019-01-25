#include "trFixedVector3.hpp"

tr::FixedVector3& tr::FixedVector3::operator+=(const FixedVector3& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

tr::FixedVector3& tr::FixedVector3::operator-=(const FixedVector3& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;

	return *this;
}

tr::FixedVector3& tr::FixedVector3::operator/=(const Fixed& rhs)
{
	x = sg14::divide(x, rhs);
	y = sg14::divide(y, rhs);
	z = sg14::divide(z, rhs);

	return *this;
}

tr::FixedVector3 tr::FixedVector3::operator+(const FixedVector3& rhs) const
{
	return FixedVector3 {
		x + rhs.x,
		y + rhs.y,
		z + rhs.z
	};
}

tr::FixedVector3 tr::FixedVector3::operator-(const FixedVector3& rhs) const
{
	return FixedVector3 {
		x - rhs.x,
		y - rhs.y,
		z - rhs.z
	};
}

tr::FixedVector3 tr::FixedVector3::operator*(const Fixed& rhs) const
{
	return FixedVector3 {
		x * rhs,
		y * rhs,
		z * rhs
	};
}

tr::FixedVector3 tr::FixedVector3::operator/(const Fixed& rhs) const
{
	return FixedVector3 {
		sg14::divide(x, rhs),
		sg14::divide(y, rhs),
		sg14::divide(z, rhs)
	};
}