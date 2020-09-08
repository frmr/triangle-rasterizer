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
			const float a01 = vertices[0].projectedPosition.y - vertices[1].projectedPosition.y;
			const float b01 = vertices[1].projectedPosition.x - vertices[0].projectedPosition.x;
			const float a12 = vertices[1].projectedPosition.y - vertices[2].projectedPosition.y;
			const float b12 = vertices[2].projectedPosition.x - vertices[1].projectedPosition.x;
			const float a20 = vertices[2].projectedPosition.y - vertices[0].projectedPosition.y;
			const float b20 = vertices[0].projectedPosition.x - vertices[2].projectedPosition.x;
			
			const size_t minX = size_t(std::min(vertices[0].projectedPosition.x, std::min(vertices[1].projectedPosition.x, vertices[2].projectedPosition.x)) + 0.5f);
			const size_t minY = size_t(std::min(vertices[0].projectedPosition.y, std::min(vertices[1].projectedPosition.y, vertices[2].projectedPosition.y)) + 0.5f);
			const size_t maxX = size_t(std::max(vertices[0].projectedPosition.x, std::max(vertices[1].projectedPosition.x, vertices[2].projectedPosition.x)) + 0.5f);
			const size_t maxY = size_t(std::max(vertices[0].projectedPosition.y, std::max(vertices[1].projectedPosition.y, vertices[2].projectedPosition.y)) + 0.5f);
			
			Vector4 point(float(minX), float(minY), 0.0f, 0.0f);
			
			const float area = orientPoint(vertices[0].projectedPosition, vertices[1].projectedPosition, vertices[2].projectedPosition);

			float wRow0 = orientPoint(vertices[1].projectedPosition, vertices[2].projectedPosition, point);
			float wRow1 = orientPoint(vertices[2].projectedPosition, vertices[0].projectedPosition, point);
			float wRow2 = orientPoint(vertices[0].projectedPosition, vertices[1].projectedPosition, point);
			
			for (size_t y = minY; y <= maxY; ++y)
			{
				Color* colorPointer = colorBuffer.getData() + (y * colorBuffer.getWidth() + minX);
				float* depthPointer = depthBuffer.getData() + (y * depthBuffer.getWidth() + minX);

				float w0 = wRow0;
				float w1 = wRow1;
				float w2 = wRow2;
			
				for (size_t x = minX; x <= maxX; ++x, ++colorPointer, ++depthPointer)
				{
					if ((w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) || (w0 <= 0.0f && w1 <= 0.0f && w2 <= 0.0f))
					{
						float weight0 = std::abs(w0 / area);
						float weight1 = std::abs(w1 / area);
						float weight2 = std::abs(w2 / area);

						const TransformedVertex attributes = vertices[0] * weight0 + vertices[1] * weight1 + vertices[2] * weight2;

						if (!m_depthTest || attributes.projectedPosition.z + m_depthBias < *depthPointer)
						{
							Vector2 textureCoord = attributes.textureCoord;
						
							if (m_textureMode == TextureMode::Perspective)
								textureCoord /= attributes.inverseW;
						
							shader.draw(attributes.projectedPosition, attributes.worldPosition / attributes.inverseW, attributes.normal / attributes.inverseW, textureCoord, *colorPointer, *depthPointer);
						}
					}
			
					w0 += a12;
					w1 += a20;
					w2 += a01;
				}
			
				wRow0 += b12;
				wRow1 += b20;
				wRow2 += b01;
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