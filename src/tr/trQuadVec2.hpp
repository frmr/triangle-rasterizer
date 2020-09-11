#pragma once

#include "trQuadFloat.hpp"
#include "Vectors.h"

namespace tr
{
	struct QuadVec2
	{
		          QuadVec2(const float x, const float y);
		          QuadVec2(const QuadFloat& x, const QuadFloat& y);
		          QuadVec2(const Vector2& vector);

		QuadVec2& operator+=(const QuadVec2& rhs);
		QuadVec2& operator*=(const QuadFloat& rhs);
		QuadVec2& operator/=(const QuadFloat& rhs);

		QuadVec2  operator+(const QuadVec2& rhs) const;
		QuadVec2  operator*(const QuadFloat& rhs) const;
		QuadVec2  operator/(const QuadFloat& rhs) const;

		QuadFloat x;
		QuadFloat y;
	};
}