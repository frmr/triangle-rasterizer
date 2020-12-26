#include "trQuadVec3.hpp"

tr::QuadVec3::QuadVec3(const float x, const float y, const float z) :
	x(x),
	y(y),
	z(z)
{
}

tr::QuadVec3::QuadVec3(const QuadFloat& x, const QuadFloat& y, const QuadFloat& z) :
	x(x),
	y(y),
	z(z)
{
}

tr::QuadVec3::QuadVec3(const Vector3& vector) :
	x(vector.x),
	y(vector.y),
	z(vector.z)
{
}

tr::QuadFloat tr::QuadVec3::dot(const QuadVec3& rhs) const
{
	return QuadFloat(x * rhs.x + y * rhs.y + z * rhs.z);
}

tr::QuadFloat tr::QuadVec3::length() const
{
	return (x * x + y * y + z * z).sqrt();
}

tr::QuadVec3& tr::QuadVec3::operator+=(const QuadVec3& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

tr::QuadVec3& tr::QuadVec3::operator*=(const QuadFloat& rhs)
{
	x *= rhs;
	y *= rhs;
	z *= rhs;

	return *this;
}

tr::QuadVec3& tr::QuadVec3::operator/=(const QuadFloat& rhs)
{
	x /= rhs;
	y /= rhs;
	z /= rhs;

	return *this;
}

tr::QuadVec3 tr::QuadVec3::operator+(const QuadVec3& rhs) const
{
	return QuadVec3(
		x + rhs.x,
		y + rhs.y,
		z + rhs.z
	);
}

tr::QuadVec3 tr::QuadVec3::operator-(const QuadVec3& rhs) const
{
	return QuadVec3(
		x - rhs.x,
		y - rhs.y,
		z - rhs.z
	);
}

tr::QuadVec3 tr::QuadVec3::operator*(const QuadVec3& rhs) const
{
	return QuadVec3(
		x * rhs.x,
		y * rhs.y,
		z * rhs.z
	);
}

tr::QuadVec3 tr::QuadVec3::operator*(const QuadFloat& rhs) const
{
	return QuadVec3(
		x * rhs,
		y * rhs,
		z * rhs
	);
}
