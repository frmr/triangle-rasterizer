#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <vector>

#include "trCoord.hpp"
#include "trFrameBuffer.hpp"
#include "trMat4.hpp"
#include "trTexture.hpp"
#include "trVec4.hpp"
#include "../matrix/Matrices.h"

using std::vector;

namespace tr
{
    enum class DrawMode
    {
        POINTS,
        LINES,
        LINE_STRIP,
        LINE_LOOP,
        TRIANGLES,
        TRIANGLE_STRIP,
        TRIANGLE_FAN
    };

    //void DrawLine(int xa, int ya, int xb, int yb, tr::FrameBuffer& fb, const tr::Color& color)
    //{
    //    const int dx = abs(xb - xa);
    //    const int dy = abs(yb - ya);
    //    const int sx = xb >= xa ? 1 : -1;
    //    const int sy = yb >= ya ? 1 : -1;

    //    fb.ColorAt(xa, ya) = color;

    //    if (xa == xb && ya == yb)
    //    {
    //        return;
    //    }

    //    if (dy <= dx)
    //    {
    //        const int d1 = dy << 1;
    //        const int d2 = (dy - dx) << 1;
    //        int d = (dy << 1) - dx;
    //        int x = xa + sx;
    //        int y = ya;

    //        for (int i = 1; i <= dx; ++i, x += sx)
    //        {
    //            if (d > 0)
    //            {
    //                d += d2;
    //                y += sy;
    //            }
    //            else
    //            {
    //                d += d1;
    //            }

    //            fb.ColorAt(x, y) = color;
    //        }
    //    }
    //    else
    //    {
    //        const int d1 = dx << 1;
    //        const int d2 = (dx - dy) << 1;
    //        int d = (dx << 1) - dy;
    //        int x = xa;
    //        int y = ya + sy;

    //        for (int i = 1; i <= dy; ++i, y += sy)
    //        {
    //            if (d > 0)
    //            {
    //                d += d2;
    //                x += sx;
    //            }
    //            else
    //            {
    //                d += d1;
    //            }

    //            fb.ColorAt(x, y) = color;
    //        }
    //    }
    //}

	void GetLinePixels(const tr::Coord& start, const tr::Coord& end, vector<tr::Coord>& pixels)
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
			int d = (dy << 1) - dx;
			int x = start.x + sx;
			int y = start.y;

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
			int d = (dx << 1) - dy;
			int x = start.x;
			int y = start.y + sy;

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

	void DrawLine(const tr::Coord& start, const tr::Coord& end, tr::FrameBuffer& frameBuffer)
	{
		vector<tr::Coord> pixels;
		GetLinePixels(start, end, pixels);

		for (const auto& pixel : pixels)
		{
			frameBuffer.colorBuffer.At(pixel.x, pixel.y) = std::numeric_limits<uint32_t>::max();
		}
	}

	bool VertexInNdcCube(const Vector4 vertex)
	{
		return vertex.x >= -1.0f && vertex.x < 1.0f && vertex.y >= -1.0f && vertex.y < 1.0f && vertex.z >= -1.0f && vertex.z < 1.0f;
	}

	void NdcIntersectionX(const Vector4& inside, Vector4& outside, const float xValue)
	{
		const float alpha = (xValue - inside.x) / (outside.x - inside.x);

		outside.x = xValue;
		outside.y = inside.y + alpha * (outside.y - inside.y);
		//TODO: set z
	}

	void NdcIntersectionY(const Vector4& inside, Vector4& outside, const float yValue)
	{
		const float alpha = (yValue - inside.y) / (outside.y - inside.y);

		outside.y = yValue;
		outside.x = inside.x + alpha * (outside.x - inside.x);
		//TODO: set z
	}

	bool ClipNdcLine(Vector4& start, Vector4& end)
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
			NdcIntersectionX(end, start, -1.0f);
		}
		else if (start.x >= 1.0f)
		{
			NdcIntersectionX(end, start, 0.9999f);
		}
		
		if (end.x < -1.0f)
		{
			NdcIntersectionX(start, end, -1.0f);
		}
		else if (end.x >= 1.0f)
		{
			NdcIntersectionX(start, end, 0.9999f);
		}

		if (start.y < -1.0f)
		{
			NdcIntersectionY(end, start, -1.0f);
		}
		else if (start.y >= 1.0f)
		{
			NdcIntersectionY(end, start, 0.9999f);
		}

		if (end.y < -1.0f)
		{
			NdcIntersectionY(start, end, -1.0f);
		}
		else if (end.y >= 1.0f)
		{
			NdcIntersectionY(start, end, 0.9999f);
		}

		return true;
	}

	void DrawLine(Vector4 v0, Vector4 v1, const double halfWidth, const double halfHeight, tr::FrameBuffer& frameBuffer)
	{
		//clip
		if (!ClipNdcLine(v0, v1))
		{
			return;
		}

		//convert to screenspace
		const tr::Coord c0(int(halfWidth * v0.x + halfWidth), int(halfHeight * v0.y + halfHeight), v0.z);
		const tr::Coord c1(int(halfWidth * v1.x + halfWidth), int(halfHeight * v1.y + halfHeight), v1.z);

		//draw
		DrawLine(c0, c1, frameBuffer);
	}

    void Draw(const tr::DrawMode mode, vector<Vector4> vertices, const vector<size_t>& indices, const tr::Texture& texture, const Matrix4& modelViewProjectionMatrix, const int width, const int height, tr::FrameBuffer& frameBuffer)
    {
		assert(width > 0 && height > 0);

		vector<tr::Coord> screenCoords;
		screenCoords.reserve(vertices.size());

		const double halfWidth = double(width) / 2.0;
		const double halfHeight = double(height) / 2.0;

		for (auto& vertex : vertices)
		{
			vertex = modelViewProjectionMatrix * vertex;
			vertex /= vertex.w;
		}

		//vertices now contains ndc


		if (mode == tr::DrawMode::LINES)
		{
			for (vector<size_t>::const_iterator indexIt = indices.begin(); indexIt != indices.end(); indexIt += 2)
			{
				DrawLine(vertices[*indexIt], vertices[*(indexIt + 1)], halfWidth, halfHeight, frameBuffer);
			}
		}


        //for (auto& vertex : vertices)
		//{
		//	vertex = modelViewProjectionMatrix * vertex;
		//	vertex /= vertex.w; //normalized device coordinates (NDC)
		//
		//	screenCoords.emplace_back(std::floorl(halfWidth * vertex.x + halfWidth), std::floorl(halfHeight * vertex.y + halfHeight), vertex.z);
		//}
		//
		//if (mode == tr::DrawMode::POINTS)
		//{
		//	for (const auto& coord : screenCoords)
		//	{
		//		if (coord.x >= 0 && coord.x < width && coord.y >= 0 && coord.y < height && coord.depth >= 0.0 && coord.depth < 1.0)
		//			frameBuffer.colorBuffer.At(coord.x, coord.y) = std::numeric_limits<uint32_t>::max();
		//	}
		//}
		//else if (mode == tr::DrawMode::LINES)
		//{
		//	if (screenCoords.size() < 2 || screenCoords.size() % 2 != 0)
		//	{
		//		return;
		//	}
		//
		//	for (vector<tr::Coord>::const_iterator it = screenCoords.begin(); it != screenCoords.end(); it += 2)
		//	{
		//		//clip line
		//		//convert to screenspace
		//		DrawLine(*it, *(it + 1), width, height, frameBuffer);
		//	}
		//}
		//else if (mode == tr::DrawMode::LINE_STRIP)
		//{
		//	if (screenCoords.size() < 2)
		//	{
		//		return;
		//	}
		//
		//	for (vector<tr::Coord>::const_iterator it = screenCoords.begin(); it != screenCoords.end() - 1; ++it)
		//	{
		//		DrawLine(*it, *(it + 1), width, height, frameBuffer);
		//	}
		//}
		//else if (mode == tr::DrawMode::LINE_LOOP)
		//{
		//	if (screenCoords.size() < 2)
		//	{
		//		return;
		//	}
		//
		//	for (vector<tr::Coord>::const_iterator it = screenCoords.begin(); it != screenCoords.end() - 1; ++it)
		//	{
		//		DrawLine(*it, *(it + 1), width, height, frameBuffer);
		//	}
		//
		//	DrawLine(*(screenCoords.end() - 1), *screenCoords.begin(), width, height, frameBuffer);
		//}
		//else if (mode == tr::DrawMode::TRIANGLES)
		//{
		//
		//}
		//else if (mode == tr::DrawMode::TRIANGLE_STRIP)
		//{
		//
		//}
		//else if (mode == tr::DrawMode::TRIANGLE_FAN)
		//{
		//
		//}
		//else
		//{
		//	assert(false);
		//}

		

        //if (mode == tr::DrawMode::LINES)
        //{
        //    //for (const auto& it = transformedVertices.begin(); it != transformedVertices.end(); ++it)
        //    for (int i = 1; i < transformedVertices.size(); ++i)
        //    {
        //        //DrawLine(lround(it->x), lround(it->y), lround((it+1)->x), lround((it+1)->y), fb,
        //        DrawLine(floorl(clippedVertices[i-1].x), floorl(clippedVertices[i-1].y), floorl(clippedVertices[i].x), floorl(clippedVertices[i].y), fb, std::numeric_limits<uint32_t>::max());
        //    }
        //}

//        const size_t totalPixels = fb.width * fb.height;
//        for (size_t index = 0; index < totalPixels; ++index)
//        {
//            fb.colorBuffer.data[index] = texture.colorBuffer.data[index];
//        }
    }
}
