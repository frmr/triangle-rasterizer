#ifndef RS_RASTERIZER_H
#define RS_RASTERIZER_H

#include <limits>
#include <vector>

#include "rsFrameBuffer.h"
#include "rsMat4.h"
#include "rsTexture.h"
#include "rsVec4.h"

using std::vector;

namespace rs
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

    void Draw(const rs::DrawMode mode, const vector<rs::Vec4d>& vertices, const vector<size_t>& indices, const rs::Texture& texture, const rs::Mat4d& modelViewMatrix, const rs::Mat4d& projectionMatrix, rs::FrameBuffer& fb)
    {
//        modelViewMatrix.Print();
//        projectionMatrix.Print();
        vector<rs::Vec4d> transformedVertices;
        transformedVertices.reserve(vertices.size());
        for (const auto& vertex : vertices)
        {
            transformedVertices.emplace_back(modelViewMatrix * projectionMatrix * vertex);
            transformedVertices.back() /= transformedVertices.back().w;
            //(modelViewMatrix*projectionMatrix*vertex).Print();
        }

        //clip

        //viewport transformation
        for (const auto& vertex : transformedVertices)
        {
            const double vx = (vertex.x + 1) * 800 / 2;
            const double vy = (vertex.y + 1) * 600 / 2;
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

#endif // RS_RASTERIZER_H
