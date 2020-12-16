#include "trTriangle.hpp"

tr::Triangle::Triangle(std::array<TransformedVertex, 3>&& vertices, const size_t shaderIndex, const size_t rasterizationParamsIndex) :
	vertices(std::move(vertices)),
	boundingBox(vertices),
	shaderIndex(shaderIndex),
	rasterizationParamsIndex(rasterizationParamsIndex)
{
}