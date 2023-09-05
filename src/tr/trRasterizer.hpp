#pragma once

#include "trAxis.hpp"
#include "trTexture.hpp"
#include "trCoord.hpp"
#include "trCullFaceMode.hpp"
#include "trDepthBuffer.hpp"
#include "trEdgeInfo.hpp"
#include "trPrimitive.hpp"
#include "trTileManager.hpp"
#include "trTextureMode.hpp"
#include "trTextureWrappingMode.hpp"
#include "trVertex.hpp"
#include "trTransformedVertex.hpp"
#include "trVertexClipBitMasks.hpp"
#include "../matrix/Matrices.h"
#include "trQuadTransformedVertex.hpp"
#include "trRect.hpp"

#include <array>

namespace tr
{
	template <typename TShader>
	class Rasterizer
	{
	public:
		Rasterizer(const uint32_t bufferWidth, const uint32_t bufferHeight, const uint32_t tileWidth, const uint32_t tileHeight) :
			m_bufferHalfWidth(float(bufferWidth) / 2.0f),
			m_bufferHalfHeight(float(bufferHeight) / 2.0f),
			m_tileManager(bufferWidth, bufferHeight, tileWidth, tileHeight),
			m_primitive(Primitive::Triangles),
			m_projectionMatrix(),
			m_viewMatrix(),
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
			std::vector<TransformedVertex> transformed;

			transformed.reserve(vertices.size());

			const size_t shaderIndex              = m_tileManager.storeShader(shader);
			const size_t rasterizationParamsIndex = m_tileManager.storeRasterizationParams(m_depthTest, m_depthBias, m_textureMode);

			for (const Vertex& vertex : vertices)
			{
				const Vector4 worldPosition = m_modelMatrix * vertex.position;

				transformed.emplace_back(
					Vector3(worldPosition.x, worldPosition.y, worldPosition.z),
					m_projectionMatrix * m_viewMatrix * m_modelMatrix * vertex.position,
					m_modelNormalRotationMatrix * vertex.normal,
					vertex.textureCoord
				);
			}

			if (m_primitive == Primitive::Triangles)
			{
				for (size_t i = 0; i + 2 < transformed.size(); i += 3)
				{
					clipAndQueueTriangle({ transformed[i], transformed[i + 1], transformed[i + 2] }, shaderIndex, rasterizationParamsIndex);
				}
			}
			else if (m_primitive == Primitive::TriangleStrip)
			{
				bool   reverse    = false;
				size_t lastIndex  = 0;
				size_t firstIndex = 1;

				for (size_t newIndex = 2; newIndex < transformed.size(); ++newIndex)
				{
					clipAndQueueTriangle({ transformed[reverse ? newIndex : lastIndex], transformed[firstIndex], transformed[reverse ? lastIndex : newIndex] }, shaderIndex, rasterizationParamsIndex);

					firstIndex = lastIndex;
					lastIndex  = newIndex;
					reverse    = !reverse;
				}
			}
			else if (m_primitive == Primitive::TriangleFan)
			{
				for (std::vector<TransformedVertex>::const_iterator it = transformed.begin() + 1; it + 1 < transformed.end(); it += 1)
				{
					clipAndQueueTriangle({ transformed.front(), *it, *(it + 1) }, shaderIndex, rasterizationParamsIndex);
				}
			}
		}

		void draw(const size_t numThreads, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer)
		{
			m_tileManager.draw(numThreads, colorBuffer, depthBuffer);
		}

		void clear()
		{
			m_tileManager.clear();
		}

		void setTilerAttributes(const uint32_t bufferWidth, const uint32_t bufferHeight, const uint32_t tileWidth, const uint32_t tileHeight)
		{
			m_tileManager.setAttributes(bufferWidth, bufferHeight, tileWidth, tileHeight);

			m_bufferHalfWidth  = float(bufferWidth)  / 2.0f;
			m_bufferHalfHeight = float(bufferHeight) / 2.0f;
		}

		void setPrimitive(const Primitive primitive)
		{
			m_primitive = primitive;
		}

		void setProjectionMatrix(const Matrix4& matrix)
		{
			m_projectionMatrix = matrix;
		}

		void setViewMatrix(const Matrix4& matrix)
		{
			m_viewMatrix = matrix;
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
			const Vector4   pos0 = vertices[0].projectedPosition;
			const Vector4   pos1 = vertices[1].projectedPosition;
			const Vector4   pos2 = vertices[2].projectedPosition;

			// Ignore degenerate triangles
			if (pos0 == pos1 || pos1 == pos2 || pos2 == pos0)
			{
				return;
			}

			if (pos0.x < -pos0.w &&
				pos1.x < -pos1.w &&
				pos2.x < -pos2.w)
			{
				return;
			}

			if (pos0.x > pos0.w &&
				pos1.x > pos1.w &&
				pos2.x > pos2.w)
			{
				return;
			}

			if (pos0.y < -pos0.w &&
				pos1.y < -pos1.w &&
				pos2.y < -pos2.w)
			{
				return;
			}

			if (pos0.y > pos0.w &&
				pos1.y > pos1.w &&
				pos2.y > pos2.w)
			{
				return;
			}
			
			if (pos0.z < -pos0.w &&
				pos1.z < -pos1.w &&
				pos2.z < -pos2.w)
			{
				return;
			}

			if (pos0.z > pos0.w &&
				pos1.z > pos1.w &&
				pos2.z > pos2.w)
			{
				return;
			}
			
			// Clip against near plane

			constexpr VertexOrder orders[3] = { { 0, 1, 2 }, { 1, 2, 0 }, { 2, 0, 1 } };
			std::array<bool, 3> clipVertices { false, false, false };
			uint8_t clipCount = 0;

			for (size_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
			{
				const TransformedVertex& vertex = vertices[vertexIndex];

				if (vertex.projectedPosition.z < -vertex.projectedPosition.w)
				{
					clipVertices[vertexIndex] = true;
					++clipCount;
				}
			}

			assert(clipCount != 3);

			if (clipCount == 0)
			{
				queueTriangle(std::move(vertices), shaderIndex, rasterizationParamsIndex);
			}
			else if (clipCount == 1)
			{
				size_t orderIndex = 0;

				if (clipVertices[1])
				{
					orderIndex = 1;
				}
				else if (clipVertices[2])
				{
					orderIndex = 2;
				}

				const VertexOrder& order = orders[orderIndex];

				const TransformedVertex& v0 = vertices[order.firstVertexIndex];
				const TransformedVertex& v1 = vertices[order.secondVertexIndex];
				const TransformedVertex& v2 = vertices[order.oppositeVertexIndex];

				const TransformedVertex intersection0 = lineFrustumIntersection(v0, v1, Axis::Z, true);
				const TransformedVertex intersection1 = lineFrustumIntersection(v2, v0, Axis::Z, true);

				queueTriangle({ v1, v2, intersection1 }, shaderIndex, rasterizationParamsIndex);
				queueTriangle({ intersection1, intersection0, v1}, shaderIndex, rasterizationParamsIndex);
			}
			else if (clipCount == 2)
			{
				size_t orderIndex = 0;

				if (!clipVertices[1])
				{
					orderIndex = 1;
				}
				else if (!clipVertices[2])
				{
					orderIndex = 2;
				}

				const VertexOrder& order = orders[orderIndex];

				const TransformedVertex& v0 = vertices[order.firstVertexIndex];
				const TransformedVertex& v1 = vertices[order.secondVertexIndex];
				const TransformedVertex& v2 = vertices[order.oppositeVertexIndex];

				const TransformedVertex intersection0 = lineFrustumIntersection(v0, v1, Axis::Z, true);
				const TransformedVertex intersection1 = lineFrustumIntersection(v2, v0, Axis::Z, true);

				queueTriangle({ v0, intersection0, intersection1 }, shaderIndex, rasterizationParamsIndex);
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
		float                 m_bufferHalfWidth;
		float                 m_bufferHalfHeight;
		TileManager<TShader>  m_tileManager;
		Primitive             m_primitive;
		Matrix4               m_projectionMatrix;
		Matrix4               m_viewMatrix;
		Matrix4               m_modelMatrix;
		Matrix3               m_modelNormalRotationMatrix;
		CullFaceMode          m_cullFaceMode;
		TextureMode           m_textureMode;
		bool                  m_depthTest;
		float                 m_depthBias;
	};
}