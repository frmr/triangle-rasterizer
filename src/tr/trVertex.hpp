#pragma once

#include "../matrix/Vectors.h"

namespace tr
{
	struct Vertex
	{
		        Vertex();
		        Vertex(const Vector4& worldPosition, const Vector3& normal, const Vector2& textureCoord);
		        Vertex(const Vector4& worldPosition, const Vector4& position, const Vector3& normal, const Vector2& textureCoord);
		        Vertex(const Vector4& worldPosition, const Vector4& position, const Vector3& normal, const Vector2& textureCoord, const float inverseW);

		Vertex& operator+=(const Vertex& rhs);
		Vertex& operator-=(const Vertex& rhs);

		Vertex  operator+(const Vertex& rhs) const;
		Vertex  operator-(const Vertex& rhs) const;
		Vertex  operator*(const float& rhs) const;
		Vertex  operator/(const float& rhs) const;

		Vertex& normalize();

		Vector4 worldPosition;
		Vector4 position;
		Vector3 normal;
		Vector2 textureCoord;
		float   inverseW;
	};
}