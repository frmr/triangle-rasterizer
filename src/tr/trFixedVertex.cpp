#include "trFixedVertex.hpp"

tr::FixedVertex& tr::FixedVertex::operator+=(const FixedVertex& rhs)
{
	position     += rhs.position;
	normal       += rhs.normal;
	textureCoord += rhs.textureCoord;
	inverseW     += rhs.inverseW;

	return *this;
}

tr::FixedVertex& tr::FixedVertex::operator-=(const FixedVertex& rhs)
{
	position     -= rhs.position;
	normal       -= rhs.normal;
	textureCoord -= rhs.textureCoord;
	inverseW     -= rhs.inverseW;

	return *this;
}

tr::FixedVertex& tr::FixedVertex::operator/=(const Fixed& rhs)
{
	position     /= rhs;
	normal       /= rhs;
	textureCoord /= rhs;
	inverseW     = sg14::divide(inverseW, rhs);

	return *this;
}

tr::FixedVertex tr::FixedVertex::operator+(const FixedVertex& rhs) const
{
	return FixedVertex {
		position     + rhs.position,
		normal       + rhs.normal,
		textureCoord + rhs.textureCoord,
		inverseW     + rhs.inverseW
	};
}

tr::FixedVertex tr::FixedVertex::operator-(const FixedVertex& rhs) const
{
	return FixedVertex {
		position     - rhs.position,
		normal       - rhs.normal,
		textureCoord - rhs.textureCoord,
		inverseW     - rhs.inverseW
	};
}

tr::FixedVertex tr::FixedVertex::operator*(const Fixed& rhs) const
{
	return FixedVertex {
		position     * rhs,
		normal       * rhs,
		textureCoord * rhs,
		inverseW     * rhs
	};
}

tr::FixedVertex tr::FixedVertex::operator/(const Fixed& rhs) const
{
	return FixedVertex {
		position.operator/(rhs),
		normal.operator/(rhs),
		textureCoord.operator/(rhs),
		sg14::divide(inverseW, rhs)
	};
}

tr::FixedVertex& tr::FixedVertex::normalize()
{
	const Fixed length = sg14::sqrt(position.x * position.x + position.y * position.y + position.z * position.z);
	
	position     /= length;
	normal       /= length;
	textureCoord /= length;
	inverseW     = sg14::divide(inverseW, length);

	return *this;
}