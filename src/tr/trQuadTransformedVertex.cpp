#include "trQuadTransformedVertex.hpp"

tr::QuadTransformedVertex::QuadTransformedVertex(const TransformedVertex& transformedVertex) :
	worldPosition(transformedVertex.worldPosition),
	projectedPosition(transformedVertex.projectedPosition.x, transformedVertex.projectedPosition.y, transformedVertex.projectedPosition.z),
	normal(transformedVertex.normal),
	textureCoord(transformedVertex.textureCoord),
	inverseW(transformedVertex.inverseW)
{
}

tr::QuadTransformedVertex tr::QuadTransformedVertex::operator+(const QuadTransformedVertex& rhs) const
{
	QuadTransformedVertex result = *this;

	result.worldPosition     += rhs.worldPosition;
	result.projectedPosition += rhs.projectedPosition;
	result.normal            += rhs.normal;
	result.textureCoord      += rhs.textureCoord;
	result.inverseW          += rhs.inverseW;

	return result;
}

tr::QuadTransformedVertex tr::QuadTransformedVertex::operator*(const QuadFloat& rhs) const
{
	QuadTransformedVertex result = *this;

	result.worldPosition     *= rhs;
	result.projectedPosition *= rhs;
	result.normal            *= rhs;
	result.textureCoord      *= rhs;
	result.inverseW          *= rhs;

	return result;
}
