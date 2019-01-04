#pragma once

#include "trAxis.hpp"
#include "trColorBuffer.hpp"
#include "trCoord.hpp"
#include "trDepthBuffer.hpp"
#include "trPrimitive.hpp"
#include "trTriangle.hpp"
#include "trVertex.hpp"
#include "../matrix/Matrices.h"

#include <vector>

namespace tr
{
	class Rasterizer
	{
	public:
		void           draw(std::vector<Vertex> vertices, const ColorBuffer& texture, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer);
		void           setPrimitive(const Primitive primitive);
		void           setMatrix(const Matrix4& matrix);

	private:
		static void    clipTriangles(std::vector<Triangle>& triangles);
		static Vertex  lineFrustumIntersection(const Vertex& lineStart, const Vertex& lineEnd, const tr::Axis axis, const bool negativeW);

		static void    drawPoint(const int x, const int y, const float depth, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer);
		static void    drawPoint(const Vector2& point, const float depth, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer);
		static void    drawTriangle(const Triangle& triangle, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer);
	
		template<typename T>
		static float   orientPoint(const Vector4& lineStart, const Vector4& lineEnd, const T& point);

	private:
		Primitive      m_primitive;
		Matrix4        m_matrix;
	};
}