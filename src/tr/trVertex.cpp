#include "trVertex.hpp"

tr::TransformedVertex::TransformedVertex() :
	inverseW(0.0f)
{
}

tr::TransformedVertex::TransformedVertex(const Vector4& worldPosition, const Vector3& normal, const Vector2& textureCoord) :
	worldPosition(worldPosition),
	normal(normal),
	textureCoord(textureCoord),
	inverseW(0.0f)
{
}

tr::TransformedVertex::TransformedVertex(const Vector4& worldPosition, const Vector4& position, const Vector3& normal, const Vector2& textureCoord) :
	worldPosition(worldPosition),
	position(position),
	normal(normal),
	textureCoord(textureCoord),
	inverseW(0.0f)
{
}

tr::TransformedVertex::TransformedVertex(const Vector4& worldPosition, const Vector4& position, const Vector3& normal, const Vector2& textureCoord, const float inverseW) :
	worldPosition(worldPosition),
	position(position),
	normal(normal),
	textureCoord(textureCoord),
	inverseW(inverseW)
{
}

tr::TransformedVertex& tr::TransformedVertex::operator+=(const TransformedVertex& rhs)
{
	worldPosition += rhs.worldPosition;
	position      += rhs.position;
	normal        += rhs.normal;
	textureCoord  += rhs.textureCoord;
	inverseW      += rhs.inverseW;

	return *this;
}

tr::TransformedVertex& tr::TransformedVertex::operator-=(const TransformedVertex& rhs)
{
	worldPosition -= rhs.worldPosition;
	position      -= rhs.position;
	normal        -= rhs.normal;
	textureCoord  -= rhs.textureCoord;
	inverseW      -= rhs.inverseW;

	return *this;
}

tr::TransformedVertex tr::TransformedVertex::operator+(const TransformedVertex& rhs) const
{
	return TransformedVertex(
		worldPosition + rhs.worldPosition,
		position      + rhs.position,
		normal        + rhs.normal,
		textureCoord  + rhs.textureCoord,
		inverseW      + rhs.inverseW
	);
}

tr::TransformedVertex tr::TransformedVertex::operator-(const TransformedVertex& rhs) const
{
	return TransformedVertex(
		worldPosition - rhs.worldPosition,
		position      - rhs.position,
		normal        - rhs.normal,
		textureCoord  - rhs.textureCoord,
		inverseW      - rhs.inverseW
	);
}

tr::TransformedVertex tr::TransformedVertex::operator*(const float& rhs) const
{
	return TransformedVertex(
		worldPosition * rhs,
		position      * rhs,
		normal        * rhs,
		textureCoord  * rhs,
		inverseW      * rhs
	);
}

tr::TransformedVertex tr::TransformedVertex::operator/(const float& rhs) const
{
	return TransformedVertex(
		worldPosition / rhs,
		position      / rhs,
		normal        / rhs,
		textureCoord  / rhs,
		inverseW      / rhs
	);
}

tr::TransformedVertex& tr::TransformedVertex::normalize()
{
	const float length = std::sqrt(position.x * position.x + position.y * position.y + position.z * position.z);

	worldPosition.x   /= length;
	worldPosition.y   /= length;
	worldPosition.z   /= length;

	position.x        /= length;
	position.y        /= length;
	position.z        /= length;

	normal            /= length;
	textureCoord      /= length;
	inverseW          /= length;

	return *this;
}
