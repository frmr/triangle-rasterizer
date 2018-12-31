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

	const float           halfWidth  = float(colorBuffer.getWidth())  / 2.0f;
	const float           halfHeight = float(colorBuffer.getHeight()) / 2.0f;
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

	pixels.reserve(dx < dy ? dy : dx);

	const int sx = end.x >= start.x ? 1 : -1;
	const int sy = end.y >= start.y ? 1 : -1;

	pixels.push_back(start);

	if (start.x == end.x && start.y == end.y)
	{
		return;
	}

	if (dy <= dx)
	{
		const int d1 = dy << 1;
		const int d2 = (dy - dx) << 1;
		int       d = (dy << 1) - dx;
		int       x = start.x + sx;
		int       y = start.y;

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
	for (int i = 0; i < triangles.size();)
	{
		const Triangle& triangle                   = triangles[i];
		unsigned char   vertexClipBitFields[3]     = { 0, 0, 0 };
		unsigned char   vertexEqualityBitFields[3] = { 0, 0, 0 };

		for (size_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
		{
			const Vertex&   vertex      = triangle.vertices[vertexIndex];
			constexpr float margin      = 0.0001f;
			const float     wLessMargin = vertex.position.w - margin;
			const float     wPlusMargin = vertex.position.w + margin;

			//vertexClipBitFields[vertexIndex] |= (vertex.position.w <=  0          ) ? wBitMask      : 0;
			vertexClipBitFields[vertexIndex] |= (vertex.position.x <  -wPlusMargin) ? leftBitMask   : 0;
			vertexClipBitFields[vertexIndex] |= (vertex.position.x >   wPlusMargin) ? rightBitMask  : 0;
			vertexClipBitFields[vertexIndex] |= (vertex.position.y <  -wPlusMargin) ? topBitMask    : 0;
			vertexClipBitFields[vertexIndex] |= (vertex.position.y >   wPlusMargin) ? bottomBitMask : 0;
			vertexClipBitFields[vertexIndex] |= (vertex.position.z <  -wPlusMargin) ? nearBitMask   : 0;
			vertexClipBitFields[vertexIndex] |= (vertex.position.z >   wPlusMargin) ? farBitMask    : 0;

			vertexEqualityBitFields[vertexIndex] |= (vertex.position.x > -wPlusMargin && vertex.position.x < -wLessMargin) ? leftBitMask   : 0;
			vertexEqualityBitFields[vertexIndex] |= (vertex.position.x >  wLessMargin && vertex.position.x <  wPlusMargin) ? rightBitMask  : 0;
			vertexEqualityBitFields[vertexIndex] |= (vertex.position.y > -wPlusMargin && vertex.position.y < -wLessMargin) ? topBitMask    : 0;
			vertexEqualityBitFields[vertexIndex] |= (vertex.position.y >  wLessMargin && vertex.position.y <  wPlusMargin) ? bottomBitMask : 0;
			vertexEqualityBitFields[vertexIndex] |= (vertex.position.z > -wPlusMargin && vertex.position.z < -wLessMargin) ? nearBitMask   : 0;
			vertexEqualityBitFields[vertexIndex] |= (vertex.position.z >  wLessMargin && vertex.position.z <  wPlusMargin) ? farBitMask    : 0;
		}

		if (!(vertexClipBitFields[0] | vertexClipBitFields[1] | vertexClipBitFields[2]))
		{
			++i;
		}
		else if ((vertexClipBitFields[0] | vertexEqualityBitFields[0]) &
		         (vertexClipBitFields[1] | vertexEqualityBitFields[1]) &
		         (vertexClipBitFields[2] | vertexEqualityBitFields[2]))
		{
			triangles.erase(triangles.begin() + i);
		}
		else
		{
			constexpr EdgeInfo edges[3] = {
				{ 0, 1, 2 },
				{ 1, 2, 0 },
				{ 2, 0, 1 }
			};

			for (const EdgeInfo& edge : edges)
			{
				const unsigned char firstClipField      = vertexClipBitFields[edge.firstVertexIndex];
				const unsigned char secondClipField     = vertexClipBitFields[edge.secondVertexIndex];
				const unsigned char clipFieldXor        = firstClipField ^ secondClipField;
				const unsigned char firstEqualityField  = vertexEqualityBitFields[edge.firstVertexIndex];
				const unsigned char secondEqualityField = vertexEqualityBitFields[edge.secondVertexIndex];

				if (clipFieldXor & ~(firstEqualityField | secondEqualityField))
				{
					const Vertex firstVertex    = triangle.vertices[edge.firstVertexIndex];
					const Vertex secondVertex   = triangle.vertices[edge.secondVertexIndex];
					const Vertex oppositeVertex = triangle.vertices[edge.oppositeVertexIndex];
					Vertex       intersection;

					if      (clipFieldXor & leftBitMask)   { intersection = lineFrustumIntersectionLeft(  firstVertex, secondVertex); }
					else if (clipFieldXor & rightBitMask)  { intersection = lineFrustumIntersectionRight( firstVertex, secondVertex); }
					else if (clipFieldXor & topBitMask)    { intersection = lineFrustumIntersectionTop(   firstVertex, secondVertex); }
					else if (clipFieldXor & bottomBitMask) { intersection = lineFrustumIntersectionBottom(firstVertex, secondVertex); }
					else if (clipFieldXor & nearBitMask)   { intersection = lineFrustumIntersectionNear(  firstVertex, secondVertex); }
					else if (clipFieldXor & farBitMask)    { intersection = lineFrustumIntersectionFar(   firstVertex, secondVertex); }
					else
					{
						assert(false);
					}

					triangles.push_back(Triangle{ { firstVertex,  intersection,   oppositeVertex } });
					triangles.push_back(Triangle{ { secondVertex, oppositeVertex, intersection   } });

					triangles.erase(triangles.begin() + i);

					break;
				}
			}
		}
	}
}

tr::Vertex tr::Rasterizer::lineFrustumIntersection(const Vertex& lineStart, const Vertex& lineEnd, const float alpha)
{
	const Vector4 position     = lineStart.position     + (lineEnd.position     - lineStart.position)     * alpha;
	const Vector3 normal       = lineStart.normal       + (lineEnd.normal       - lineStart.normal)       * alpha;
	const Vector2 textureCoord = lineStart.textureCoord + (lineEnd.textureCoord - lineStart.textureCoord) * alpha;
	
	return { position, normal, textureCoord };
}

tr::Vertex tr::Rasterizer::lineFrustumIntersectionRight(const Vertex& lineStart, const Vertex& lineEnd)
{
	const float alpha = (lineStart.position.w - lineStart.position.x) / (lineEnd.position.x - lineStart.position.x - lineEnd.position.w + lineStart.position.w);
	return lineFrustumIntersection(lineStart, lineEnd, alpha);
}

tr::Vertex tr::Rasterizer::lineFrustumIntersectionLeft(const Vertex& lineStart, const Vertex& lineEnd)
{
	const float alpha = (-lineStart.position.w - lineStart.position.x) / (lineEnd.position.x - lineStart.position.x + lineEnd.position.w - lineStart.position.w);
	return lineFrustumIntersection(lineStart, lineEnd, alpha);
}

tr::Vertex tr::Rasterizer::lineFrustumIntersectionBottom(const Vertex& lineStart, const Vertex& lineEnd)
{
	const float alpha = (lineStart.position.w - lineStart.position.y) / (lineEnd.position.y - lineStart.position.y - lineEnd.position.w + lineStart.position.w);
	return lineFrustumIntersection(lineStart, lineEnd, alpha);
}

tr::Vertex tr::Rasterizer::lineFrustumIntersectionTop(const Vertex& lineStart, const Vertex& lineEnd)
{
	const float alpha = (-lineStart.position.w - lineStart.position.y) / (lineEnd.position.y - lineStart.position.y + lineEnd.position.w - lineStart.position.w);
	return lineFrustumIntersection(lineStart, lineEnd, alpha);
}

tr::Vertex tr::Rasterizer::lineFrustumIntersectionFar(const Vertex& lineStart, const Vertex& lineEnd)
{
	const float alpha = (lineStart.position.w - lineStart.position.z) / (lineEnd.position.z - lineStart.position.z - lineEnd.position.w + lineStart.position.w);
	return lineFrustumIntersection(lineStart, lineEnd, alpha);
}

tr::Vertex tr::Rasterizer::lineFrustumIntersectionNear(const Vertex& lineStart, const Vertex& lineEnd)
{
	const float alpha = (-lineStart.position.w - lineStart.position.z) / (lineEnd.position.z - lineStart.position.z + lineEnd.position.w - lineStart.position.w);
	return lineFrustumIntersection(lineStart, lineEnd, alpha);
}

void tr::Rasterizer::drawPoint(const Coord& position, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer)
{
	if (position.isValid(800, 600))
	{
		colorBuffer.at(position.x, position.y) = std::numeric_limits<uint32_t>::max();
		depthBuffer.at(position.x, position.y) = position.depth;
	}
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
