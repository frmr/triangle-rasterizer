#pragma once

#include "../matrix/Vectors.h"

namespace tr
{
	struct TransformedVertex
	{
		                   TransformedVertex();
		                   TransformedVertex(const Vector3& worldPosition, const Vector4& projectedPosition, const Vector3& normal, const Vector2& textureCoord);
		                   TransformedVertex(const Vector3& worldPosition, const Vector4& projectedPosition, const Vector3& normal, const Vector2& textureCoord, const float inverseW);

		TransformedVertex& operator+=(const TransformedVertex& rhs);
		TransformedVertex& operator-=(const TransformedVertex& rhs);

		TransformedVertex  operator+(const TransformedVertex& rhs) const;
		TransformedVertex  operator-(const TransformedVertex& rhs) const;
		TransformedVertex  operator*(const float& rhs) const;
		TransformedVertex  operator/(const float& rhs) const;

		TransformedVertex& normalize();

		Vector3            worldPosition;
		Vector4            projectedPosition;
		Vector3            normal;
		Vector2            textureCoord;
		float              inverseW;
	};
}