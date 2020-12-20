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

#include <array>

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
			m_cullFaceMode(CullFaceMode::Back),
			m_textureMode(TextureMode::Perspective),
			m_depthTest(true),
			m_depthBias(0.0f)
		{
		}

		void queue(const std::vector<Vertex>& vertices, const TShader& shader)
		{
			std::vector<TransformedVertex> transformedVertices;

			transformedVertices.reserve(vertices.size());

			const size_t shaderIndex              = m_tileManager.storeShader(shader);
			const size_t rasterizationParamsIndex = m_tileManager.storeRasterizationParams(m_depthTest, m_depthBias, m_textureMode);

			for (const Vertex& vertex : vertices)
			{
				const Vector4 worldPosition = m_modelMatrix * vertex.position;

				transformedVertices.emplace_back(
					Vector3(worldPosition.x, worldPosition.y, worldPosition.z),
					m_projectionViewMatrix * m_modelMatrix * vertex.position,
					m_modelNormalRotationMatrix * vertex.normal,
					vertex.textureCoord
				);
			}

			if (m_primitive == Primitive::Triangles)
			{
				for (std::vector<TransformedVertex>::const_iterator it = transformedVertices.begin(); it < transformedVertices.end() - 2; it += 3)
				{
					clipAndQueueTriangle({ *it, *(it + 1), *(it + 2) }, shaderIndex, rasterizationParamsIndex);
				}
			}
			else if (m_primitive == Primitive::TriangleStrip)
			{
				bool   reverse    = false;
				size_t lastIndex  = 0;
				size_t firstIndex = 1;

				for (size_t newIndex = 2; newIndex < transformedVertices.size(); ++newIndex)
				{
					clipAndQueueTriangle({ transformedVertices[reverse ? newIndex : lastIndex], transformedVertices[firstIndex], transformedVertices[reverse ? lastIndex : newIndex] }, shaderIndex, rasterizationParamsIndex);

					firstIndex = lastIndex;
					lastIndex  = newIndex;
					reverse    = !reverse;
				}
			}
			else if (m_primitive == Primitive::TriangleFan)
			{
				for (std::vector<TransformedVertex>::const_iterator it = transformedVertices.begin() + 1; it < transformedVertices.end() - 1; it += 1)
				{
					clipAndQueueTriangle({ transformedVertices.front(), *it, *(it + 1) }, shaderIndex, rasterizationParamsIndex);
				}
			}
		}

		Error draw(const size_t numThreads, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer)
		{
			// Maybe throw an exception in the constructor instead
			if (m_bufferWidth == 0 || m_bufferHeight == 0)
			{
				return Error::InvalidBufferSize;
			}

			if (colorBuffer.getWidth() != m_bufferWidth || depthBuffer.getWidth() != m_bufferWidth || colorBuffer.getHeight() != m_bufferHeight || depthBuffer.getHeight() != m_bufferHeight)
			{
				return Error::BufferSizeMismatch;
			}

			m_tileManager.draw(numThreads, colorBuffer, depthBuffer);

			return Error::Success;
		}

		void clear()
		{
			m_tileManager.clear();
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

		void queueTriangle(std::array<TransformedVertex, 3>&& vertices, const size_t shaderIndex, const size_t rasterizationParamsIndex)
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

			m_tileManager.queue(Triangle(std::move(vertices), shaderIndex, rasterizationParamsIndex));
		}

		void clipAndQueueTriangle(std::array<TransformedVertex, 3>&& vertices, const size_t shaderIndex, const size_t rasterizationParamsIndex)
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
				queueTriangle(std::move(vertices), shaderIndex, rasterizationParamsIndex);
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

						clipAndQueueTriangle({ firstVertex,  intersection,   oppositeVertex }, shaderIndex, rasterizationParamsIndex);
						clipAndQueueTriangle({ secondVertex, oppositeVertex, intersection   }, shaderIndex, rasterizationParamsIndex);

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

				vertex.normal.normalize();
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

		static float orientPoint(const Vector4& lineStart, const Vector4& lineEnd, const Vector4& point)
		{
			return (lineEnd.x - lineStart.x) * (point.y - lineStart.y) - (lineEnd.y - lineStart.y) * (point.x - lineStart.x);
		}

	private:
		size_t                m_bufferWidth;
		size_t                m_bufferHeight;
		float                 m_bufferHalfWidth;
		float                 m_bufferHalfHeight;
		TileManager<TShader>  m_tileManager;
		Primitive             m_primitive;
		Matrix4               m_projectionViewMatrix;
		Matrix4               m_modelMatrix;
		Matrix3               m_modelNormalRotationMatrix;
		CullFaceMode          m_cullFaceMode;
		TextureMode           m_textureMode;
		bool                  m_depthTest;
		float                 m_depthBias;
	};
}