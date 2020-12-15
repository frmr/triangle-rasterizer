#pragma once

#include "trQuadFloat.hpp"
#include "Vectors.h"

namespace tr
{
	struct QuadVec4
	{
		          QuadVec4(const float x, const float y, const float z, const float w);
		          QuadVec4(const QuadFloat& x, const QuadFloat& y, const QuadFloat& z, const QuadFloat& w);
		          QuadVec4(const Vector4& vector);

		QuadVec4& operator+=(const QuadVec4& rhs);
		QuadVec4& operator*=(const QuadFloat& rhs);

		QuadVec4  operator+(const QuadVec4& rhs) const;
		QuadVec4  operator*(const QuadFloat& rhs) const;

		QuadFloat x;
		QuadFloat y;
		QuadFloat z;
		QuadFloat w;
	};
}