#pragma once

#include "trAxis.hpp"
#include "trTexture.hpp"
#include "trCoord.hpp"
#include "trCullFaceMode.hpp"
#include "trDepthBuffer.hpp"
#include "trEdgeInfo.hpp"
#include "trError.hpp"
#include "trPrimitive.hpp"
#include "trTileManager.hpp"
#include "trTextureMode.hpp"
#include "trTextureWrappingMode.hpp"
#include "trVertex.hpp"
#include "trTransformedVertex.hpp"
#include "trVertexClipBitMasks.hpp"
#include "../matrix/Matrices.h"
#include "tfTextFile.hpp"
#include "trQuadTransformedVertex.hpp"
#include "trRect.hpp"
#include "trTriangle.hpp"

#include <vector>
#include <array>
#include <cassert>

namespace tr
{
	template <typename TShader>
	class Rasterizer
	{
	public:
		Rasterizer(const size_t bufferWidth, const size_t bufferHeight, const size_t tileWidth, const size_t tileHeight) :
			m_bufferWidth(bufferWidth),
			m_bufferHeight(bufferHeight),
			m_bufferHalfWidth(float(bufferWidth) / 2.0f),
			m_bufferHalfHeight(float(bufferHeight) / 2.0f),
			m_tileManager(bufferWidth, bufferHeight, tileWidth, tileHeight),
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

		Error draw(const std::vector<Vertex>& vertices, const TShader& shader, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer)
		{
			std::vector<TransformedVertex> transformedVertices;

			transformedVertices.reserve(vertices.size());

			// Maybe throw an exception in the constructor instead
			if (m_bufferWidth == 0 || m_bufferHeight == 0)
			{
				return Error::InvalidBufferSize;
			}

			if (colorBuffer.getWidth() != m_bufferWidth || depthBuffer.getWidth() != m_bufferWidth || colorBuffer.getHeight() != m_bufferHeight || depthBuffer.getHeight() != m_bufferHeight)
			{
				return Error::BufferSizeMismatch;
			}

			m_shaders.push_back(shader);

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
					clipAndQueueTriangle({ *it, *(it + 1), *(it + 2) }, m_shaders.size() - 1);
				}
			}
			else if (m_primitive == Primitive::TriangleStrip)
			{
				bool   reverse    = false;
				size_t lastIndex  = 0;
				size_t firstIndex = 1;

				for (size_t newIndex = 2; newIndex < transformedVertices.size(); ++newIndex)
				{
					clipAndQueueTriangle({ transformedVertices[reverse ? newIndex : lastIndex], transformedVertices[firstIndex], transformedVertices[reverse ? lastIndex : newIndex] }, m_shaders.size() - 1);

					firstIndex = lastIndex;
					lastIndex  = newIndex;
					reverse    = !reverse;
				}
			}
			else if (m_primitive == Primitive::TriangleFan)
			{
				for (std::vector<TransformedVertex>::const_iterator it = transformedVertices.begin() + 1; it < transformedVertices.end() - 1; it += 1)
				{
					clipAndQueueTriangle({ transformedVertices.front(), *it, *(it + 1) }, m_shaders.size() - 1);
				}
			}

			drawTriangles(colorBuffer, depthBuffer);
			m_triangles.clear();

			return Error::Success;
		}

		void setTilerAttributes(const size_t bufferWidth, const size_t bufferHeight, const size_t tileWidth, const size_t tileHeight)
		{
			m_tileManager.setAttributes(bufferWidth, bufferHeight, tileWidth, tileHeight);

			m_bufferWidth = bufferWidth;
			m_bufferHeight = bufferHeight;
			m_bufferHalfWidth = float(bufferWidth) / 2.0f;
			m_bufferHalfHeight = float(bufferHeight) / 2.0f;
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

		void queueTriangle(std::array<TransformedVertex, 3> vertices, const size_t shaderIndex)
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
			
			viewportTransformation(vertices);
			pixelShift(vertices);

			m_triangles.emplace_back(vertices, shaderIndex);
		}

		void clipAndQueueTriangle(const std::array<TransformedVertex, 3>& vertices, const size_t shaderIndex)
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
				queueTriangle(vertices, shaderIndex);
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

						clipAndQueueTriangle({ firstVertex,  intersection,   oppositeVertex }, shaderIndex);
						clipAndQueueTriangle({ secondVertex, oppositeVertex, intersection   }, shaderIndex);

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

		void viewportTransformation(std::array<TransformedVertex,3>& vertices) const
		{
			for (TransformedVertex& vertex : vertices)
			{
				vertex.projectedPosition.x = vertex.projectedPosition.x * m_bufferHalfWidth + m_bufferHalfWidth;
				vertex.projectedPosition.y = m_bufferHalfHeight - vertex.projectedPosition.y * m_bufferHalfHeight;
			}
		}

		void drawTriangles(ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const
		{
			assert(depthBuffer.getWidth()  == colorBuffer.getWidth());
			assert(depthBuffer.getHeight() == colorBuffer.getHeight());

			for (const Triangle& triangle : m_triangles)
			{
				const TShader& shader = m_shaders[triangle.shaderIndex];

				const size_t bufferStepX = 4;
				const size_t bufferStepY = depthBuffer.getWidth() - (triangle.boundingBox.getMaxX() - triangle.boundingBox.getMinX()) + (triangle.boundingBox.getMaxX() - triangle.boundingBox.getMinX()) % bufferStepX - bufferStepX;

				Color* colorPointer = colorBuffer.getData() + triangle.boundingBox.getMinY() * colorBuffer.getWidth() + triangle.boundingBox.getMinX();
				float* depthPointer = depthBuffer.getData() + triangle.boundingBox.getMinY() * depthBuffer.getWidth() + triangle.boundingBox.getMinX();

				QuadFloat rowWeights0 = triangle.rowWeights0;
				QuadFloat rowWeights1 = triangle.rowWeights1;
				QuadFloat rowWeights2 = triangle.rowWeights2;

				for (size_t y = triangle.boundingBox.getMinY(); y <= triangle.boundingBox.getMaxY(); y += 1, colorPointer += bufferStepY, depthPointer += bufferStepY)
				{
					QuadFloat weights0 = rowWeights0;
					QuadFloat weights1 = rowWeights1;
					QuadFloat weights2 = rowWeights2;

					for (size_t x = triangle.boundingBox.getMinX(); x <= triangle.boundingBox.getMaxX(); x += 4, colorPointer += bufferStepX, depthPointer += bufferStepX)
					{
						const QuadMask positiveWeightsMask = ~(weights0 | weights1 | weights2).castToMask();
						const QuadMask negativeWeightsMask =  (weights0 & weights1 & weights2).castToMask();

						QuadMask renderMask = positiveWeightsMask | negativeWeightsMask;

						if (renderMask.moveMask())
						{
							const QuadFloat normalizedWeights0 = (weights0 / triangle.quadArea).abs();
							const QuadFloat normalizedWeights1 = (weights1 / triangle.quadArea).abs();
							const QuadFloat normalizedWeights2 = (weights2 / triangle.quadArea).abs();

							QuadTransformedVertex attributes = triangle.quadVertex0 * normalizedWeights0 + triangle.quadVertex1 * normalizedWeights1 + triangle.quadVertex2 * normalizedWeights2;

							if (m_depthTest)
							{
								renderMask &= QuadFloat(depthPointer, renderMask).greaterThan(attributes.projectedPosition.z + m_depthBias);
							}

							attributes.textureCoord /= attributes.inverseW;

							shader.draw(renderMask, attributes.projectedPosition, attributes.worldPosition, attributes.normal, attributes.textureCoord, colorPointer, depthPointer);
						}

						weights0 += triangle.quadA12;
						weights1 += triangle.quadA20;
						weights2 += triangle.quadA01;
					}

					rowWeights0 += triangle.quadB12;
					rowWeights1 += triangle.quadB20;
					rowWeights2 += triangle.quadB01;
				}
			}
		}

		static float orientPoint(const Vector4& lineStart, const Vector4& lineEnd, const Vector4& point)
		{
			return (lineEnd.x - lineStart.x) * (point.y - lineStart.y) - (lineEnd.y - lineStart.y) * (point.x - lineStart.x);
		}

	private:
		size_t                m_bufferWidth;
		size_t                m_bufferHeight;
		float                 m_bufferHalfWidth;
		float                 m_bufferHalfHeight;
		TileManager           m_tileManager;
		Primitive             m_primitive;
		Matrix4               m_projectionViewMatrix;
		Matrix4               m_modelMatrix;
		Matrix3               m_modelNormalRotationMatrix;
		bool                  m_depthTest;
		TextureMode           m_textureMode;
		CullFaceMode          m_cullFaceMode;
		QuadFloat             m_depthBias;
		std::vector<TShader>  m_shaders;
		std::vector<Triangle> m_triangles;
	};
}