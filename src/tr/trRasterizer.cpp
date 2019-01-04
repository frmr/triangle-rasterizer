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
		triangles.reserve(vertices.size() / 3);

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

			if (vertex.position.z < -wPlusMargin) { clipBitField |= nearBitMask; }
			if (vertex.position.z >  wPlusMargin) { clipBitField |= farBitMask;  }

			if (vertex.position.z <= -wLessMargin) { equalityBitField |= (~clipBitField) & nearBitMask; }
			if (vertex.position.z >=  wLessMargin) { equalityBitField |= (~clipBitField) & farBitMask;  }
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
				const Vertex firstVertex    = triangle.vertices[edge.firstVertexIndex];
				const Vertex secondVertex   = triangle.vertices[edge.secondVertexIndex];
				const Vertex oppositeVertex = triangle.vertices[edge.oppositeVertexIndex];
				Vertex       intersection;

				const unsigned char combinedField = (vertexClipBitFields[edge.firstVertexIndex] ^ vertexClipBitFields[edge.secondVertexIndex]) & ~(vertexEqualityBitFields[edge.firstVertexIndex] | vertexEqualityBitFields[edge.secondVertexIndex]);

				if (combinedField)
				{
					if      (combinedField & nearBitMask) { intersection = lineFrustumIntersection(firstVertex, secondVertex, Axis::Z, true ); }
					else if (combinedField & farBitMask)  { intersection = lineFrustumIntersection(firstVertex, secondVertex, Axis::Z, false); }
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

void tr::Rasterizer::drawPoint(const int x, const int y, const float depth, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer)
{
	colorBuffer.at(x, y) = std::numeric_limits<uint32_t>::max();
	depthBuffer.at(x, y) = depth;
}

void tr::Rasterizer::drawPoint(const Vector2& point, const float depth, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer)
{
	drawPoint(int(point.x), int(point.y), depth, colorBuffer, depthBuffer);
}

void tr::Rasterizer::drawTriangle(const Triangle& triangle, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer)
{
	constexpr EdgeInfo edges[3] = {
		{ 0, 1, 2 },
		{ 1, 2, 0 },
		{ 2, 0, 1 }
	};

	Vector4 vertex0 = triangle.vertices[0].position / triangle.vertices[0].position.w;
	Vector4 vertex1 = triangle.vertices[1].position / triangle.vertices[1].position.w;
	Vector4 vertex2 = triangle.vertices[2].position / triangle.vertices[2].position.w;
	Vector2 point;

	if (orientPoint(vertex0, vertex1, vertex2) >= 0.0f)
	{
		return;
	}

	vertex0.x = vertex0.x * halfWidth + halfWidth;
	vertex1.x = vertex1.x * halfWidth + halfWidth;
	vertex2.x = vertex2.x * halfWidth + halfWidth;

	vertex0.y = halfHeight - vertex0.y * halfHeight;
	vertex1.y = halfHeight - vertex1.y * halfHeight;
	vertex2.y = halfHeight - vertex2.y * halfHeight;

	float minX = std::min({ vertex0.x, vertex1.x, vertex2.x });
	float minY = std::min({ vertex0.y, vertex1.y, vertex2.y });
	float maxX = std::max({ vertex0.x, vertex1.x, vertex2.x });
	float maxY = std::max({ vertex0.y, vertex1.y, vertex2.y });
	
	minX = std::max(minX, 0.0f);
	minY = std::max(minY, 0.0f);
	maxX = std::min(maxX, 2560.0f);
	maxY = std::min(maxY, 1440.0f);

	minX = std::trunc(minX) + 0.5f;
	minY = std::trunc(minY) + 0.5f;

	for (point.x = minX; point.x < maxX; point.x += 1.0f)
	{
		for (point.y = minY; point.y < maxY; point.y += 1.0f)
		{
			const float weight0 = orientPoint(vertex1, vertex2, point);
			const float weight1 = orientPoint(vertex2, vertex0, point);
			const float weight2 = orientPoint(vertex0, vertex1, point);

			if (weight0 >= 0.0f && weight1 >= 0.0f && weight2 >= 0.0f)
			{
				// interpolate depth
				const float depth = 0.5f;

				drawPoint(point, depth, colorBuffer, depthBuffer);
			}
		}
	}
}

template<typename T>
float tr::Rasterizer::orientPoint(const Vector4& lineStart, const Vector4& lineEnd, const T& point)
{
	return (lineEnd.x - lineStart.x) * (point.y - lineStart.y) - (lineEnd.y - lineStart.y) * (point.x - lineStart.x);
}
