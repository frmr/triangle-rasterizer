#include "trRasterizer.hpp"
#include "trCoord.hpp"
#include "trEdgeInfo.hpp"
#include "trVertexClipBitMasks.hpp"
#include <cassert>

tr::Rasterizer::Rasterizer() :
	m_primitive(Primitive::Triangles),
	m_matrix(),
	m_depthMode(DepthMode::ReadWrite),
	m_textureMode(TextureMode::Perspective),
	m_textureWrappingMode(TextureWrappingMode::Repeat)
{
}

void tr::Rasterizer::draw(std::vector<Vertex> vertices, const ColorBuffer& texture, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
{
	if (colorBuffer.getWidth() == 0 || colorBuffer.getHeight() == 0)
	{
		return;
	}

	const float halfWidth  = float(colorBuffer.getWidth())  / 2.0f;
	const float halfHeight = float(colorBuffer.getHeight()) / 2.0f;

	for (auto& vertex : vertices)
	{
		vertex.position = m_matrix * vertex.position;
	}

	if (m_primitive == Primitive::Triangles)
	{
		for (std::vector<Vertex>::const_iterator it = vertices.begin(); it < vertices.end() - 2; it += 3)
		{
			clipAndDrawTriangle({ *it, *(it + 1), *(it + 2) }, texture, halfWidth, halfHeight, colorBuffer, depthBuffer);
		}
	}
	else if (m_primitive == Primitive::TriangleStrip)
	{

	}
	else if (m_primitive == Primitive::TriangleFan)
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

void tr::Rasterizer::setDepthMode(const DepthMode depthMode)
{
	m_depthMode = depthMode;
}

void tr::Rasterizer::setTextureMode(const TextureMode textureMode)
{
	m_textureMode = textureMode;
}

void tr::Rasterizer::setTextureWrappingMode(const TextureWrappingMode textureWrappingMode)
{
	m_textureWrappingMode = textureWrappingMode;
}

tr::Vertex tr::Rasterizer::lineFrustumIntersection(const Vertex& lineStart, const Vertex& lineEnd, const Axis axis, const bool negativeW)
{
	const float   alpha        = negativeW ?
	                             (-lineStart.position.w  - lineStart.position[axis]) / (lineEnd.position[axis] - lineStart.position[axis] + lineEnd.position.w - lineStart.position.w) :
	                             ( lineStart.position.w  - lineStart.position[axis]) / (lineEnd.position[axis] - lineStart.position[axis] - lineEnd.position.w + lineStart.position.w);

	const Vector4 position     = lineStart.position     + (lineEnd.position     - lineStart.position)     * alpha;
	const Vector3 normal       = lineStart.normal       + (lineEnd.normal       - lineStart.normal)       * alpha;
	const Vector2 textureCoord = lineStart.textureCoord + (lineEnd.textureCoord - lineStart.textureCoord) * alpha;
	
	return { position, normal, textureCoord };
}

void tr::Rasterizer::drawTriangle(std::array<Vertex, 3> vertices, const ColorBuffer& texture, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
{
	perspectiveDivide(vertices);

	if (orientPoint(vertices[0].position, vertices[1].position, vertices[2].position) >= 0.0f)
	{
		return;
	}

	viewportTransformation(vertices, halfWidth, halfHeight);

	const uint16_t minX = std::min({ uint16_t(vertices[0].position.x), uint16_t(vertices[1].position.x), uint16_t(vertices[2].position.x) });
	const uint16_t minY = std::min({ uint16_t(vertices[0].position.y), uint16_t(vertices[1].position.y), uint16_t(vertices[2].position.y) });
	const uint16_t maxX = std::max({ uint16_t(vertices[0].position.x), uint16_t(vertices[1].position.x), uint16_t(vertices[2].position.x) });
	const uint16_t maxY = std::max({ uint16_t(vertices[0].position.y), uint16_t(vertices[1].position.y), uint16_t(vertices[2].position.y) });

	pixelShift(vertices);
	sortVertices(vertices);
	fillTriangle(vertices, texture, colorBuffer, depthBuffer);
}

void tr::Rasterizer::clipAndDrawTriangle(const std::array<Vertex, 3>& vertices, const ColorBuffer& texture, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
{
	std::array<unsigned char, 3> vertexClipBitFields     = { 0, 0, 0 };
	std::array<unsigned char, 3> vertexEqualityBitFields = { 0, 0, 0 };

	for (size_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
	{
		const Vertex&   vertex           = vertices[vertexIndex];
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
		drawTriangle(vertices, texture, halfWidth, halfHeight, colorBuffer, depthBuffer);
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
			const Vertex firstVertex    = vertices[edge.firstVertexIndex];
			const Vertex secondVertex   = vertices[edge.secondVertexIndex];
			const Vertex oppositeVertex = vertices[edge.oppositeVertexIndex];

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

				clipAndDrawTriangle({ firstVertex,  intersection,   oppositeVertex }, texture, halfWidth, halfHeight, colorBuffer, depthBuffer);
				clipAndDrawTriangle({ secondVertex, oppositeVertex, intersection   }, texture, halfWidth, halfHeight, colorBuffer, depthBuffer);

				break;
			}
		}
	}
}

void tr::Rasterizer::pixelShift(std::array<Vertex, 3>& vertices)
{
	for (Vertex& vertex : vertices)
	{
		vertex.position.x -= 0.5f;
		vertex.position.y -= 0.5f;
	}
}

void tr::Rasterizer::perspectiveDivide(std::array<Vertex, 3>& vertices) const
{
	for (Vertex& vertex : vertices)
	{		
		if (m_textureMode == TextureMode::Perspective)
		{
			vertex.textureCoord /= vertex.position.w;
			vertex.inverseW      = 1.0f / vertex.position.w;
		}

		vertex.normal       /= vertex.position.w;
		vertex.position     /= vertex.position.w;
	}
}

void tr::Rasterizer::viewportTransformation(std::array<Vertex, 3>& vertices, const float halfWidth, const float halfHeight)
{
	for (Vertex& vertex : vertices)
	{
		vertex.position.x = vertex.position.x * halfWidth + halfWidth;
		vertex.position.y = halfHeight - vertex.position.y * halfHeight;
	}
}

void tr::Rasterizer::sortVertices(std::array<Vertex, 3>& vertices)
{
	for (int iteration = 0; iteration < 2; ++iteration)
	{
		for (size_t vertexIndex = 0; vertexIndex < 2; ++vertexIndex)
		{
			if (vertices[vertexIndex].position.y > vertices[vertexIndex+1].position.y)
			{
				Vertex temp = vertices[vertexIndex];

				vertices[vertexIndex]   = vertices[vertexIndex+1];
				vertices[vertexIndex+1] = temp;
			}
		}
	}
}

void tr::Rasterizer::fillTriangle(const std::array<Vertex, 3>& vertices, const ColorBuffer& texture, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
{
	const Vertex topToMiddleVector    = (vertices[1] - vertices[0]).normalize();
	const Vertex topToBottomVector    = (vertices[2] - vertices[0]).normalize();
	const Vertex middleToBottomVector = (vertices[2] - vertices[1]).normalize();

	const bool   middleVertexLeft     =  topToMiddleVector.position.x <= topToBottomVector.position.x;

	fillBottomHeavyTriangle(vertices, topToMiddleVector, topToBottomVector, middleToBottomVector, middleVertexLeft, texture, colorBuffer, depthBuffer);
	fillTopHeavyTriangle(   vertices, topToMiddleVector, topToBottomVector, middleToBottomVector, middleVertexLeft, texture, colorBuffer, depthBuffer);
}

void tr::Rasterizer::fillBottomHeavyTriangle(const std::array<Vertex, 3>& vertices, const Vertex& topToMiddleVector, const Vertex& topToBottomVector, const Vertex& middleToBottomVector, const bool middleVertexLeft, const ColorBuffer& texture, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
{
	if (vertices[0].position.y != vertices[1].position.y)
	{
		const size_t  firstY          = size_t(std::ceil(vertices[0].position.y));

		const float   topToFirstYDiff = float(firstY) - vertices[0].position.y;

		const Vertex& leftVector      = middleVertexLeft ? topToMiddleVector : topToBottomVector;
		const Vertex& rightVector     = middleVertexLeft ? topToBottomVector : topToMiddleVector;

		const float   leftRatio       = topToFirstYDiff / leftVector.position.y;
		const float   rightRatio      = topToFirstYDiff / rightVector.position.y;

		const Vertex  startLeft       = vertices[0] + leftVector  * leftRatio;
		const Vertex  startRight      = vertices[0] + rightVector * rightRatio;

		const size_t  targetY         = size_t(std::ceil(vertices[1].position.y));

		fillTriangle(leftVector, rightVector, firstY, targetY, startLeft, startRight, texture, colorBuffer, depthBuffer);
	}
}

void tr::Rasterizer::fillTopHeavyTriangle(const std::array<Vertex, 3>& vertices, const Vertex& topToMiddleVector, const Vertex& topToBottomVector, const Vertex& middleToBottomVector, const bool middleVertexLeft, const ColorBuffer& texture, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
{
	if (vertices[1].position.y != vertices[2].position.y)
	{
		const size_t  firstY         = size_t(std::ceil(vertices[1].position.y));

		const float   middleToFirstY = float(firstY) - vertices[1].position.y;
		const float   topToFirstY    = float(firstY) - vertices[0].position.y;

		const Vertex& leftVector     = middleVertexLeft ? middleToBottomVector : topToBottomVector;
		const Vertex& rightVector    = middleVertexLeft ? topToBottomVector    : middleToBottomVector;

		const float   ratioLeft      = (middleVertexLeft ? middleToFirstY : topToFirstY   ) / leftVector.position.y;
		const float   ratioRight     = (middleVertexLeft ? topToFirstY    : middleToFirstY) / rightVector.position.y;

		const Vertex  startLeft      = (middleVertexLeft ? vertices[1] : vertices[0]) + leftVector  * ratioLeft;
		const Vertex  startRight     = (middleVertexLeft ? vertices[0] : vertices[1]) + rightVector * ratioRight;

		const size_t  targetY        = size_t(std::ceil(vertices[2].position.y));

		fillTriangle(leftVector, rightVector, firstY, targetY, startLeft, startRight, texture, colorBuffer, depthBuffer);
	}
}

void tr::Rasterizer::fillTriangle(const Vertex& leftVector, const Vertex& rightVector, const size_t firstY, const size_t targetY, const Vertex& leftStart, const Vertex& rightStart, const ColorBuffer& texture, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
{
	const Vertex leftChange   = leftVector  / leftVector.position.y;
	const Vertex rightChange  = rightVector / rightVector.position.y;

	Vertex       currentLeft  = leftStart;
	Vertex       currentRight = rightStart;

	for (size_t currentY = firstY; currentY < targetY; ++currentY, currentLeft += leftChange, currentRight += rightChange)
	{
		const Vertex leftToRightVector = (currentRight - currentLeft) / (currentRight.position.x - currentLeft.position.x);
		const size_t firstX            = size_t(std::ceil(currentLeft.position.x));
		const size_t lastX             = size_t(std::ceil(currentRight.position.x));
		const float  leftToFirstX      = firstX - currentLeft.position.x;
		const float  ratio             = leftToFirstX / (lastX - firstX);
		Vertex       pixel             = currentLeft + leftToRightVector * ratio;
		Color*       colorPointer      = colorBuffer.getData() + (currentY * colorBuffer.getWidth() + firstX);
		float*       depthPointer      = depthBuffer.getData() + (currentY * depthBuffer.getWidth() + firstX);
	
		for (size_t x = firstX; x < lastX; ++x, ++colorPointer, ++depthPointer, pixel += leftToRightVector)
		{
			if (!(m_depthMode & DepthMode::Read) || pixel.position.z < *depthPointer)
			{
				Vector2 textureCoord = (m_textureMode == TextureMode::Perspective) ? pixel.textureCoord / pixel.inverseW : pixel.textureCoord;

				*colorPointer = texture.getAt(textureCoord.x, textureCoord.y, false, m_textureWrappingMode);

				if (m_depthMode & DepthMode::Write)
				{
					*depthPointer = pixel.position.z;
				}
			}
		}
	}
}

float tr::Rasterizer::orientPoint(const Vector4& lineStart, const Vector4& lineEnd, const Vector4& point)
{
	return (lineEnd.x - lineStart.x) * (point.y - lineStart.y) - (lineEnd.y - lineStart.y) * (point.x - lineStart.x);
}
