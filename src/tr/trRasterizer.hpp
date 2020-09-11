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
#include "tfTextFile.hpp"
#include "trQuadTransformedVertex.hpp"

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

		void setDepthBias(const float depthBias)
		{
			m_depthBias = depthBias;
		}

	private:
		static TransformedVertex lineFrustumIntersection(const TransformedVertex& lineStart, const TransformedVertex& lineEnd, const tr::Axis axis, const bool negativeW)
		{
			const float   scalar = negativeW ?
	                               (-lineStart.projectedPosition.w  - lineStart.projectedPosition[axis]) / (lineEnd.projectedPosition[axis] - lineStart.projectedPosition[axis] + lineEnd.projectedPosition.w - lineStart.projectedPosition.w) :
	                               ( lineStart.projectedPosition.w  - lineStart.projectedPosition[axis]) / (lineEnd.projectedPosition[axis] - lineStart.projectedPosition[axis] - lineEnd.projectedPosition.w + lineStart.projectedPosition.w);

			const Vector3 worldPosition	    = lineStart.worldPosition     + (lineEnd.worldPosition     - lineStart.worldPosition)     * scalar;
			const Vector4 projectedPosition = lineStart.projectedPosition + (lineEnd.projectedPosition - lineStart.projectedPosition) * scalar;
			const Vector3 normal            = lineStart.normal            + (lineEnd.normal            - lineStart.normal)            * scalar;
			const Vector2 textureCoord      = lineStart.textureCoord      + (lineEnd.textureCoord      - lineStart.textureCoord)      * scalar;
	
			return TransformedVertex(worldPosition, projectedPosition, normal, textureCoord);
		}

		void drawTriangle(std::array<TransformedVertex, 3> vertices, const TShader& shader, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			perspectiveDivide(vertices);

			if (m_cullFaceMode != CullFaceMode::None)
			{
				const float pointValue = orientPoint(vertices[0].projectedPosition, vertices[1].projectedPosition, vertices[2].projectedPosition);

				if ((m_cullFaceMode == CullFaceMode::Back  && pointValue >= 0.0f) ||
					(m_cullFaceMode == CullFaceMode::Front && pointValue <  0.0f))
				{
					return;
				}
			}
			
			viewportTransformation(vertices, halfWidth, halfHeight);
			pixelShift(vertices);
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

		void fillTriangle(const std::array<TransformedVertex,3>& vertices, const TShader& shader, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			const QuadFloat quadA01(4.0f * (vertices[0].projectedPosition.y - vertices[1].projectedPosition.y));
			const QuadFloat quadB01(1.0f * (vertices[1].projectedPosition.x - vertices[0].projectedPosition.x));
			const QuadFloat quadA12(4.0f * (vertices[1].projectedPosition.y - vertices[2].projectedPosition.y));
			const QuadFloat quadB12(1.0f * (vertices[2].projectedPosition.x - vertices[1].projectedPosition.x));
			const QuadFloat quadA20(4.0f * (vertices[2].projectedPosition.y - vertices[0].projectedPosition.y));
			const QuadFloat quadB20(1.0f * (vertices[0].projectedPosition.x - vertices[2].projectedPosition.x));

			constexpr size_t quadAlignmentMask = std::numeric_limits<size_t>::max() ^ 0x03;

			const size_t  minX = size_t(std::min({ vertices[0].projectedPosition.x, vertices[1].projectedPosition.x, vertices[2].projectedPosition.x })) & quadAlignmentMask;
			const size_t  minY = size_t(std::min({ vertices[0].projectedPosition.y, vertices[1].projectedPosition.y, vertices[2].projectedPosition.y }));
			const size_t  maxX = size_t(std::max({ vertices[0].projectedPosition.x, vertices[1].projectedPosition.x, vertices[2].projectedPosition.x }));
			const size_t  maxY = size_t(std::max({ vertices[0].projectedPosition.y, vertices[1].projectedPosition.y, vertices[2].projectedPosition.y }));

			const QuadVec3 points(
				QuadFloat(float(minX), float(minX + 1), float(minX + 2),     float(minX + 3)),
				QuadFloat(float(minY), float(minY),     float(minY),         float(minY)),
				0.0f
			);

			QuadTransformedVertex quadVertex0(vertices[0]);
			QuadTransformedVertex quadVertex1(vertices[1]);
			QuadTransformedVertex quadVertex2(vertices[2]);

			QuadFloat rowWeights0 = orientPoints(quadVertex1.projectedPosition, quadVertex2.projectedPosition, points);
			QuadFloat rowWeights1 = orientPoints(quadVertex2.projectedPosition, quadVertex0.projectedPosition, points);
			QuadFloat rowWeights2 = orientPoints(quadVertex0.projectedPosition, quadVertex1.projectedPosition, points);

			const QuadFloat quadArea = orientPoints(quadVertex0.projectedPosition, quadVertex1.projectedPosition, quadVertex2.projectedPosition);

			assert(depthBuffer.getWidth()  == colorBuffer.getWidth());
			assert(depthBuffer.getHeight() == colorBuffer.getHeight());

			const QuadFloat quadZero(0.0f);

			const size_t bufferStepX = 4;
			const size_t bufferStepY = depthBuffer.getWidth() - (maxX - minX) + (maxX - minX) % bufferStepX - bufferStepX;

			Color* colorPointer = colorBuffer.getData() + minY * colorBuffer.getWidth() + minX;
			float* depthPointer = depthBuffer.getData() + minY * depthBuffer.getWidth() + minX;

			for (size_t y = minY; y <= maxY; y += 1, colorPointer += bufferStepY, depthPointer += bufferStepY)
			{
				QuadFloat weights0 = rowWeights0;
				QuadFloat weights1 = rowWeights1;
				QuadFloat weights2 = rowWeights2;

				for (size_t x = minX; x <= maxX; x += 4, colorPointer += bufferStepX, depthPointer += bufferStepX)
				{
					const QuadMask positiveWeightsMask = weights0.greaterThan(quadZero) & weights1.greaterThan(quadZero) & weights2.greaterThan(quadZero);
					const QuadMask negativeWeightsMask = weights0.lessThan(quadZero)    & weights1.lessThan(quadZero)    & weights2.lessThan(quadZero);
					
					QuadMask       renderMask          = positiveWeightsMask | negativeWeightsMask;

					if (renderMask.moveMask())
					{
						const QuadFloat normalizedWeights0 = (weights0 / quadArea).abs();
						const QuadFloat normalizedWeights1 = (weights1 / quadArea).abs();
						const QuadFloat normalizedWeights2 = (weights2 / quadArea).abs();

						QuadTransformedVertex attributes = quadVertex0 * normalizedWeights0 + quadVertex1 * normalizedWeights1 + quadVertex2 * normalizedWeights2;

						if (m_depthTest)
						{
							renderMask &= QuadFloat(depthPointer).greaterThan(attributes.projectedPosition.z + m_depthBias);
						}

						attributes.textureCoord /= attributes.inverseW;

						shader.draw(renderMask, attributes.projectedPosition, attributes.worldPosition, attributes.normal, attributes.textureCoord, colorPointer, depthPointer);
					}

					weights0 += quadA12;
					weights1 += quadA20;
					weights2 += quadA01;
				}

				rowWeights0 += quadB12;
				rowWeights1 += quadB20;
				rowWeights2 += quadB01;
			}
		}

		static float orientPoint(const Vector4& lineStart, const Vector4& lineEnd, const Vector4& point)
		{
			return (lineEnd.x - lineStart.x) * (point.y - lineStart.y) - (lineEnd.y - lineStart.y) * (point.x - lineStart.x);
		}

		static QuadFloat orientPoints(const QuadVec3& lineStarts, const QuadVec3& lineEnds, const QuadVec3& points)
		{
			return (lineEnds.x - lineStarts.x) * (points.y - lineStarts.y) - (lineEnds.y - lineStarts.y) * (points.x - lineStarts.x);
		}

	private:
		Primitive    m_primitive;
		Matrix4      m_projectionViewMatrix;
		Matrix4      m_modelMatrix;
		Matrix3      m_modelNormalRotationMatrix;
		bool         m_depthTest;
		TextureMode  m_textureMode;
		CullFaceMode m_cullFaceMode;
		QuadFloat    m_depthBias;
	};
}