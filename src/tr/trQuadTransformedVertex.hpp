#pragma once

#include "trQuadVec2.hpp"
#include "trQuadVec3.hpp"
#include "trTransformedVertex.hpp"

namespace tr
{
	struct QuadTransformedVertex
	{
		                      QuadTransformedVertex(const TransformedVertex& transformedVertex);
		                      QuadTransformedVertex(const QuadVec3& worldPosition, const QuadVec3& projectedPosition, const QuadVec3& normal, const QuadVec2& textureCoord, const QuadFloat& inverseW);

		QuadTransformedVertex operator+(const QuadTransformedVertex& rhs) const;
		QuadTransformedVertex operator*(const QuadFloat& rhs) const;

		QuadVec3              worldPosition;
		QuadVec3              projectedPosition;
		QuadVec3              normal;
		QuadVec2              textureCoord;
		QuadFloat             inverseW;
	};
}