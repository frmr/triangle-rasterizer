#include "trVertex.hpp"

tr::Vertex& tr::Vertex::operator+=(const Vertex& rhs)
{
	position     += rhs.position;
	normal       += rhs.normal;
	textureCoord += rhs.textureCoord;

	return *this;
}

tr::Vertex& tr::Vertex::operator-=(const Vertex& rhs)
{
	position     -= rhs.position;
	normal       -= rhs.normal;
	textureCoord -= rhs.textureCoord;

	return *this;
}

tr::Vertex tr::Vertex::operator+(const Vertex& rhs) const
{
	return Vertex {
		position     + rhs.position,
		normal       + rhs.normal,
		textureCoord + rhs.textureCoord
	};
}

tr::Vertex tr::Vertex::operator-(const Vertex& rhs) const
{
	return Vertex {
		position     - rhs.position,
		normal       - rhs.normal,
		textureCoord - rhs.textureCoord
	};
}

tr::Vertex tr::Vertex::operator*(const float& rhs) const
{
	return Vertex {
		position     * rhs,
		normal       * rhs,
		textureCoord * rhs
	};
}

tr::Vertex tr::Vertex::operator/(const float& rhs) const
{
	return Vertex {
		position     / rhs,
		normal       / rhs,
		textureCoord / rhs
	};
}

tr::Vertex& tr::Vertex::normalize()
{
	position.normalize();
	normal.normalize();
	textureCoord.normalize();

	return *this;
}
