#pragma once

#include "trQuadFloat.hpp"
#include "trQuadTransformedVertex.hpp"
#include "trRect.hpp"

namespace tr
{
	struct Triangle
	{
	public:
		                                Triangle(std::array<TransformedVertex,3>&& vertices, const size_t shaderIndex, const size_t rasterizationParamsIndex);

	public:
		std::array<TransformedVertex,3> vertices;
		Rect                            boundingBox;
		size_t                          shaderIndex;
		size_t                          rasterizationParamsIndex;
	};
}