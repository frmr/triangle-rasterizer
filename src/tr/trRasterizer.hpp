#pragma once

#include "trAxis.hpp"
#include "trColorBuffer.hpp"
#include "trCoord.hpp"
#include "trDepthBuffer.hpp"
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
		void           draw(std::vector<Vertex> vertices, const ColorBuffer& texture, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer);
		void           setPrimitive(const Primitive primitive);
		void           setMatrix(const Matrix4& matrix);

	private:
		static Vertex  lineFrustumIntersection(const Vertex& lineStart, const Vertex& lineEnd, const tr::Axis axis, const bool negativeW);
		static void    drawTriangle(std::array<Vertex, 3> vertices, const ColorBuffer& texture, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer);
		static void    clipAndDrawTriangle(const std::array<Vertex, 3>& vertices, const ColorBuffer& texture, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer);
		static float   interpolate(const float weight0, const float value0, const float weight1, const float value1, const float weight2, const float value2);
	
		template<typename T>
		static float   orientPoint(const Vector4& lineStart, const Vector4& lineEnd, const T& point);

	private:
		Primitive      m_primitive;
		Matrix4        m_matrix;
	};
}