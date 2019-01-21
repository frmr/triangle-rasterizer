#pragma once

#include "trAxis.hpp"
#include "trColorBuffer.hpp"
#include "trCoord.hpp"
#include "trDepthBuffer.hpp"
#include "trDepthMode.hpp"
#include "trPrimitive.hpp"
#include "trVertex.hpp"
#include "../matrix/Matrices.h"

#include <vector>
#include <array>

namespace tr
{
	class Rasterizer
	{
	public:
		               Rasterizer();

		void           draw(std::vector<Vertex> vertices, const ColorBuffer& texture, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const;
		void           setPrimitive(const Primitive primitive);
		void           setMatrix(const Matrix4& matrix);
		void           setDepthMode(const DepthMode depthMode);

	private:
		static Vertex  lineFrustumIntersection(const Vertex& lineStart, const Vertex& lineEnd, const tr::Axis axis, const bool negativeW);
		void           drawTriangle(std::array<Vertex, 3> vertices, const ColorBuffer& texture, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const;
		void           clipAndDrawTriangle(const std::array<Vertex, 3>& vertices, const ColorBuffer& texture, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const;
		static void    pixelShift(std::array<Vertex, 3>& vertices);
		static void    perspectiveDivide(std::array<Vertex, 3>& vertices);
		static void    viewportTransformation(std::array<Vertex,3>& vertices, const float halfWidth, const float halfHeight);
		static void    sortVertices(std::array<Vertex,3>& vertices);
		void           fillTriangle(const std::array<Vertex,3>& vertices, const ColorBuffer& texture, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const;
		void           fillBottomHeavyTriangle(const std::array<Vertex,3>& vertices, const Vertex& topToMiddleVector, const Vertex& topToBottomVector, const Vertex& middleToBottomVector, const bool middleVertexLeft, const ColorBuffer& texture, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const;
		void           fillTopHeavyTriangle(const std::array<Vertex,3>& vertices, const Vertex& topToMiddleVector, const Vertex& topToBottomVector, const Vertex& middleToBottomVector, const bool middleVertexLeft, const ColorBuffer& texture, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const;
		void           fillTriangle(const Vertex& leftVector, const Vertex& rightVector, const size_t firstY, const size_t targetY, const Vertex& leftStart, const Vertex& rightStart, const ColorBuffer& texture, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer) const;

		static float   orientPoint(const Vector4& lineStart, const Vector4& lineEnd, const Vector4& point);

	private:
		Primitive      m_primitive;
		Matrix4        m_matrix;
		DepthMode      m_depthMode;
	};
}