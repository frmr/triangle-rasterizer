#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>
#include <vector>

#include "trFrameBuffer.hpp"
#include "trMat4.hpp"
#include "trTexture.hpp"
#include "trVec4.hpp"

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

    void Draw(const tr::DrawMode mode, const vector<tr::Vec4d>& vertices, const vector<size_t>& indices, const tr::Texture& texture, const tr::Mat4d& modelViewMatrix, const tr::Mat4d& projectionMatrix, tr::FrameBuffer& fb)
    {
//        modelViewMatrix.Print();
//        projectionMatrix.Print();
        vector<tr::Vec4d> transformedVertices;
        transformedVertices.reserve(vertices.size());
        for (const auto& vertex : vertices)
        {
            transformedVertices.emplace_back(modelViewMatrix * projectionMatrix * vertex);
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

        //viewport transformation
        for (const auto& vertex : transformedVertices)
        {

            //const double vx = (vertex.x+1) * 800.0 / 2.0;
            //const double vy = (vertex.y+1) * 600.0 / 2.0;
            const double vx = (800.0 / 2.0) * vertex.x + (800.0 / 2.0);
            const double vy = (600.0 / 2.0) * vertex.y + (600.0 / 2.0);


            clippedVertices.emplace_back(vx, vy, vertex.z, vertex.w);

            //vertex.Print();
            //std::cout << vx << " " << vy << std::endl;
            fb.colorBuffer.At(floor(vx), 600-floor(vy)) = std::numeric_limits<uint32_t>::max();
        }

        if (mode == tr::DrawMode::LINES)
        {
            //for (const auto& it = transformedVertices.begin(); it != transformedVertices.end(); ++it)
            for (int i = 1; i < transformedVertices.size(); ++i)
            {
                //DrawLine(lround(it->x), lround(it->y), lround((it+1)->x), lround((it+1)->y), fb,
                DrawLine(floor(transformedVertices[i-1].x), floor(transformedVertices[i-1].y), floor(transformedVertices[i].x), floor(transformedVertices[i].y), fb, std::numeric_limits<uint32_t>::max());
            }
        }

//        const size_t totalPixels = fb.width * fb.height;
//        for (size_t index = 0; index < totalPixels; ++index)
//        {
//            fb.colorBuffer.data[index] = texture.colorBuffer.data[index];
//        }
    }
}
