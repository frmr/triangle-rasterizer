#include "trQuadVec4.hpp"

tr::QuadVec4::QuadVec4(const float x, const float y, const float z, const float w) :
	x(x),
	y(y),
	z(z),
	w(w)
{
}

tr::QuadVec4::QuadVec4(const QuadFloat& x, const QuadFloat& y, const QuadFloat& z, const QuadFloat& w) :
	x(x),
	y(y),
	z(z),
	w(w)
{
}

tr::QuadVec4::QuadVec4(const Vector4& vector) :
	x(vector.x),
	y(vector.y),
	z(vector.z),
	w(vector.w)
{
}

tr::QuadVec4& tr::QuadVec4::operator+=(const QuadVec4& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	w += rhs.w;

	return *this;
}

tr::QuadVec4& tr::QuadVec4::operator*=(const QuadFloat& rhs)
{
	x *= rhs;
	y *= rhs;
	z *= rhs;
	w *= rhs;

	return *this;
}

tr::QuadVec4 tr::QuadVec4::operator+(const QuadVec4& rhs) const
{
	return QuadVec4(
		x + rhs.x,
		y + rhs.y,
		z + rhs.z,
		w + rhs.w
	);
}

tr::QuadVec4 tr::QuadVec4::operator*(const QuadFloat& rhs) const
{
	return QuadVec4(
		x * rhs,
		y * rhs,
		z * rhs,
		w * rhs
	);
}
