#include "trVertex.hpp"

tr::Vertex& tr::Vertex::operator+=(const Vertex& rhs)
{
	position     += rhs.position;
	normal       += rhs.normal;
	textureCoord += rhs.textureCoord;
	inverseW     += rhs.inverseW;

	return *this;
}

tr::Vertex& tr::Vertex::operator-=(const Vertex& rhs)
{
	position     -= rhs.position;
	normal       -= rhs.normal;
	textureCoord -= rhs.textureCoord;
	inverseW     -= rhs.inverseW;

	return *this;
}

tr::Vertex tr::Vertex::operator+(const Vertex& rhs) const
{
	return Vertex {
		position     + rhs.position,
		normal       + rhs.normal,
		textureCoord + rhs.textureCoord,
		inverseW     + rhs.inverseW
	};
}

tr::Vertex tr::Vertex::operator-(const Vertex& rhs) const
{
	return Vertex {
		position     - rhs.position,
		normal       - rhs.normal,
		textureCoord - rhs.textureCoord,
		inverseW     - rhs.inverseW
	};
}

tr::Vertex tr::Vertex::operator*(const float& rhs) const
{
	return Vertex {
		position     * rhs,
		normal       * rhs,
		textureCoord * rhs,
		inverseW     * rhs
	};
}

tr::Vertex tr::Vertex::operator/(const float& rhs) const
{
	return Vertex {
		position     / rhs,
		normal       / rhs,
		textureCoord / rhs,
		inverseW     / rhs
	};
}

tr::Vertex& tr::Vertex::normalize()
{
	const float length = std::sqrt(position.x * position.x + position.y * position.y + position.z * position.z);

	position.x   /= length;
	position.y   /= length;
	position.z   /= length;

	normal       /= length;
	textureCoord /= length;
	inverseW     /= length;

	return *this;
}
