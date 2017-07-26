#include "trRasterizer.hpp"
#include "trCoord.hpp"

#include <assert.h>

void tr::Rasterizer::draw(const tr::Primitive primitive, std::vector<Vertex> vertices, const tr::ColorBuffer& texture, const Matrix4& modelViewProjectionMatrix, const int width, const int height, tr::ColorBuffer& colorBuffer, tr::DepthBuffer& depthBuffer)
{
	assert(width > 0 && height > 0);

	std::vector<tr::Coord> screenCoords;
	screenCoords.reserve(vertices.size());

	const float halfWidth = width / 2.0f;
	const float halfHeight = height / 2.0f;

	//transform vertices from world space to ndc
	for (auto& vertex : vertices)
	{
		vertex.position = modelViewProjectionMatrix * vertex.position;
		vertex.position /= vertex.position.w;
	}

	if (primitive == tr::Primitive::LINES)
	{
		for (std::vector<Vertex>::const_iterator it = vertices.begin(); it != vertices.end(); it += 2)
		{
			drawLine(it->position, (it + 1)->position, halfWidth, halfHeight, colorBuffer, depthBuffer);
		}
	}
	else if (primitive == tr::Primitive::LINE_STRIP)
	{
		for (std::vector<Vertex>::const_iterator it = vertices.begin(); it != vertices.end() - 1; ++it)
		{
			drawLine(it->position, (it + 1)->position, halfWidth, halfHeight, colorBuffer, depthBuffer);
		}
	}
	else if (primitive == tr::Primitive::LINE_LOOP)
	{
		for (std::vector<Vertex>::const_iterator it = vertices.begin(); it != vertices.end() - 1; ++it)
		{
			drawLine(it->position, (it + 1)->position, halfWidth, halfHeight, colorBuffer, depthBuffer);
		}

		drawLine(vertices.back().position, vertices.front().position, halfWidth, halfHeight, colorBuffer, depthBuffer);
	}
	else if (primitive == tr::Primitive::TRIANGLES)
	{
		for (std::vector<Vertex>::const_iterator it = vertices.begin(); it != vertices.end(); it += 3)
		{
			drawTriangle(it->position, (it + 1)->position, (it + 2)->position, halfWidth, halfHeight, colorBuffer, depthBuffer);
		}
	}
}

void tr::Rasterizer::getLinePixels(const tr::Coord& start, const tr::Coord& end, std::vector<tr::Coord>& pixels)
{
	const int dx = abs(end.x - start.x);
	const int dy = abs(end.y - start.y);

	pixels.reserve(dx < dy ? dy : dx);

	const int sx = end.x >= start.x ? 1 : -1;
	const int sy = end.y >= start.y ? 1 : -1;

	pixels.push_back(start);

	if (start.x == end.x && start.y == end.y)
	{
		return;
	}

	if (dy <= dx)
	{
		const int d1 = dy << 1;
		const int d2 = (dy - dx) << 1;
		int       d = (dy << 1) - dx;
		int       x = start.x + sx;
		int       y = start.y;

		for (int i = 1; i <= dx; ++i, x += sx)
		{
			if (d > 0)
			{
				d += d2;
				y += sy;
			}
			else
			{
				d += d1;
			}

			pixels.push_back(tr::Coord(x, y, start.depth));
		}
	}
	else
	{
		const int d1 = dx << 1;
		const int d2 = (dx - dy) << 1;
		int       d  = (dx << 1) - dy;
		int       x  = start.x;
		int       y  = start.y + sy;

		for (int i = 1; i <= dy; ++i, y += sy)
		{
			if (d > 0)
			{
				d += d2;
				x += sx;
			}
			else
			{
				d += d1;
			}

			pixels.push_back(tr::Coord(x, y, start.depth));
		}
	}
}

bool tr::Rasterizer::vertexInNdcCube(const Vector4 vertex)
{
	return vertex.x >= -1.0f && vertex.x < 1.0f && vertex.y >= -1.0f && vertex.y < 1.0f && vertex.z >= -1.0f && vertex.z < 1.0f;
}

bool tr::Rasterizer::clipNdcLine(Vector4& start, Vector4& end)
{
	if ((start.x < -1.0f && end.x < -1.0f) || (start.y < -1.0f && end.y < -1.0f) || (start.z < -1.0f && end.z < -1.0f))
	{
		return false;
	}

	if ((start.x >= 1.0f && end.x >= 1.0f) || (start.y >= 1.0f && end.y > 1.0f) || (start.z > 1.0f && end.z > 1.0f))
	{
		return false;
	}

	if (start.x < -1.0f)
	{
		ndcIntersectionX(end, start, -1.0f);
	}
	else if (start.x >= 1.0f)
	{
		ndcIntersectionX(end, start, 0.9999f);
	}

	if (end.x < -1.0f)
	{
		ndcIntersectionX(start, end, -1.0f);
	}
	else if (end.x >= 1.0f)
	{
		ndcIntersectionX(start, end, 0.9999f);
	}

	if (start.y < -1.0f)
	{
		ndcIntersectionY(end, start, -1.0f);
	}
	else if (start.y >= 1.0f)
	{
		ndcIntersectionY(end, start, 0.9999f);
	}

	if (end.y < -1.0f)
	{
		ndcIntersectionY(start, end, -1.0f);
	}
	else if (end.y >= 1.0f)
	{
		ndcIntersectionY(start, end, 0.9999f);
	}

	return true;
}

void tr::Rasterizer::ndcIntersectionX(const Vector4& inside, Vector4& outside, const float xValue)
{
	const float alpha = (xValue - inside.x) / (outside.x - inside.x);

	outside.x = xValue;
	outside.y = inside.y + alpha * (outside.y - inside.y);
	//TODO: set z
}

void tr::Rasterizer::ndcIntersectionY(const Vector4& inside, Vector4& outside, const float yValue)
{
	const float alpha = (yValue - inside.y) / (outside.y - inside.y);

	outside.y = yValue;
	outside.x = inside.x + alpha * (outside.x - inside.x);
	//TODO: set z
}

void tr::Rasterizer::drawLine(const tr::Coord& start, const tr::Coord& end, tr::ColorBuffer& colorBuffer, tr::DepthBuffer& depthBuffer)
{
	std::vector<tr::Coord> pixels;
	getLinePixels(start, end, pixels);

	for (const auto& pixel : pixels)
	{
		colorBuffer.at(pixel.x, pixel.y) = std::numeric_limits<uint32_t>::max();
	}
}

void tr::Rasterizer::drawLine(Vector4 v0, Vector4 v1, const float halfWidth, const float halfHeight, tr::ColorBuffer& colorBuffer, tr::DepthBuffer& depthBuffer)
{
	//clip
	if (!clipNdcLine(v0, v1))
	{
		return;
	}

	//convert to screenspace
	const tr::Coord c0(int(halfWidth * v0.x + halfWidth), int(halfHeight * v0.y + halfHeight), v0.z);
	const tr::Coord c1(int(halfWidth * v1.x + halfWidth), int(halfHeight * v1.y + halfHeight), v1.z);

	//draw
	drawLine(c0, c1, colorBuffer, depthBuffer);
}

void tr::Rasterizer::drawTriangle(Vector4 v0, Vector4 v1, Vector4 v2, const float halfWidth, const float halfHeight, tr::ColorBuffer& colorBuffer, tr::DepthBuffer& depthBuffer)
{
	//if all vertices in
	//draw one triangle
	//else if all vertices out
	//discard
	//else if 
}
