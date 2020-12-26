#include "trQuadTransformedVertex.hpp"

tr::QuadTransformedVertex::QuadTransformedVertex(const TransformedVertex& transformedVertex) :
	worldPosition(transformedVertex.worldPosition),
	projectedPosition(transformedVertex.projectedPosition.x, transformedVertex.projectedPosition.y, transformedVertex.projectedPosition.z),
	normal(transformedVertex.normal),
	textureCoord(transformedVertex.textureCoord),
	inverseW(transformedVertex.inverseW)
{
}

tr::QuadTransformedVertex::QuadTransformedVertex(const QuadVec3& worldPosition, const QuadVec3& projectedPosition, const QuadVec3& normal, const QuadVec2& textureCoord, const QuadFloat& inverseW) :
	worldPosition(worldPosition),
	projectedPosition(projectedPosition),
	normal(normal),
	textureCoord(textureCoord),
	inverseW(inverseW)
{
}

tr::QuadTransformedVertex tr::QuadTransformedVertex::operator+(const QuadTransformedVertex& rhs) const
{
	return QuadTransformedVertex(
		worldPosition     + rhs.worldPosition,
		projectedPosition + rhs.projectedPosition,
		normal            + rhs.normal,
		textureCoord      + rhs.textureCoord,
		inverseW          + rhs.inverseW
	);
}

tr::QuadTransformedVertex tr::QuadTransformedVertex::operator*(const QuadFloat& rhs) const
{
	return QuadTransformedVertex(
		worldPosition     * rhs,
		projectedPosition * rhs,
		normal            * rhs,
		textureCoord      * rhs,
		inverseW          * rhs
	);
}
