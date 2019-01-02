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

	const float           halfWidth  = float(colorBuffer.getWidth())  / 2.0f - 0.0001f;
	const float           halfHeight = float(colorBuffer.getHeight()) / 2.0f - 0.0001f;
	std::vector<Triangle> triangles;

	for (auto& vertex : vertices)
	{
		vertex.position = m_matrix * vertex.position;
	}

	if (m_primitive == Primitive::TRIANGLES)
	{
		for (std::vector<Vertex>::const_iterator it = vertices.begin(); it < vertices.end() - 2; it += 3)
		{
			triangles.push_back(Triangle{ *it, *(it + 1), *(it + 2) });
		}
	}
	else if (m_primitive == Primitive::TRIANGLE_STRIP)
	{

	}
	else if (m_primitive == Primitive::TRIANGLE_FAN)
	{

	}

	clipTriangles(triangles);

	for (const Triangle& triangle : triangles)
	{
		drawTriangle(triangle, halfWidth, halfHeight, colorBuffer, depthBuffer);
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

void tr::Rasterizer::getLinePixels(const Coord& start, const Coord& end, std::vector<Coord>& pixels)
{
	const int dx = abs(end.x - start.x);
	const int dy = abs(end.y - start.y);

	const int sx = end.x >= start.x ? 1 : -1;
	const int sy = end.y >= start.y ? 1 : -1;

	pixels.reserve(dx < dy ? dy : dx);
	pixels.push_back(start);

	if (start.x == end.x && start.y == end.y)
	{
		return;
	}

	if (dy <= dx)
	{
		const int d1 = dy << 1;
		const int d2 = (dy - dx) << 1;
		int       d  = (dy << 1) - dx;
		int       x  = start.x + sx;
		int       y  = start.y;

		for (int i = 1; i <= dx; ++i, x += sx)
		{
			if (d > 0)
			{
				d += d2;
				y += sy;
			}
			else
			{
				d += d1;
			}

			pixels.push_back(Coord(x, y, start.depth));
		}
	}
	else
	{
		const int d1 = dx << 1;
		const int d2 = (dx - dy) << 1;
		int       d  = (dx << 1) - dy;
		int       x  = start.x;
		int       y  = start.y + sy;

		for (int i = 1; i <= dy; ++i, y += sy)
		{
			if (d > 0)
			{
				d += d2;
				x += sx;
			}
			else
			{
				d += d1;
			}

			pixels.push_back(Coord(x, y, start.depth));
		}
	}
}

void tr::Rasterizer::clipTriangles(std::vector<Triangle>& triangles)
{
	for (size_t triangleIndex = 0; triangleIndex < triangles.size();)
	{
		const Triangle&              triangle                = triangles[triangleIndex];
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

			//vertexClipBitFields[vertexIndex] |= (vertex.position.w <=  0          ) ? wBitMask      : 0;

			if (vertex.position.x < -wPlusMargin) { clipBitField |= leftBitMask;   }
			if (vertex.position.x >  wPlusMargin) { clipBitField |= rightBitMask;  }
			if (vertex.position.y < -wPlusMargin) { clipBitField |= topBitMask;    }
			if (vertex.position.y >  wPlusMargin) { clipBitField |= bottomBitMask; }
			if (vertex.position.z < -wPlusMargin) { clipBitField |= nearBitMask;   }
			if (vertex.position.z >  wPlusMargin) { clipBitField |= farBitMask;    }

			const unsigned char notClipField = ~clipBitField;

			if (vertex.position.x <= -wLessMargin) { equalityBitField |= notClipField & leftBitMask;   }
			if (vertex.position.x >=  wLessMargin) { equalityBitField |= notClipField & rightBitMask;  }
			if (vertex.position.y <= -wLessMargin) { equalityBitField |= notClipField & topBitMask;    }
			if (vertex.position.y >=  wLessMargin) { equalityBitField |= notClipField & bottomBitMask; }
			if (vertex.position.z <= -wLessMargin) { equalityBitField |= notClipField & nearBitMask;   }
			if (vertex.position.z >=  wLessMargin) { equalityBitField |= notClipField & farBitMask;    }
		}

		if (!(vertexClipBitFields[0] | vertexClipBitFields[1] | vertexClipBitFields[2]))
		{
			++triangleIndex;
		}
		else if ((vertexClipBitFields[0] | vertexEqualityBitFields[0]) &
		         (vertexClipBitFields[1] | vertexEqualityBitFields[1]) &
		         (vertexClipBitFields[2] | vertexEqualityBitFields[2]))
		{
			triangles.erase(triangles.begin() + triangleIndex);
		}
		else
		{
			constexpr EdgeInfo edges[3] = { { 0, 1, 2 }, { 1, 2, 0 }, { 2, 0, 1 } };

			for (const EdgeInfo& edge : edges)
			{
				const unsigned char combinedField = (vertexClipBitFields[edge.firstVertexIndex] ^ vertexClipBitFields[edge.secondVertexIndex]) & ~(vertexEqualityBitFields[edge.firstVertexIndex] | vertexEqualityBitFields[edge.secondVertexIndex]);

				if (combinedField)
				{
					const Vertex firstVertex    = triangle.vertices[edge.firstVertexIndex];
					const Vertex secondVertex   = triangle.vertices[edge.secondVertexIndex];
					const Vertex oppositeVertex = triangle.vertices[edge.oppositeVertexIndex];
					Vertex       intersection;

					if      (combinedField & leftBitMask)   { intersection = lineFrustumIntersection(firstVertex, secondVertex, Axis::X, true ); }
					else if (combinedField & rightBitMask)  { intersection = lineFrustumIntersection(firstVertex, secondVertex, Axis::X, false); }
					else if (combinedField & topBitMask)    { intersection = lineFrustumIntersection(firstVertex, secondVertex, Axis::Y, true ); }
					else if (combinedField & bottomBitMask) { intersection = lineFrustumIntersection(firstVertex, secondVertex, Axis::Y, false); }
					else if (combinedField & nearBitMask)   { intersection = lineFrustumIntersection(firstVertex, secondVertex, Axis::Z, false); }
					else if (combinedField & farBitMask)    { intersection = lineFrustumIntersection(firstVertex, secondVertex, Axis::Z, true ); }
					else
					{
						assert(false);
					}

					triangles.push_back(Triangle{ { firstVertex,  intersection,   oppositeVertex } });
					triangles.push_back(Triangle{ { secondVertex, oppositeVertex, intersection   } });

					triangles.erase(triangles.begin() + triangleIndex);

					break;
				}
			}
		}
	}
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

void tr::Rasterizer::drawPoint(const Coord& position, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer)
{
	colorBuffer.at(position.x, position.y) = std::numeric_limits<uint32_t>::max();
	depthBuffer.at(position.x, position.y) = position.depth;
}

void tr::Rasterizer::drawPoint(const Vector4& position, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer)
{
	drawPoint(Coord(int(halfWidth * position.x + halfWidth), int(halfHeight * position.y + halfHeight), position.z), colorBuffer, depthBuffer);
}

void tr::Rasterizer::drawTriangle(const Triangle& triangle, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer)
{
	constexpr EdgeInfo edges[3] = {
		{ 0, 1, 2 },
		{ 1, 2, 0 },
		{ 2, 0, 1 }
	};

	for (const auto edge : edges)
	{
		const Vertex& firstVertex  = triangle.vertices[edge.firstVertexIndex];
		const Vertex& secondVertex = triangle.vertices[edge.secondVertexIndex];

		Vector4 firstVertexScreenSpace = firstVertex.position / firstVertex.position.w;
		Vector4 secondVertexScreenSpace = secondVertex.position / firstVertex.position.w;

		drawPoint(firstVertexScreenSpace, halfWidth, halfHeight, colorBuffer, depthBuffer);
	}
}
