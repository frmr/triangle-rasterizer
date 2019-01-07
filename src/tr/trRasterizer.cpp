#include "trRasterizer.hpp"
#include "trCoord.hpp"
#include "trEdgeInfo.hpp"
#include "trVertexClipBitMasks.hpp"
#include <cassert>

void tr::Rasterizer::draw(std::vector<Vertex> vertices, const ColorBuffer& texture, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer)
{
	if (colorBuffer.getWidth() == 0 || colorBuffer.getHeight() == 0)
	{
		return;
	}

	const float halfWidth  = float(colorBuffer.getWidth())  / 2.0f - 0.0001f;
	const float halfHeight = float(colorBuffer.getHeight()) / 2.0f - 0.0001f;

	for (auto& vertex : vertices)
	{
		vertex.position = m_matrix * vertex.position;
	}

	if (m_primitive == Primitive::TRIANGLES)
	{
		for (std::vector<Vertex>::const_iterator it = vertices.begin(); it < vertices.end() - 2; it += 3)
		{
			clipAndDrawTriangle(Triangle{ *it, *(it + 1), *(it + 2) }, texture, halfWidth, halfHeight, colorBuffer, depthBuffer);
		}
	}
	else if (m_primitive == Primitive::TRIANGLE_STRIP)
	{

	}
	else if (m_primitive == Primitive::TRIANGLE_FAN)
	{

	}
}

void tr::Rasterizer::setPrimitive(const Primitive primitive)
{
	m_primitive = primitive;
}

void tr::Rasterizer::setMatrix(const Matrix4& matrix)
{
	m_matrix = matrix;
}

tr::Vertex tr::Rasterizer::lineFrustumIntersection(const Vertex& lineStart, const Vertex& lineEnd, const tr::Axis axis, const bool negativeW)
{
	const float   alpha        = negativeW ?
	                             (-lineStart.position.w  - lineStart.position[axis]) / (lineEnd.position[axis] - lineStart.position[axis] + lineEnd.position.w - lineStart.position.w) :
	                             ( lineStart.position.w  - lineStart.position[axis]) / (lineEnd.position[axis] - lineStart.position[axis] - lineEnd.position.w + lineStart.position.w);

	const Vector4 position     = lineStart.position     + (lineEnd.position     - lineStart.position)     * alpha;
	const Vector3 normal       = lineStart.normal       + (lineEnd.normal       - lineStart.normal)       * alpha;
	const Vector2 textureCoord = lineStart.textureCoord + (lineEnd.textureCoord - lineStart.textureCoord) * alpha;
	
	return { position, normal, textureCoord };
}

void tr::Rasterizer::drawPoint(const Vector2& point, const Color& color, const float depth, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer)
{
	colorBuffer.at(point.x, point.y) = color;
	depthBuffer.at(point.x, point.y) = depth;
}

void tr::Rasterizer::drawTriangle(const Triangle& triangle, const ColorBuffer& texture, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer)
{
	constexpr EdgeInfo edges[3] = {
		{ 0, 1, 2 },
		{ 1, 2, 0 },
		{ 2, 0, 1 }
	};

	const Vertex& vertex0 = triangle.vertices[0];
	const Vertex& vertex1 = triangle.vertices[1];
	const Vertex& vertex2 = triangle.vertices[2];

	Vector4 position0 = vertex0.position / vertex0.position.w;
	Vector4 position1 = vertex1.position / vertex1.position.w;
	Vector4 position2 = vertex2.position / vertex2.position.w;
	Coord   point;

	if (orientPoint(position0, position1, position2) >= 0.0f)
	{
		return;
	}

	position0.x = position0.x * halfWidth + halfWidth;
	position1.x = position1.x * halfWidth + halfWidth;
	position2.x = position2.x * halfWidth + halfWidth;

	position0.y = halfHeight - position0.y * halfHeight;
	position1.y = halfHeight - position1.y * halfHeight;
	position2.y = halfHeight - position2.y * halfHeight;

	uint16_t minX = std::min({ uint16_t(position0.x), uint16_t(position1.x), uint16_t(position2.x) });
	uint16_t minY = std::min({ uint16_t(position0.y), uint16_t(position1.y), uint16_t(position2.y) });
	uint16_t maxX = std::max({ uint16_t(position0.x), uint16_t(position1.x), uint16_t(position2.x) });
	uint16_t maxY = std::max({ uint16_t(position0.y), uint16_t(position1.y), uint16_t(position2.y) });

	position0.x -= 0.5f;
	position0.y -= 0.5f;
	position1.x -= 0.5f;
	position1.y -= 0.5f;
	position2.x -= 0.5f;
	position2.y -= 0.5f;

	size_t step = colorBuffer.getWidth() - (maxX - minX) - 1;

	Color* colorPointer = colorBuffer.getData() + (minY * colorBuffer.getWidth() + minX);
	float* depthPointer = depthBuffer.getData() + (minY * depthBuffer.getWidth() + minX);

	const float triangleAreaInverse = 1.0f / orientPoint(position0, position1, position2);

	for (point.y = minY; point.y <= maxY; ++point.y)
	{
		for (point.x = minX; point.x <= maxX; ++point.x, ++colorPointer, ++depthPointer)
		{
			const Vector2 pointFloat(float(point.x), float(point.y));

			float weight0 = orientPoint(position1, position2, pointFloat);
			float weight1 = orientPoint(position2, position0, pointFloat);
			float weight2 = orientPoint(position0, position1, pointFloat);

			if (weight0 >= 0.0f && weight1 >= 0.0f && weight2 >= 0.0f)
			{
				weight0 *= triangleAreaInverse;
				weight1 *= triangleAreaInverse;
				weight2 *= triangleAreaInverse;

				const float depth = interpolate(weight0, position0.z, weight1, position1.z, weight2, position2.z);

				if (depth < *depthPointer)
				{
					const Vector2 interpolatedTextureCoord(
						interpolate(weight0, vertex0.textureCoord.x, weight1, vertex1.textureCoord.x, weight2, vertex2.textureCoord.x),
						interpolate(weight0, vertex0.textureCoord.y, weight1, vertex1.textureCoord.y, weight2, vertex2.textureCoord.y)
					);

					const Color color = texture.getAt(size_t(interpolatedTextureCoord.x * (texture.getWidth() - 1)), size_t(interpolatedTextureCoord.y * (texture.getHeight() - 1)));

					*colorPointer = color;
					*depthPointer = depth;
				}
			}
		}

		colorPointer += step;
		depthPointer += step;
	}
}

void tr::Rasterizer::clipAndDrawTriangle(const Triangle& triangle, const ColorBuffer& texture, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer)
{
	std::array<unsigned char, 3> vertexClipBitFields     = { 0, 0, 0 };
	std::array<unsigned char, 3> vertexEqualityBitFields = { 0, 0, 0 };

	for (size_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
	{
		const Vertex&   vertex           = triangle.vertices[vertexIndex];
		constexpr float margin           = 0.0001f;
		const float     wLessMargin      = vertex.position.w - margin;
		const float     wPlusMargin      = vertex.position.w + margin;
		unsigned char&  clipBitField     = vertexClipBitFields[vertexIndex];
		unsigned char&  equalityBitField = vertexEqualityBitFields[vertexIndex];

		if (vertex.position.x <  -wPlusMargin) { clipBitField     |= leftBitMask;                     }
		if (vertex.position.x >   wPlusMargin) { clipBitField     |= rightBitMask;                    }
		if (vertex.position.y <  -wPlusMargin) { clipBitField     |= bottomBitMask;                   }
		if (vertex.position.y >   wPlusMargin) { clipBitField     |= topBitMask;                      }
		if (vertex.position.z <  -wPlusMargin) { clipBitField     |= nearBitMask;                     }
		if (vertex.position.z >   wPlusMargin) { clipBitField     |= farBitMask;                      }

		if (vertex.position.x <= -wLessMargin) { equalityBitField |= (~clipBitField) & leftBitMask;   }
		if (vertex.position.x >=  wLessMargin) { equalityBitField |= (~clipBitField) & rightBitMask;  }
		if (vertex.position.y <= -wLessMargin) { equalityBitField |= (~clipBitField) & bottomBitMask; }
		if (vertex.position.y >=  wLessMargin) { equalityBitField |= (~clipBitField) & topBitMask;    }
		if (vertex.position.z <= -wLessMargin) { equalityBitField |= (~clipBitField) & nearBitMask;   }
		if (vertex.position.z >=  wLessMargin) { equalityBitField |= (~clipBitField) & farBitMask;    }
	}

	if (!(vertexClipBitFields[0] | vertexClipBitFields[1] | vertexClipBitFields[2]))
	{
		drawTriangle(triangle, texture, halfWidth, halfHeight, colorBuffer, depthBuffer);
	}
	else if ((vertexClipBitFields[0] | vertexEqualityBitFields[0]) &
	         (vertexClipBitFields[1] | vertexEqualityBitFields[1]) &
	         (vertexClipBitFields[2] | vertexEqualityBitFields[2]))
	{
		return;
	}
	else
	{
		constexpr EdgeInfo edges[3] = { { 0, 1, 2 }, { 1, 2, 0 }, { 2, 0, 1 } };

		for (const EdgeInfo& edge : edges)
		{
			const Vertex firstVertex    = triangle.vertices[edge.firstVertexIndex];
			const Vertex secondVertex   = triangle.vertices[edge.secondVertexIndex];
			const Vertex oppositeVertex = triangle.vertices[edge.oppositeVertexIndex];

			const unsigned char combinedField = (vertexClipBitFields[edge.firstVertexIndex] ^ vertexClipBitFields[edge.secondVertexIndex]) & ~(vertexEqualityBitFields[edge.firstVertexIndex] | vertexEqualityBitFields[edge.secondVertexIndex]);

			if (combinedField)
			{
				Vertex intersection;

				if      (combinedField & leftBitMask)   { intersection = lineFrustumIntersection(firstVertex, secondVertex, Axis::X, true ); } 
				else if (combinedField & rightBitMask)  { intersection = lineFrustumIntersection(firstVertex, secondVertex, Axis::X, false); }
				else if (combinedField & bottomBitMask) { intersection = lineFrustumIntersection(firstVertex, secondVertex, Axis::Y, true ); }
				else if (combinedField & topBitMask)    { intersection = lineFrustumIntersection(firstVertex, secondVertex, Axis::Y, false); }
				else if (combinedField & nearBitMask)   { intersection = lineFrustumIntersection(firstVertex, secondVertex, Axis::Z, true ); }
				else if (combinedField & farBitMask)    { intersection = lineFrustumIntersection(firstVertex, secondVertex, Axis::Z, false); }
				else
				{
					assert(false);
				}

				clipAndDrawTriangle(Triangle{ firstVertex,  intersection,   oppositeVertex }, texture, halfWidth, halfHeight, colorBuffer, depthBuffer);
				clipAndDrawTriangle(Triangle{ secondVertex, oppositeVertex, intersection   }, texture, halfWidth, halfHeight, colorBuffer, depthBuffer);

				break;
			}
		}
	}
}

float tr::Rasterizer::interpolate(const float weight0, const float value0, const float weight1, const float value1, const float weight2, const float value2)
{
	return weight0 * value0 + weight1 * value1 + weight2 * value2;
}

template<typename T>
float tr::Rasterizer::orientPoint(const Vector4& lineStart, const Vector4& lineEnd, const T& point)
{
	return (lineEnd.x - lineStart.x) * (point.y - lineStart.y) - (lineEnd.y - lineStart.y) * (point.x - lineStart.x);
}
