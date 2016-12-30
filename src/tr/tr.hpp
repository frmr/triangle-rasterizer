#pragma once

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <vector>

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

    void DrawLine(int xa, int ya, int xb, int yb, tr::FrameBuffer& fb, const tr::Color& color)
    {
        const int dx = abs(xb - xa);
        const int dy = abs(yb - ya);
        const int sx = xb >= xa ? 1 : -1;
        const int sy = yb >= ya ? 1 : -1;

        fb.ColorAt(xa, ya) = color;

        if (xa == xb && ya == yb)
        {
            return;
        }

        if (dy <= dx)
        {
            const int d1 = dy << 1;
            const int d2 = (dy - dx) << 1;
            int d = (dy << 1) - dx;
            int x = xa + sx;
            int y = ya;

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

                fb.ColorAt(x, y) = color;
            }
        }
        else
        {
            const int d1 = dx << 1;
            const int d2 = (dx - dy) << 1;
            int d = (dx << 1) - dy;
            int x = xa;
            int y = ya + sy;

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

                fb.ColorAt(x, y) = color;
            }
        }
    }

    void Draw(const tr::DrawMode mode, const vector<Vector4>& vertices, const vector<size_t>& indices, const tr::Texture& texture, const Matrix4& modelViewProjectionMatrix, const size_t width, const size_t height, tr::FrameBuffer& fb)
    {
		vector<Vector4> transformedVertices;
		transformedVertices.reserve(vertices.size());

        for (const auto& vertex : vertices)
		{
			transformedVertices.emplace_back(modelViewProjectionMatrix * vertex);
			//transformedVertices.emplace_back(projectionMatrix * modelViewMatrix * vertex);
			transformedVertices.back() /= transformedVertices.back().w; //normalized device coordinates (NDC)
			//(modelViewMatrix*projectionMatrix*vertex).Print();
		}

        //clip

//        std::cout << "Model view" << std::endl;
//        modelViewMatrix.Print();
//
//        std::cout << "Projection" << std::endl;
//        projectionMatrix.Print();


        vector<tr::Vec4d> clippedVertices;
		clippedVertices.reserve(vertices.size());

		const double halfWidth = double(width) / 2.0;
		const double halfHeight = double(height) / 2.0;

        //viewport transformation
        for (const auto& vertex : transformedVertices)
        {
			const double vx = halfWidth * vertex.x + halfWidth;
			const double vy = halfHeight * vertex.y + halfHeight;

			//std::cout << vertex.x << ", " << vertex.y << ", " << vertex.z << std::endl;

			if (vx < 0 || vx >= width || vy < 0 || vy >= height || vertex.z < 0.0 || vertex.z > 1.0)
				continue;

			fb.colorBuffer.At(floorl(vx), floorl(vy)) = std::numeric_limits<uint32_t>::max();

			clippedVertices.emplace_back(vx, vy, vertex.z, vertex.w);

            //vertex.Print();
            //std::cout << vx << " " << vy << std::endl;

			//if (clippedVertices.back().z > 0.0)
				//fb.colorBuffer.At(floorl(vx), floorl(vy)) = std::numeric_limits<uint32_t>::max();
        }
		//std::cout << std::endl;

        if (mode == tr::DrawMode::LINES)
        {
            //for (const auto& it = transformedVertices.begin(); it != transformedVertices.end(); ++it)
            for (int i = 1; i < transformedVertices.size(); ++i)
            {
                //DrawLine(lround(it->x), lround(it->y), lround((it+1)->x), lround((it+1)->y), fb,
                DrawLine(floorl(clippedVertices[i-1].x), floorl(clippedVertices[i-1].y), floorl(clippedVertices[i].x), floorl(clippedVertices[i].y), fb, std::numeric_limits<uint32_t>::max());
            }
        }

//        const size_t totalPixels = fb.width * fb.height;
//        for (size_t index = 0; index < totalPixels; ++index)
//        {
//            fb.colorBuffer.data[index] = texture.colorBuffer.data[index];
//        }
    }
}
