#include "trQuadVec2.hpp"

tr::QuadVec2::QuadVec2(const float x, const float y) :
	x(x, x, x, x),
	y(y, y, y, y)
{
}

tr::QuadVec2::QuadVec2(const QuadFloat& x, const QuadFloat& y) :
	x(x),
	y(y)
{
}

tr::QuadVec2::QuadVec2(const Vector2& vector) :
	x(vector.x),
	y(vector.y)
{
}

tr::QuadVec2& tr::QuadVec2::operator+=(const QuadVec2& rhs)
{
	x += rhs.x;
	y += rhs.y;

	return *this;
}

tr::QuadVec2& tr::QuadVec2::operator*=(const QuadFloat& rhs)
{
	x *= rhs;
	y *= rhs;

	return *this;
}

tr::QuadVec2& tr::QuadVec2::operator/=(const QuadFloat& rhs)
{
	x /= rhs;
	y /= rhs;

	return *this;
}
