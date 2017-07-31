#include "trColorBuffer.hpp"
#include "trCoord.hpp"
#include "trDepthBuffer.hpp"
#include "trPrimitive.hpp"
#include "trVertex.hpp"
#include "../matrix/Matrices.h"

#include <vector>

namespace tr
{
	class Rasterizer
	{
	public:
		void draw(std::vector<Vertex> vertices, const tr::ColorBuffer& texture, const int width, const int height, tr::ColorBuffer& colorBuffer, tr::DepthBuffer& depthBuffer);
		void setPrimitive(const tr::Primitive primitive);
		void setMatrix(const Matrix4& matrix);

	private:
		void getLinePixels(const tr::Coord& start, const tr::Coord& end, std::vector<tr::Coord>& pixels);
		
		bool vertexInNdcCube(const Vector4 vertex);
		bool clipNdcLine(Vector4& start, Vector4& end);

		void ndcIntersectionX(const Vector4& inside, Vector4& outside, const float xValue);
		void ndcIntersectionY(const Vector4& inside, Vector4& outside, const float yValue);

		void drawPoint(const tr::Coord& position, tr::ColorBuffer& colorBuffer, tr::DepthBuffer& depthBuffer);

		void drawPoint(const Vector4& position, const float halfWidth, const float halfHeight, tr::ColorBuffer& colorBuffer, tr::DepthBuffer& depthBuffer);

		
		void drawLine(const tr::Coord& start, const tr::Coord& end, tr::ColorBuffer& colorBuffer, tr::DepthBuffer& depthBuffer);
		void drawLine(Vector4 v0, Vector4 v1, const float halfWidth, const float halfHeight, tr::ColorBuffer& colorBuffer, tr::DepthBuffer& depthBuffer);
		void drawTriangle(Vector4 v0, Vector4 v1, Vector4 v2, const float halfWidth, const float halfHeight, tr::ColorBuffer& colorBuffer, tr::DepthBuffer& depthBuffer);
	
	private:
		tr::Primitive m_primitive;
		Matrix4       m_matrix;
	};
}