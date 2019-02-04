#pragma once

#include "../matrix/Vectors.h"

namespace tr
{
	struct Vertex
	{
		        Vertex();
		        Vertex(const Vector4& worldPosition, const Vector3& normal, const Vector2& textureCoord);
		        Vertex(const Vector4& worldPosition, const Vector4& position, const Vector3& normal, const Vector2& textureCoord);
		        Vertex(const Vector4& worldPosition, const Vector4& position, const Vector3& normal, const Vector2& textureCoord, const double inverseW);

		Vertex& operator+=(const Vertex& rhs);
		Vertex& operator-=(const Vertex& rhs);

		Vertex  operator+(const Vertex& rhs) const;
		Vertex  operator-(const Vertex& rhs) const;
		Vertex  operator*(const double& rhs) const;
		Vertex  operator/(const double& rhs) const;

		Vertex& normalize();

		Vector4 worldPosition;
		Vector4 position;
		Vector3 normal;
		Vector2 textureCoord;
		double  inverseW;
	};
}