#ifndef TR_HPP
#define TR_HPP

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

    void Draw(const tr::DrawMode mode, const vector<tr::Vec4d>& vertices, const vector<size_t>& indices, const tr::Texture& texture, const tr::Mat4d& modelViewMatrix, const tr::Mat4d& projectionMatrix, tr::FrameBuffer& fb)
    {
//        modelViewMatrix.Print();
//        projectionMatrix.Print();
        vector<tr::Vec4d> transformedVertices;
        transformedVertices.reserve(vertices.size());
        for (const auto& vertex : vertices)
        {
            transformedVertices.emplace_back(modelViewMatrix * projectionMatrix * vertex);
            transformedVertices.back() /= transformedVertices.back().w;
            //(modelViewMatrix*projectionMatrix*vertex).Print();
        }

        //clip

//        std::cout << "Model view" << std::endl;
//        modelViewMatrix.Print();
//
//        std::cout << "Projection" << std::endl;
//        projectionMatrix.Print();



        //viewport transformation
        for (const auto& vertex : transformedVertices)
        {
            const double vx = (vertex.x+1) * 800.0 / 2.0;
            const double vy = (vertex.y+1) * 600.0 / 2.0;

            //vertex.Print();
            //std::cout << vx << " " << vy << std::endl;
            fb.colorBuffer.At(floor(vx), 600-floor(vy)) = std::numeric_limits<uint32_t>::max();
        }

//        const size_t totalPixels = fb.width * fb.height;
//        for (size_t index = 0; index < totalPixels; ++index)
//        {
//            fb.colorBuffer.data[index] = texture.colorBuffer.data[index];
//        }
    }
}

#endif
