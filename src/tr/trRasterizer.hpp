#pragma once

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
		static void    getLinePixels(const Coord& start, const Coord& end, std::vector<Coord>& pixels);
		static void    clipTriangles(std::vector<Triangle>& triangles);

		static Vertex  lineFrustumIntersection(const Vertex& lineStart, const Vertex& lineEnd, const float alpha);
		static Vertex  lineFrustumIntersectionRight(const Vertex& lineStart, const Vertex& lineEnd);
		static Vertex  lineFrustumIntersectionLeft(const Vertex& lineStart, const Vertex& lineEnd);
		static Vertex  lineFrustumIntersectionBottom(const Vertex& lineStart, const Vertex& lineEnd);
		static Vertex  lineFrustumIntersectionTop(const Vertex& lineStart, const Vertex& lineEnd);
		static Vertex  lineFrustumIntersectionFar(const Vertex& lineStart, const Vertex& lineEnd);
		static Vertex  lineFrustumIntersectionNear(const Vertex& lineStart, const Vertex& lineEnd);

		static void    drawPoint(const Coord& position, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer);
		static void    drawPoint(const Vector4& position, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer);
		static void    drawTriangle(const Triangle& triangle, const float halfWidth, const float halfHeight, ColorBuffer& colorBuffer, DepthBuffer& depthBuffer);
	
	private:
		Primitive      m_primitive;
		Matrix4        m_matrix;
	};
}