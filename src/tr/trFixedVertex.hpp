#pragma once

#include "trFixedVector2.hpp"
#include "trFixedVector3.hpp"
#include "trVertex.hpp"

namespace tr
{
	struct FixedVertex
	{
		FixedVertex(const FixedVector3& position, const FixedVector3& normal, const FixedVector2& textureCoord, const Fixed& inverseW);
		FixedVertex(const Vertex& vertex);

		FixedVertex& operator+=(const FixedVertex& rhs);
		FixedVertex& operator-=(const FixedVertex& rhs);
		FixedVertex& operator/=(const Fixed& rhs);

		FixedVertex  operator+(const FixedVertex& rhs) const;
		FixedVertex  operator-(const FixedVertex& rhs) const;
		FixedVertex  operator*(const Fixed& rhs) const;
		FixedVertex  operator/(const Fixed& rhs) const;

		FixedVertex& normalize();

		FixedVector3 position;
		FixedVector3 normal;
		FixedVector2 textureCoord;

		Fixed        inverseW;
	};
}