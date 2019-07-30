#include "trVertex.hpp"

tr::Vertex::Vertex() :
	inverseW(0.0f)
{
}

tr::Vertex::Vertex(const Vector4& worldPosition, const Vector3& normal, const Vector2& textureCoord) :
	worldPosition(worldPosition),
	normal(normal),
	textureCoord(textureCoord),
	inverseW(0.0f)
{
}

tr::Vertex::Vertex(const Vector4& worldPosition, const Vector4& position, const Vector3& normal, const Vector2& textureCoord) :
	worldPosition(worldPosition),
	position(position),
	normal(normal),
	textureCoord(textureCoord),
	inverseW(0.0f)
{
}

tr::Vertex::Vertex(const Vector4& worldPosition, const Vector4& position, const Vector3& normal, const Vector2& textureCoord, const float inverseW) :
	worldPosition(worldPosition),
	position(position),
	normal(normal),
	textureCoord(textureCoord),
	inverseW(inverseW)
{
}

tr::Vertex& tr::Vertex::operator+=(const Vertex& rhs)
{
	worldPosition += rhs.worldPosition;
	position      += rhs.position;
	normal        += rhs.normal;
	textureCoord  += rhs.textureCoord;
	inverseW      += rhs.inverseW;

	return *this;
}

tr::Vertex& tr::Vertex::operator-=(const Vertex& rhs)
{
	worldPosition -= rhs.worldPosition;
	position      -= rhs.position;
	normal        -= rhs.normal;
	textureCoord  -= rhs.textureCoord;
	inverseW      -= rhs.inverseW;

	return *this;
}

tr::Vertex tr::Vertex::operator+(const Vertex& rhs) const
{
	return Vertex(
		worldPosition + rhs.worldPosition,
		position      + rhs.position,
		normal        + rhs.normal,
		textureCoord  + rhs.textureCoord,
		inverseW      + rhs.inverseW
	);
}

tr::Vertex tr::Vertex::operator-(const Vertex& rhs) const
{
	return Vertex(
		worldPosition - rhs.worldPosition,
		position      - rhs.position,
		normal        - rhs.normal,
		textureCoord  - rhs.textureCoord,
		inverseW      - rhs.inverseW
	);
}

tr::Vertex tr::Vertex::operator*(const float& rhs) const
{
	return Vertex(
		worldPosition * rhs,
		position      * rhs,
		normal        * rhs,
		textureCoord  * rhs,
		inverseW      * rhs
	);
}

tr::Vertex tr::Vertex::operator/(const float& rhs) const
{
	return Vertex(
		worldPosition / rhs,
		position      / rhs,
		normal        / rhs,
		textureCoord  / rhs,
		inverseW      / rhs
	);
}

tr::Vertex& tr::Vertex::normalize()
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
