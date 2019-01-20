#pragma once

#include "../matrix/Vectors.h"

namespace tr
{
	struct Vertex
	{
		Vertex& operator+=(const Vertex& rhs);
		Vertex& operator-=(const Vertex& rhs);

		Vertex  operator+(const Vertex& rhs) const;
		Vertex  operator-(const Vertex& rhs) const;
		Vertex  operator*(const float& rhs) const;
		Vertex  operator/(const float& rhs) const;

		Vertex& normalize();

		Vector4 position;
		Vector3 normal;
		Vector2 textureCoord;
	};
}