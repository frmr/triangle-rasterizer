#pragma once

#include "trQuadFloat.hpp"
#include "Vectors.h"

namespace tr
{
	struct QuadVec3
	{
		          QuadVec3(const float x, const float y, const float z);
		          QuadVec3(const QuadFloat& x, const QuadFloat& y, const QuadFloat& z);
		          QuadVec3(const Vector3& vector);

		QuadFloat dot(const QuadVec3& rhs) const;
		QuadFloat length() const;

		QuadVec3& operator+=(const QuadVec3& rhs);
		QuadVec3& operator*=(const QuadFloat& rhs);
		QuadVec3& operator/=(const QuadFloat& rhs);

		QuadVec3  operator+(const QuadVec3& rhs) const;
		QuadVec3  operator-(const QuadVec3& rhs) const;
		QuadVec3  operator*(const QuadVec3& rhs) const;
		QuadVec3  operator*(const QuadFloat& rhs) const;

		QuadFloat x;
		QuadFloat y;
		QuadFloat z;
	};
}