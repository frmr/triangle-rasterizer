#pragma once

#include "trQuadFloat.hpp"
#include "trQuadTransformedVertex.hpp"
#include "trRect.hpp"

namespace tr
{
	class Triangle
	{
	public:
		                      Triangle(const std::array<TransformedVertex,3>& vertices, const size_t shaderIndex, const size_t rasterizationParamsIndex);

	private:
		static QuadFloat      orientPoints(const QuadVec3& lineStarts, const QuadVec3& lineEnds, const QuadVec3& points);

	public:
		size_t                shaderIndex;
		size_t                rasterizationParamsIndex;

		Rect                  boundingBox;

		QuadFloat             quadA01;
		QuadFloat             quadB01;
		QuadFloat             quadA12;
		QuadFloat             quadB12;
		QuadFloat             quadA20;
		QuadFloat             quadB20;

		QuadTransformedVertex quadVertex0;
		QuadTransformedVertex quadVertex1;
		QuadTransformedVertex quadVertex2;

		QuadFloat             quadArea;
	};
}