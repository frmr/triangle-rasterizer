#include "trTransformedVertex.hpp"

tr::TransformedVertex::TransformedVertex() :
	inverseW(0.0f)
{
}

tr::TransformedVertex::TransformedVertex(const Vector4& worldPosition, const Vector4& projectedPosition, const Vector3& normal, const Vector2& textureCoord) :
	worldPosition(worldPosition),
	projectedPosition(projectedPosition),
	normal(normal),
	textureCoord(textureCoord),
	inverseW(0.0f)
{
}

tr::TransformedVertex::TransformedVertex(const Vector4& worldPosition, const Vector4& projectedPosition, const Vector3& normal, const Vector2& textureCoord, const float inverseW) :
	worldPosition(worldPosition),
	projectedPosition(projectedPosition),
	normal(normal),
	textureCoord(textureCoord),
	inverseW(inverseW)
{
}

tr::TransformedVertex& tr::TransformedVertex::operator+=(const TransformedVertex& rhs)
{
	worldPosition     += rhs.worldPosition;
	projectedPosition += rhs.projectedPosition;
	normal            += rhs.normal;
	textureCoord      += rhs.textureCoord;
	inverseW          += rhs.inverseW;

	return *this;
}

tr::TransformedVertex& tr::TransformedVertex::operator-=(const TransformedVertex& rhs)
{
	worldPosition     -= rhs.worldPosition;
	projectedPosition -= rhs.projectedPosition;
	normal            -= rhs.normal;
	textureCoord      -= rhs.textureCoord;
	inverseW          -= rhs.inverseW;

	return *this;
}

tr::TransformedVertex tr::TransformedVertex::operator+(const TransformedVertex& rhs) const
{
	return TransformedVertex(
		worldPosition     + rhs.worldPosition,
		projectedPosition + rhs.projectedPosition,
		normal            + rhs.normal,
		textureCoord      + rhs.textureCoord,
		inverseW          + rhs.inverseW
	);
}

tr::TransformedVertex tr::TransformedVertex::operator-(const TransformedVertex& rhs) const
{
	return TransformedVertex(
		worldPosition     - rhs.worldPosition,
		projectedPosition - rhs.projectedPosition,
		normal            - rhs.normal,
		textureCoord      - rhs.textureCoord,
		inverseW          - rhs.inverseW
	);
}

tr::TransformedVertex tr::TransformedVertex::operator*(const float& rhs) const
{
	return TransformedVertex(
		worldPosition     * rhs,
		projectedPosition * rhs,
		normal            * rhs,
		textureCoord      * rhs,
		inverseW          * rhs
	);
}

tr::TransformedVertex tr::TransformedVertex::operator/(const float& rhs) const
{
	return TransformedVertex(
		worldPosition     / rhs,
		projectedPosition / rhs,
		normal            / rhs,
		textureCoord      / rhs,
		inverseW          / rhs
	);
}

tr::TransformedVertex& tr::TransformedVertex::normalize()
{
	const float length = std::sqrt(projectedPosition.x * projectedPosition.x + projectedPosition.y * projectedPosition.y + projectedPosition.z * projectedPosition.z);

	worldPosition.x     /= length;
	worldPosition.y     /= length;
	worldPosition.z     /= length;

	projectedPosition.x /= length;
	projectedPosition.y /= length;
	projectedPosition.z /= length;

	normal              /= length;
	textureCoord        /= length;
	inverseW            /= length;

	return *this;
}
