#include "trTriangle.hpp"

tr::Triangle::Triangle(const std::array<TransformedVertex, 3>& vertices, const size_t shaderIndex) :
	shaderIndex(shaderIndex),
	boundingBox(vertices),
	quadA01(4.0f * (vertices[0].projectedPosition.y - vertices[1].projectedPosition.y)),
	quadB01(1.0f * (vertices[1].projectedPosition.x - vertices[0].projectedPosition.x)),
	quadA12(4.0f * (vertices[1].projectedPosition.y - vertices[2].projectedPosition.y)),
	quadB12(1.0f * (vertices[2].projectedPosition.x - vertices[1].projectedPosition.x)),
	quadA20(4.0f * (vertices[2].projectedPosition.y - vertices[0].projectedPosition.y)),
	quadB20(1.0f * (vertices[0].projectedPosition.x - vertices[2].projectedPosition.x)),
	quadVertex0(QuadTransformedVertex(vertices[0])),
	quadVertex1(QuadTransformedVertex(vertices[1])),
	quadVertex2(QuadTransformedVertex(vertices[2])),
	points(QuadFloat(float(boundingBox.getMinX()), float(boundingBox.getMinX() + 1), float(boundingBox.getMinX() + 2), float(boundingBox.getMinX() + 3)),
	       QuadFloat(float(boundingBox.getMinY()), float(boundingBox.getMinY()    ), float(boundingBox.getMinY()    ), float(boundingBox.getMinY()    )),
	       0.0f
	),
	rowWeights0(orientPoints(quadVertex1.projectedPosition, quadVertex2.projectedPosition, points)),
	rowWeights1(orientPoints(quadVertex2.projectedPosition, quadVertex0.projectedPosition, points)),
	rowWeights2(orientPoints(quadVertex0.projectedPosition, quadVertex1.projectedPosition, points)),
	quadArea(orientPoints(quadVertex0.projectedPosition, quadVertex1.projectedPosition, quadVertex2.projectedPosition))
{
}

tr::QuadFloat tr::Triangle::orientPoints(const QuadVec3& lineStarts, const QuadVec3& lineEnds, const QuadVec3& points)
{
	return (lineEnds.x - lineStarts.x) * (points.y - lineStarts.y) - (lineEnds.y - lineStarts.y) * (points.x - lineStarts.x);
}
