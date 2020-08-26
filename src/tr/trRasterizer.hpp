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
#include "trTransformedVertex.hpp"
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
			m_interlaceStep(1),
			m_depthBias(0.0f)
		{
		}

		Error draw(const std::vector<Vertex>& vertices, const TShader& shader, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			std::vector<TransformedVertex> transformedVertices;

			transformedVertices.reserve(vertices.size());

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

			for (const Vertex& vertex : vertices)
			{
				const Vector4 worldPosition = m_modelMatrix * vertex.position;

				transformedVertices.emplace_back(
					Vector3(worldPosition.x, worldPosition.y, worldPosition.z),
					m_projectionViewMatrix * m_modelMatrix * vertex.position,
					(m_modelNormalRotationMatrix * vertex.normal).normalize(),
					vertex.textureCoord
				);
			}

			if (m_primitive == Primitive::Triangles)
			{
				for (std::vector<TransformedVertex>::const_iterator it = transformedVertices.begin(); it < transformedVertices.end() - 2; it += 3)
				{
					clipAndDrawTriangle({ *it, *(it + 1), *(it + 2) }, shader, halfWidth, halfHeight, colorBuffer, depthBuffer);
				}
			}
			else if (m_primitive == Primitive::TriangleStrip)
			{
				bool   reverse    = false;
				size_t lastIndex  = 0;
				size_t firstIndex = 1;

				for (size_t newIndex = 2; newIndex < transformedVertices.size(); ++newIndex)
				{
					clipAndDrawTriangle({ transformedVertices[reverse ? newIndex : lastIndex], transformedVertices[firstIndex], transformedVertices[reverse ? lastIndex : newIndex] }, shader, halfWidth, halfHeight, colorBuffer, depthBuffer);

					firstIndex = lastIndex;
					lastIndex  = newIndex;
					reverse    = !reverse;
				}
			}
			else if (m_primitive == Primitive::TriangleFan)
			{
				for (std::vector<TransformedVertex>::const_iterator it = transformedVertices.begin() + 1; it < transformedVertices.end() - 1; it += 1)
				{
					clipAndDrawTriangle({ transformedVertices.front(), *it, *(it + 1) }, shader, halfWidth, halfHeight, colorBuffer, depthBuffer);
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

		Matrix4 getModelMatrix()
		{
			return m_modelMatrix;
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

		void setDepthBias(const float depthBias)
		{
			m_depthBias = depthBias;
		}

	private:
		static TransformedVertex lineFrustumIntersection(const TransformedVertex& lineStart, const TransformedVertex& lineEnd, const tr::Axis axis, const bool negativeW)
		{
			const float   alpha = negativeW ?
	                             (-lineStart.projectedPosition.w  - lineStart.projectedPosition[axis]) / (lineEnd.projectedPosition[axis] - lineStart.projectedPosition[axis] + lineEnd.projectedPosition.w - lineStart.projectedPosition.w) :
	                             ( lineStart.projectedPosition.w  - lineStart.projectedPosition[axis]) / (lineEnd.projectedPosition[axis] - lineStart.projectedPosition[axis] - lineEnd.projectedPosition.w + lineStart.projectedPosition.w);

			const Vector3 worldPosition	    = lineStart.worldPosition     + (lineEnd.worldPosition     - lineStart.worldPosition)     * alpha;
			const Vector4 projectedPosition = lineStart.projectedPosition + (lineEnd.projectedPosition - lineStart.projectedPosition) * alpha;
			const Vector3 normal            = lineStart.normal            + (lineEnd.normal            - lineStart.normal)            * alpha;
			const Vector2 textureCoord      = lineStart.textureCoord      + (lineEnd.textureCoord      - lineStart.textureCoord)      * alpha;
	
			return TransformedVertex(worldPosition, projectedPosition, normal, textureCoord);
		}

		void drawTriangle(std::array<TransformedVertex, 3> vertices, const TShader& shader, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			perspectiveDivide(vertices);

			const float pointValue = orientPoint(vertices[0].projectedPosition, vertices[1].projectedPosition, vertices[2].projectedPosition);

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

		void clipAndDrawTriangle(const std::array<TransformedVertex, 3>& vertices, const TShader& shader, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			std::array<uint8_t, 3> vertexClipBitFields     = { 0, 0, 0 };
			std::array<uint8_t, 3> vertexEqualityBitFields = { 0, 0, 0 };

			for (size_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
			{
				const TransformedVertex& vertex           = vertices[vertexIndex];
				constexpr float          margin           = 0.0001f;
				const float              wLessMargin      = vertex.projectedPosition.w - margin;
				const float              wPlusMargin      = vertex.projectedPosition.w + margin;
				uint8_t&                 clipBitField     = vertexClipBitFields[vertexIndex];
				uint8_t&                 equalityBitField = vertexEqualityBitFields[vertexIndex];

				if (vertex.projectedPosition.x <  -wPlusMargin) { clipBitField     |= leftBitMask;                     }
				if (vertex.projectedPosition.x >   wPlusMargin) { clipBitField     |= rightBitMask;                    }
				if (vertex.projectedPosition.y <  -wPlusMargin) { clipBitField     |= bottomBitMask;                   }
				if (vertex.projectedPosition.y >   wPlusMargin) { clipBitField     |= topBitMask;                      }
				if (vertex.projectedPosition.z <  -wPlusMargin) { clipBitField     |= nearBitMask;                     }
				if (vertex.projectedPosition.z >   wPlusMargin) { clipBitField     |= farBitMask;                      }

				if (vertex.projectedPosition.x <= -wLessMargin) { equalityBitField |= (~clipBitField) & leftBitMask;   }
				if (vertex.projectedPosition.x >=  wLessMargin) { equalityBitField |= (~clipBitField) & rightBitMask;  }
				if (vertex.projectedPosition.y <= -wLessMargin) { equalityBitField |= (~clipBitField) & bottomBitMask; }
				if (vertex.projectedPosition.y >=  wLessMargin) { equalityBitField |= (~clipBitField) & topBitMask;    }
				if (vertex.projectedPosition.z <= -wLessMargin) { equalityBitField |= (~clipBitField) & nearBitMask;   }
				if (vertex.projectedPosition.z >=  wLessMargin) { equalityBitField |= (~clipBitField) & farBitMask;    }
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
					const TransformedVertex firstVertex    = vertices[edge.firstVertexIndex];
					const TransformedVertex secondVertex   = vertices[edge.secondVertexIndex];
					const TransformedVertex oppositeVertex = vertices[edge.oppositeVertexIndex];
					const uint8_t           combinedField  = (vertexClipBitFields[edge.firstVertexIndex] ^ vertexClipBitFields[edge.secondVertexIndex]) & ~(vertexEqualityBitFields[edge.firstVertexIndex] | vertexEqualityBitFields[edge.secondVertexIndex]);

					if (combinedField)
					{
						TransformedVertex intersection;

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

		static void pixelShift(std::array<TransformedVertex, 3>& vertices)
		{
			for (TransformedVertex& vertex : vertices)
			{
				vertex.projectedPosition.x -= 0.5f;
				vertex.projectedPosition.y -= 0.5f;
			}
		}

		void perspectiveDivide(std::array<TransformedVertex, 3>& vertices) const
		{
			for (TransformedVertex& vertex : vertices)
			{		
				if (m_textureMode == TextureMode::Perspective)
				{
					vertex.textureCoord  /= vertex.projectedPosition.w;
				}

				vertex.inverseW           = 1.0f / vertex.projectedPosition.w;
				vertex.worldPosition     /= vertex.projectedPosition.w;
				vertex.normal            /= vertex.projectedPosition.w;
				vertex.projectedPosition /= vertex.projectedPosition.w;
			}
		}

		static void viewportTransformation(std::array<TransformedVertex,3>& vertices, const float halfWidth, const float halfHeight)
		{
			for (TransformedVertex& vertex : vertices)
			{
				vertex.projectedPosition.x = vertex.projectedPosition.x * halfWidth + halfWidth;
				vertex.projectedPosition.y = halfHeight - vertex.projectedPosition.y * halfHeight;
			}
		}

		static void sortVertices(std::array<TransformedVertex,3>& vertices)
		{
			for (uint8_t iteration = 0; iteration < 2; ++iteration)
			{
				for (size_t vertexIndex = 0; vertexIndex < 2; ++vertexIndex)
				{
					if (vertices[vertexIndex].projectedPosition.y > vertices[vertexIndex+1].projectedPosition.y)
					{
						const TransformedVertex temp = vertices[vertexIndex];

						vertices[vertexIndex]   = vertices[vertexIndex+1];
						vertices[vertexIndex+1] = temp;
					}
				}
			}
		}

		void fillTriangle(const std::array<TransformedVertex,3>& vertices, const TShader& shader, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			const TransformedVertex topToMiddleVector    = (vertices[1] - vertices[0]).normalize();
			const TransformedVertex topToBottomVector    = (vertices[2] - vertices[0]).normalize();
			const TransformedVertex middleToBottomVector = (vertices[2] - vertices[1]).normalize();
			const bool              middleVertexLeft     =  topToMiddleVector.projectedPosition.x <= topToBottomVector.projectedPosition.x;

			fillBottomHeavyTriangle(vertices, shader, topToMiddleVector, topToBottomVector, middleVertexLeft, colorBuffer, depthBuffer);
			fillTopHeavyTriangle(   vertices, shader, topToBottomVector, middleToBottomVector, middleVertexLeft, colorBuffer, depthBuffer);
		}

		void fillBottomHeavyTriangle(const std::array<TransformedVertex,3>& vertices, const TShader& shader, const TransformedVertex& topToMiddleVector, const TransformedVertex& topToBottomVector, const bool middleVertexLeft, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			if (vertices[1].projectedPosition.y != vertices[0].projectedPosition.y)
			{
				const size_t             firstY          = size_t(std::ceil(vertices[0].projectedPosition.y));

				const float              topToFirstYDiff = float(firstY) - vertices[0].projectedPosition.y;

				const TransformedVertex& leftVector      = middleVertexLeft ? topToMiddleVector : topToBottomVector;
				const TransformedVertex& rightVector     = middleVertexLeft ? topToBottomVector : topToMiddleVector;

				const float              leftRatio       = topToFirstYDiff / leftVector.projectedPosition.y;
				const float              rightRatio      = topToFirstYDiff / rightVector.projectedPosition.y;

				const TransformedVertex  startLeft       = vertices[0] + leftVector  * leftRatio;
				const TransformedVertex  startRight      = vertices[0] + rightVector * rightRatio;

				const size_t             targetY         = size_t(std::ceil(vertices[1].projectedPosition.y));

				fillTriangle(leftVector, rightVector, firstY, targetY, startLeft, startRight, shader, colorBuffer, depthBuffer);
			}
		}

		void fillTopHeavyTriangle(const std::array<TransformedVertex,3>& vertices, const TShader& shader, const TransformedVertex& topToBottomVector, const TransformedVertex& middleToBottomVector, const bool middleVertexLeft, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			if (vertices[2].projectedPosition.y != vertices[1].projectedPosition.y)
			{
				const size_t             firstY         = size_t(std::ceil(vertices[1].projectedPosition.y));

				const float              middleToFirstY = float(firstY) - vertices[1].projectedPosition.y;
				const float              topToFirstY    = float(firstY) - vertices[0].projectedPosition.y;

				const TransformedVertex& leftVector     = middleVertexLeft ? middleToBottomVector : topToBottomVector;
				const TransformedVertex& rightVector    = middleVertexLeft ? topToBottomVector    : middleToBottomVector;

				const float              ratioLeft      = (middleVertexLeft ? middleToFirstY : topToFirstY   ) / leftVector.projectedPosition.y;
				const float              ratioRight     = (middleVertexLeft ? topToFirstY    : middleToFirstY) / rightVector.projectedPosition.y;

				const TransformedVertex  startLeft      = (middleVertexLeft ? vertices[1] : vertices[0]) + leftVector  * ratioLeft;
				const TransformedVertex  startRight     = (middleVertexLeft ? vertices[0] : vertices[1]) + rightVector * ratioRight;

				const size_t  targetY        = size_t(std::ceil(vertices[2].projectedPosition.y));

				fillTriangle(leftVector, rightVector, firstY, targetY, startLeft, startRight, shader, colorBuffer, depthBuffer);
			}
		}

		void fillTriangle(const TransformedVertex& leftVector, const TransformedVertex& rightVector, const size_t firstY, const size_t targetY, const TransformedVertex& leftStart, const TransformedVertex& rightStart, const TShader& shader, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			const TransformedVertex leftChange       = leftVector  / leftVector.projectedPosition.y;
			const TransformedVertex rightChange      = rightVector / rightVector.projectedPosition.y;
			const size_t            interlacedFirstY = firstY + ((m_interlaceOffset - (firstY % m_interlaceStep)) + m_interlaceStep) % m_interlaceStep;
			size_t                  rowCount         = interlacedFirstY - firstY;

			for (size_t currentY = interlacedFirstY; currentY < targetY; rowCount += m_interlaceStep, currentY += m_interlaceStep)
			{
				const TransformedVertex currentLeft       = leftStart + leftChange * float(rowCount);
				const TransformedVertex currentRight      = rightStart + rightChange * float(rowCount);
				const TransformedVertex leftToRightVector = (currentRight - currentLeft) / (currentRight.projectedPosition.x - currentLeft.projectedPosition.x);
				const size_t            firstX            = size_t(std::ceil(currentLeft.projectedPosition.x));
				const size_t            lastX             = size_t(std::ceil(currentRight.projectedPosition.x));
				const float             leftToFirstX      = float(firstX) - currentLeft.projectedPosition.x;
				TransformedVertex       pixel             = currentLeft + leftToRightVector * leftToFirstX;
				Color*                  colorPointer      = colorBuffer.getData() + (currentY * colorBuffer.getWidth() + firstX);
				float*                  depthPointer      = depthBuffer.getData() + (currentY * depthBuffer.getWidth() + firstX);

				for (size_t x = firstX; x < lastX; ++x, ++colorPointer, ++depthPointer)
				{
					if (!m_depthTest || pixel.projectedPosition.z + m_depthBias < *depthPointer)
					{
						Vector2 textureCoord = pixel.textureCoord;

						if (m_textureMode == TextureMode::Perspective)
							textureCoord /= pixel.inverseW;

						shader.draw(pixel.projectedPosition, pixel.worldPosition / pixel.inverseW, pixel.normal / pixel.inverseW, textureCoord, *colorPointer, *depthPointer);
					}

					pixel += leftToRightVector;
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
		float        m_depthBias;
	};
}