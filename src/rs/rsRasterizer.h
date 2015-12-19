#ifndef RS_RASTERIZER_H
#define RS_RASTERIZER_H

#include <vector>

#include "rsFrameBuffer.h"
#include "rsTexture.h"
#include "rsVec3.h"

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

    void Draw(const rs::DrawMode mode, const vector<rs::Vec3d>& vertices, const vector<size_t>& indices, const rs::Texture& texture, const double* const modelViewMatrix, const double* const projectionMatrix, rs::FrameBuffer& fb)
    {
//        for (size_t x = 0; x < fb.width; ++x)
//        {
//            for (size_t y = 0; y < fb.height; ++y)
//            {
//                const size_t index = y * fb.width + x;
//                fb.ColorAt(index) = texture.GetAt(index);
//            }
//        }
        size_t target = fb.width * fb.height;
        for (size_t index = 0; index < target; ++index)
        {
            fb.ColorAt(index) = texture.GetAt(index);
        }
    }
}

#endif // RS_RASTERIZER_H
