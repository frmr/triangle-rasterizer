#pragma once

#include "trAxis.hpp"
#include "trTexture.hpp"
#include "trCoord.hpp"
#include "trCullFaceMode.hpp"
#include "trDepthBuffer.hpp"
#include "trEdgeInfo.hpp"
#include "trError.hpp"
#include "trPrimitive.hpp"
#include "trTextureMode.hpp"
#include "trTextureWrappingMode.hpp"
#include "trVertex.hpp"
#include "trVertexClipBitMasks.hpp"
#include "../matrix/Matrices.h"

#include <vector>
#include <array>
#include <cassert>

namespace tr
{
	template <typename TShader>
	class Rasterizer
	{
	public:
		Rasterizer() :
			m_primitive(Primitive::Triangles),
			m_projectionViewMatrix(),
			m_modelMatrix(),
			m_modelNormalRotationMatrix(),
			m_depthTest(true),
			m_textureMode(TextureMode::Perspective),
			m_cullFaceMode(CullFaceMode::Back),
			m_interlaceOffset(0),
			m_interlaceStep(1)
		{
		}

		Error draw(std::vector<Vertex> vertices, const TShader& shader, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			if (colorBuffer.getWidth() == 0 || colorBuffer.getHeight() == 0)
			{
				return Error::InvalidBufferSize;
			}

			if (colorBuffer.getWidth() != depthBuffer.getWidth() && colorBuffer.getHeight() != depthBuffer.getHeight())
			{
				return Error::BufferSizeMismatch;
			}

			const float halfWidth  = float(colorBuffer.getWidth())  / 2.0f;
			const float halfHeight = float(colorBuffer.getHeight()) / 2.0f;

			for (auto& vertex : vertices)
			{
				vertex.position = m_projectionViewMatrix * m_modelMatrix * vertex.worldPosition;
				vertex.normal   = (m_modelNormalRotationMatrix * vertex.normal).normalize();
			}

			if (m_primitive == Primitive::Triangles)
			{
				for (std::vector<Vertex>::const_iterator it = vertices.begin(); it < vertices.end() - 2; it += 3)
				{
					clipAndDrawTriangle({ *it, *(it + 1), *(it + 2) }, shader, halfWidth, halfHeight, colorBuffer, depthBuffer);
				}
			}
			else if (m_primitive == Primitive::TriangleStrip)
			{
				bool   reverse    = false;
				size_t lastIndex  = 0;
				size_t firstIndex = 1;

				for (size_t newIndex = 2; newIndex < vertices.size(); ++newIndex)
				{
					clipAndDrawTriangle({ vertices[reverse ? newIndex : lastIndex], vertices[firstIndex], vertices[reverse ? lastIndex : newIndex] }, shader, halfWidth, halfHeight, colorBuffer, depthBuffer);

					firstIndex = lastIndex;
					lastIndex  = newIndex;
					reverse    = !reverse;
				}
			}
			else if (m_primitive == Primitive::TriangleFan)
			{
				for (std::vector<Vertex>::const_iterator it = vertices.begin() + 1; it < vertices.end() - 1; it += 1)
				{
					clipAndDrawTriangle({ vertices.front(), *it, *(it + 1) }, shader, halfWidth, halfHeight, colorBuffer, depthBuffer);
				}
			}

			return Error::Success;
		}

		void setPrimitive(const Primitive primitive)
		{
			m_primitive = primitive;
		}

		void setProjectionViewMatrix(const Matrix4& matrix)
		{
			m_projectionViewMatrix = matrix;
		}

		void setModelMatrix(const Matrix4& matrix)
		{
			m_modelMatrix = matrix;

			m_modelNormalRotationMatrix.set(
				m_modelMatrix.get()[0],
				m_modelMatrix.get()[1],
				m_modelMatrix.get()[2],
				m_modelMatrix.get()[4],
				m_modelMatrix.get()[5],
				m_modelMatrix.get()[6],
				m_modelMatrix.get()[8],
				m_modelMatrix.get()[9],
				m_modelMatrix.get()[10]
			);

			m_modelNormalRotationMatrix.invert().transpose();
		}

		void setDepthTest(const bool depthTest)
		{
			m_depthTest = depthTest;
		}

		void setTextureMode(const TextureMode textureMode)
		{
			m_textureMode = textureMode;
		}

		void setCullFaceMode(const CullFaceMode cullFaceMode)
		{
			m_cullFaceMode = cullFaceMode;
		}

		Error setInterlace(const size_t offset, const size_t step)
		{
			if (step == 0)
			{
				return Error::InvalidInterlaceStep;
			}

			m_interlaceOffset = offset;
			m_interlaceStep   = step;

			return Error::Success;
		}

	private:
		static Vertex lineFrustumIntersection(const Vertex& lineStart, const Vertex& lineEnd, const tr::Axis axis, const bool negativeW)
		{
			const float   alpha = negativeW ?
	                             (-lineStart.position.w  - lineStart.position[axis]) / (lineEnd.position[axis] - lineStart.position[axis] + lineEnd.position.w - lineStart.position.w) :
	                             ( lineStart.position.w  - lineStart.position[axis]) / (lineEnd.position[axis] - lineStart.position[axis] - lineEnd.position.w + lineStart.position.w);

			const Vector4 worldPosition	= lineStart.worldPosition + (lineEnd.worldPosition - lineStart.worldPosition) * alpha;
			const Vector4 position      = lineStart.position      + (lineEnd.position      - lineStart.position)      * alpha;
			const Vector3 normal        = lineStart.normal        + (lineEnd.normal        - lineStart.normal)        * alpha;
			const Vector2 textureCoord  = lineStart.textureCoord  + (lineEnd.textureCoord  - lineStart.textureCoord)  * alpha;
	
			return Vertex(worldPosition, position, normal, textureCoord);
		}

		void drawTriangle(std::array<Vertex, 3> vertices, const TShader& shader, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			perspectiveDivide(vertices);

			const float pointValue = orientPoint(vertices[0].position, vertices[1].position, vertices[2].position);

			if (m_cullFaceMode != CullFaceMode::None)
			{
				if ((m_cullFaceMode == CullFaceMode::Back  && pointValue >= 0.0f) ||
					(m_cullFaceMode == CullFaceMode::Front && pointValue <  0.0f))
				{
					return;
				}
			}
			
			viewportTransformation(vertices, halfWidth, halfHeight);
			pixelShift(vertices);
			sortVertices(vertices);
			fillTriangle(vertices, shader, colorBuffer, depthBuffer);
		}

		void clipAndDrawTriangle(const std::array<Vertex, 3>& vertices, const TShader& shader, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			std::array<uint8_t, 3> vertexClipBitFields     = { 0, 0, 0 };
			std::array<uint8_t, 3> vertexEqualityBitFields = { 0, 0, 0 };

			for (size_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
			{
				const Vertex&   vertex           = vertices[vertexIndex];
				constexpr float margin           = 0.0001f;
				const float     wLessMargin      = vertex.position.w - margin;
				const float     wPlusMargin      = vertex.position.w + margin;
				uint8_t&        clipBitField     = vertexClipBitFields[vertexIndex];
				uint8_t&        equalityBitField = vertexEqualityBitFields[vertexIndex];

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
				drawTriangle(vertices, shader, halfWidth, halfHeight, colorBuffer, depthBuffer);
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
					const Vertex  firstVertex    = vertices[edge.firstVertexIndex];
					const Vertex  secondVertex   = vertices[edge.secondVertexIndex];
					const Vertex  oppositeVertex = vertices[edge.oppositeVertexIndex];
					const uint8_t combinedField  = (vertexClipBitFields[edge.firstVertexIndex] ^ vertexClipBitFields[edge.secondVertexIndex]) & ~(vertexEqualityBitFields[edge.firstVertexIndex] | vertexEqualityBitFields[edge.secondVertexIndex]);

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

						clipAndDrawTriangle({ firstVertex,  intersection,   oppositeVertex }, shader, halfWidth, halfHeight, colorBuffer, depthBuffer);
						clipAndDrawTriangle({ secondVertex, oppositeVertex, intersection   }, shader, halfWidth, halfHeight, colorBuffer, depthBuffer);

						break;
					}
				}
			}
		}

		static void pixelShift(std::array<Vertex, 3>& vertices)
		{
			for (Vertex& vertex : vertices)
			{
				vertex.position.x -= 0.5f;
				vertex.position.y -= 0.5f;
			}
		}

		void perspectiveDivide(std::array<Vertex, 3>& vertices) const
		{
			for (Vertex& vertex : vertices)
			{		
				if (m_textureMode == TextureMode::Perspective)
				{
					vertex.textureCoord  /= vertex.position.w;
				}

				vertex.inverseW       = 1.0f / vertex.position.w;
				vertex.worldPosition /= vertex.position.w;
				vertex.normal        /= vertex.position.w;
				vertex.position      /= vertex.position.w;
			}
		}

		static void viewportTransformation(std::array<Vertex,3>& vertices, const float halfWidth, const float halfHeight)
		{
			for (Vertex& vertex : vertices)
			{
				vertex.position.x = vertex.position.x * halfWidth + halfWidth;
				vertex.position.y = halfHeight - vertex.position.y * halfHeight;
			}
		}

		static void sortVertices(std::array<Vertex,3>& vertices)
		{
			for (uint8_t iteration = 0; iteration < 2; ++iteration)
			{
				for (size_t vertexIndex = 0; vertexIndex < 2; ++vertexIndex)
				{
					if (vertices[vertexIndex].position.y > vertices[vertexIndex+1].position.y)
					{
						const Vertex temp = vertices[vertexIndex];

						vertices[vertexIndex]   = vertices[vertexIndex+1];
						vertices[vertexIndex+1] = temp;
					}
				}
			}
		}

		void fillTriangle(const std::array<Vertex,3>& vertices, const TShader& shader, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			const Vertex topToMiddleVector    = (vertices[1] - vertices[0]).normalize();
			const Vertex topToBottomVector    = (vertices[2] - vertices[0]).normalize();
			const Vertex middleToBottomVector = (vertices[2] - vertices[1]).normalize();

			const bool   middleVertexLeft     =  topToMiddleVector.position.x <= topToBottomVector.position.x;

			fillBottomHeavyTriangle(vertices, shader, topToMiddleVector, topToBottomVector, middleVertexLeft, colorBuffer, depthBuffer);
			fillTopHeavyTriangle(   vertices, shader, topToBottomVector, middleToBottomVector, middleVertexLeft, colorBuffer, depthBuffer);
		}

		void fillBottomHeavyTriangle(const std::array<Vertex,3>& vertices, const TShader& shader, const Vertex& topToMiddleVector, const Vertex& topToBottomVector, const bool middleVertexLeft, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			if (vertices[1].position.y != vertices[0].position.y)
			{
				const size_t  firstY          = size_t(std::ceilf(vertices[0].position.y));

				const float   topToFirstYDiff = float(firstY) - vertices[0].position.y;

				const Vertex& leftVector      = middleVertexLeft ? topToMiddleVector : topToBottomVector;
				const Vertex& rightVector     = middleVertexLeft ? topToBottomVector : topToMiddleVector;

				const float   leftRatio       = topToFirstYDiff / leftVector.position.y;
				const float   rightRatio      = topToFirstYDiff / rightVector.position.y;

				const Vertex  startLeft       = vertices[0] + leftVector  * leftRatio;
				const Vertex  startRight      = vertices[0] + rightVector * rightRatio;

				const size_t  targetY         = size_t(std::ceilf(vertices[1].position.y));

				fillTriangle(leftVector, rightVector, firstY, targetY, startLeft, startRight, shader, colorBuffer, depthBuffer);
			}
		}

		void fillTopHeavyTriangle(const std::array<Vertex,3>& vertices, const TShader& shader, const Vertex& topToBottomVector, const Vertex& middleToBottomVector, const bool middleVertexLeft, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			if (vertices[2].position.y != vertices[1].position.y)
			{
				const size_t  firstY         = size_t(std::ceilf(vertices[1].position.y));

				const float   middleToFirstY = float(firstY) - vertices[1].position.y;
				const float   topToFirstY    = float(firstY) - vertices[0].position.y;

				const Vertex& leftVector     = middleVertexLeft ? middleToBottomVector : topToBottomVector;
				const Vertex& rightVector    = middleVertexLeft ? topToBottomVector    : middleToBottomVector;

				const float   ratioLeft      = (middleVertexLeft ? middleToFirstY : topToFirstY   ) / leftVector.position.y;
				const float   ratioRight     = (middleVertexLeft ? topToFirstY    : middleToFirstY) / rightVector.position.y;

				const Vertex  startLeft      = (middleVertexLeft ? vertices[1] : vertices[0]) + leftVector  * ratioLeft;
				const Vertex  startRight     = (middleVertexLeft ? vertices[0] : vertices[1]) + rightVector * ratioRight;

				const size_t  targetY        = size_t(std::ceilf(vertices[2].position.y));

				fillTriangle(leftVector, rightVector, firstY, targetY, startLeft, startRight, shader, colorBuffer, depthBuffer);
			}
		}

		void fillTriangle(const Vertex& leftVector, const Vertex& rightVector, const size_t firstY, const size_t targetY, const Vertex& leftStart, const Vertex& rightStart, const TShader& shader, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			const Vertex leftChange       = leftVector  / leftVector.position.y;
			const Vertex rightChange      = rightVector / rightVector.position.y;
			const size_t interlacedFirstY = firstY + ((m_interlaceOffset - (firstY % m_interlaceStep)) + m_interlaceStep) % m_interlaceStep;
			size_t       rowCount         = interlacedFirstY - firstY;
			Vertex       currentLeft      = leftStart + leftChange * float(rowCount);
			Vertex       currentRight     = rightStart + rightChange * float(rowCount);

			for (size_t currentY = interlacedFirstY; currentY < targetY; rowCount += m_interlaceStep, currentY += m_interlaceStep, currentLeft = leftStart + leftChange * float(rowCount), currentRight = rightStart + rightChange * float(rowCount))
			{
				const Vertex leftToRightVector = (currentRight - currentLeft) / (currentRight.position.x - currentLeft.position.x);
				const size_t firstX            = size_t(std::ceilf(currentLeft.position.x));
				const size_t lastX             = size_t(std::ceilf(currentRight.position.x));
				const float  leftToFirstX      = float(firstX) - currentLeft.position.x;
				const float  ratio             = leftToFirstX / (currentRight.position.x - currentLeft.position.x);
				const Vertex firstPixel        = currentLeft + leftToRightVector * ratio;
				Color*       colorPointer      = colorBuffer.getData() + (currentY * colorBuffer.getWidth() + firstX);
				float*       depthPointer      = depthBuffer.getData() + (currentY * depthBuffer.getWidth() + firstX);
				size_t       columnCount       = 0;

				for (size_t x = firstX; x < lastX; ++x, ++colorPointer, ++depthPointer, ++columnCount)
				{
					const Vertex pixel = firstPixel + leftToRightVector * float(columnCount);

					if (!m_depthTest || pixel.position.z < *depthPointer)
					{
						Vector2 textureCoord = pixel.textureCoord;

						if (m_textureMode == TextureMode::Perspective)
							textureCoord /= pixel.inverseW;

						shader.draw(pixel.position, pixel.worldPosition / pixel.inverseW, pixel.normal / pixel.inverseW, textureCoord, *colorPointer, *depthPointer);
					}
				}
			}
		}

		static float orientPoint(const Vector4& lineStart, const Vector4& lineEnd, const Vector4& point)
		{
			return (lineEnd.x - lineStart.x) * (point.y - lineStart.y) - (lineEnd.y - lineStart.y) * (point.x - lineStart.x);
		}

	private:
		Primitive    m_primitive;
		Matrix4      m_projectionViewMatrix;
		Matrix4      m_modelMatrix;
		Matrix3      m_modelNormalRotationMatrix;
		bool         m_depthTest;
		TextureMode  m_textureMode;
		CullFaceMode m_cullFaceMode;
		size_t       m_interlaceOffset;
		size_t       m_interlaceStep;
	};
}